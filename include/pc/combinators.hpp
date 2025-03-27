
#pragma once

#include <pc/pc.hpp>
#include <array>
#include <string_view>
#include <ranges>

namespace pc::combinators {
    template <std::size_t Count>
    auto manyn(auto parser) {
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

    auto trim(auto parser) {
        return [parser](std::string_view input) -> ParserResult<decltype(parser)> {
            input.remove_prefix(std::min(input.find_first_not_of("\n\t "), input.size()));
            input.remove_suffix(std::min(input.size() - (input.find_last_not_of("\n\t ") + 1), input.size()));
            return std::invoke(parser, input);
        };
    }

    namespace {
        auto choice_helper(std::string_view input, auto parser, auto... parsers) {
            using ParserValueType = std::common_type_t<ParserValueType<decltype(parser)>, ParserValueType<decltype(parsers)>...>;
            if (Result<ParserValueType> result = std::invoke(parser, input)) {
                return result;
            }

            if constexpr (sizeof...(parsers) == 0) {
                return Result<ParserValueType>{};
            } else {
                return choice_helper(input, parsers...);
            }
        }
    }

    auto choice(auto... parsers) {
        return [parsers...](std::string_view input) {
            return choice_helper(input, parsers...);
        };
    }

    auto many0_to_many1(auto parser) {
        return [parser](std::string_view input) -> ParserResult<decltype(parser)> {
            auto result = std::invoke(parser, input);
            if (result && result.value().first.empty()) {
                return failure;
            }

            return result;
        };
    }

    auto many0(auto parser) {
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

    auto many1(auto parser) {
        return many0_to_many1(many0(parser));
    }

    auto many_seperated_by0(auto parser, auto seperator) {
        using Parser = decltype(parser);
        return [parser, seperator](std::string_view input) -> ParserResult<Parser> {
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

    auto many_seperated_by1(auto parser, auto seperator) {
        return many0_to_many1(many_seperated_by0(parser, seperator));
    }

    auto many_split_by0(auto parser, std::string_view seperator) {
        using Parser = decltype(parser);
        using ValueType = ParserValueType<Parser>;
        return [parser, seperator](std::string_view input) -> Result<std::vector<ValueType>> {
            std::vector<ValueType> result;
            auto lines = input | std::views::split(seperator);
            for (auto line : lines) {
                if (auto r = std::invoke(parser, std::string_view(line.begin(), line.end())); r && r->second.empty()) {
                    result.push_back(r->first);
                } else {
                    return failure;
                }
            }
            return success(result, std::string_view());
        };
    }

    auto many_split_by1(auto parser, std::string_view seperator) {
        return many0_to_many1(many_split_by0(parser, seperator));
    }

    auto pair(auto lhs, auto rhs) {
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

    auto seperated_pair(auto lhs, auto sep, auto rhs) {
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
        auto tuple_helper(std::string_view input, auto parser, auto... rest) {
            using Parser = decltype(parser);
            if (auto result = std::invoke(parser, input)) {
                auto x = std::make_tuple(result->first);
                if constexpr(sizeof...(rest) == 0) {
                    return std::optional{std::pair{x, result->second}};
                } else {
                    auto xs = tuple_helper(result->second, rest...);
                    if (xs.has_value()) {
                        return std::optional{std::pair{std::tuple_cat(x, xs.value().first), xs.value().second}};
                    }
                }
            }

            using X = std::tuple<ParserValueType<Parser>>;
            if constexpr(sizeof...(rest) == 0) {
                return std::optional<std::pair<X, std::string_view>>{};
            } else {
                using Xs = std::invoke_result_t<decltype(tuple_helper<decltype(rest)...>), std::string_view, decltype(rest)...>::value_type::first_type;
                using XAndXs = std::invoke_result_t<decltype(std::tuple_cat<X, Xs>), X, Xs>;
                return std::optional<std::pair<XAndXs, std::string_view>>{};
            }
        }
    }

    auto tuple(auto... parsers) {
        return [parsers...](std::string_view input) {
            return tuple_helper(input, parsers...);
        };
    }

    auto map(auto parser, auto fn) {
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

    auto filter(auto parser, auto predicate) {
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
