
#pragma once

#include <pc/pc.hpp>
#include <array>
#include <string_view>
#include <ranges>

namespace pc::combinators {
    template <std::size_t Count>
    auto manyn(AnyParser auto parser)
    -> Parser<std::array<ParserValueType<decltype(parser)>, Count>> auto {
        using Element = ParserValueType<decltype(parser)>;
        return [parser](std::string_view input) -> Result<std::array<Element, Count>> {
            std::array<Element, Count> result;
            for (Element& x : result) {
                if (auto p = std::invoke(parser, input)) {
                    input = p->second;
                    x = p->first;
                } else {
                    return failure;
                }
            }
            return success(result, input);
        };
    }

    auto trim(AnyParser auto parser) -> SameParser<decltype(parser)> auto {
        return [parser](std::string_view input) -> ParserResult<decltype(parser)> {
            input.remove_prefix(std::min(input.find_first_not_of("\n\t "), input.size()));
            input.remove_suffix(std::min(input.size() - (input.find_last_not_of("\n\t ") + 1), input.size()));
            return std::invoke(parser, input);
        };
    }

    namespace {
        auto choice_helper(std::string_view input, AnyParser auto parser, AnyParser auto... parsers)
        -> Result<std::common_type_t<ParserValueType<decltype(parser)>, ParserValueType<decltype(parsers)>...>>
        {
            if (auto result = std::invoke(parser, input)) {
                return result;
            }

            if constexpr (sizeof...(parsers) == 0) {
                return failure;
            } else {
                return choice_helper(input, parsers...);
            }
        }
    }

    auto choice(AnyParser auto... parsers) -> Parser<std::common_type_t<ParserValueType<decltype(parsers)>...>> auto {
        return [parsers...](std::string_view input) {
            return choice_helper(input, parsers...);
        };
    }

    auto many0_to_many1(AnyParser auto parser) -> SameParser<decltype(parser)> auto {
        return [parser](std::string_view input) -> ParserResult<decltype(parser)> {
            auto result = std::invoke(parser, input);
            if (result && result.value().first.empty()) {
                return failure;
            }

            return result;
        };
    }

    auto many0(AnyParser auto parser) -> Parser<std::vector<ParserValueType<decltype(parser)>>> auto {
        using Parser = decltype(parser);
        return [parser](std::string_view input) -> Result<std::vector<ParserValueType<Parser>>> {
            std::string_view rest = input;
            std::vector<ParserValueType<Parser>> result;
            while (auto r = std::invoke(parser, rest)) {
                result.push_back(r->first);
                rest = r->second;
            }
            return success(result, rest);
        };
    }

    auto many1(AnyParser auto parser) -> Parser<std::vector<ParserValueType<decltype(parser)>>> auto {
        return many0_to_many1(many0(parser));
    }

    auto many_seperated_by0(AnyParser auto parser, AnyParser auto seperator) -> Parser<std::vector<ParserValueType<decltype(parser)>>> auto {
        using Parser = decltype(parser);
        return [parser, seperator](std::string_view input) -> Result<std::vector<ParserValueType<Parser>>> {
            std::vector<ParserValueType<Parser>> result;
            std::string_view rest = input;
            if (auto r = std::invoke(parser, rest)) {
                result.push_back(r->first);
                rest = r->second;
            }

            while (auto s = std::invoke(seperator, rest)) {
                if (auto r = std::invoke(parser, s->second)) {
                    result.push_back(r->first);
                    rest = r->second;
                } else {
                    break;
                }
            }
            return success(result, rest);
        };
    }

    auto many_seperated_by1(AnyParser auto parser, AnyParser auto seperator) -> Parser<std::vector<ParserValueType<decltype(parser)>>> auto {
        return many0_to_many1(many_seperated_by0(parser, seperator));
    }

    auto many_split_by0(AnyParser auto parser, std::string_view seperator) -> Parser<std::vector<ParserValueType<decltype(parser)>>> auto {
        using Parser = decltype(parser);
        using ValueType = ParserValueType<Parser>;
        return [parser, seperator](std::string_view input) -> Result<std::vector<ValueType>> {
            std::vector<ValueType> result;
            auto lines = input | std::views::split(seperator);
            if (lines.empty()) {
                if (auto r = std::invoke(parser, input); r && r->second.empty()) {
                    result.push_back(r->first);
                } else {
                    return failure;
                }
            } else {
                for (auto line : lines) {
                    if (auto r = std::invoke(parser, std::string_view(line.begin(), line.end())); r && r->second.empty()) {
                        result.push_back(r->first);
                    } else {
                        return failure;
                    }
                }
            }
            return success(result, std::string_view());
        };
    }

    auto many_split_by1(AnyParser auto parser, std::string_view seperator) -> Parser<std::vector<ParserValueType<decltype(parser)>>> auto {
        return many0_to_many1(many_split_by0(parser, seperator));
    }

    auto pair(AnyParser auto lhs, AnyParser auto rhs)
    -> Parser<std::pair<ParserValueType<decltype(lhs)>, ParserValueType<decltype(rhs)>>> auto {
        using Lhs = decltype(lhs);
        using Rhs = decltype(rhs);
        return [lhs, rhs](std::string_view input) -> Result<std::pair<ParserValueType<Lhs>, ParserValueType<Rhs>>> {
            if (auto l = std::invoke(lhs, input)) {
                if (auto r = std::invoke(rhs, l->second)) {
                    return success(std::pair{l->first, r->first}, r->second);
                }
            }
            return failure;
        };
    }

    auto seperated_pair(AnyParser auto lhs, AnyParser auto sep, AnyParser auto rhs) -> Parser<std::pair<ParserValueType<decltype(lhs)>, ParserValueType<decltype(rhs)>>> auto {
        using Lhs = decltype(lhs);
        using Rhs = decltype(rhs);
        return [lhs, rhs, sep](std::string_view input) -> Result<std::pair<ParserValueType<Lhs>, ParserValueType<Rhs>>> {
            if (auto l = std::invoke(lhs, input)) {
                if (auto s = std::invoke(sep, l->second)) {
                    if (auto r = std::invoke(rhs, s->second)) {
                        return success(std::pair{l->first, r->first}, r->second);
                    }
                }
            }
            return failure;
        };
    }

    namespace {
        auto tuple_helper(std::string_view input, AnyParser auto parser, AnyParser auto... rest) -> Result<std::tuple<ParserValueType<decltype(parser)>, ParserValueType<decltype(rest)>...>> {
            if (auto result = std::invoke(parser, input)) {
                auto x = std::make_tuple(result->first);
                if constexpr(sizeof...(rest) == 0) {
                    return success(x, result->second);
                } else {
                    auto xs = tuple_helper(result->second, rest...);
                    if (xs.has_value()) {
                        return success(std::tuple_cat(x, xs.value().first), xs.value().second);
                    }
                }
            }

            return failure;
        }
    }

    auto tuple(AnyParser auto... parsers) -> Parser<std::tuple<ParserValueType<decltype(parsers)>...>> auto {
        return [parsers...](std::string_view input) {
            return tuple_helper(input, parsers...);
        };
    }

    auto map(AnyParser auto parser, std::invocable<ParserValueType<decltype(parser)>> auto fn) -> Parser<std::invoke_result_t<decltype(fn), ParserValueType<decltype(parser)>>> auto {
        using Parser = decltype(parser);
        using Fn = decltype(fn);
        using FnReturnType = std::invoke_result_t<Fn, ParserValueType<Parser>>;
        return [parser, fn](std::string_view input) -> Result<FnReturnType> {
            if (auto result = std::invoke(parser, input)) {
                return success(std::invoke(fn, result->first), result->second);
            }
            return failure;
        };
    }

    auto filter(AnyParser auto parser, std::predicate<ParserValueType<decltype(parser)>> auto predicate) -> SameParser<decltype(parser)> auto {
        using Parser = decltype(parser);
        return [parser, predicate](std::string_view input) -> ParserResult<Parser> {
            if (auto result = std::invoke(parser, input)) {
                if (predicate(result->first)) {
                    return success(result->first, result->second);
                }
            }
            return failure;
        };
    }
} // namespace pc::combinators
