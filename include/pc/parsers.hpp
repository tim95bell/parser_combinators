
#pragma once

#include <pc/pc.hpp>
#include <pc/combinators.hpp>
#include <string_view>

namespace pc::parsers {
    auto character(std::string_view input) -> Result<char>;
    static_assert(AnyParser<decltype(character)>);
    auto newline(std::string_view input) -> Result<char>;
    static_assert(AnyParser<decltype(newline)>);
    auto line(std::string_view input) -> Result<std::string>;
    static_assert(AnyParser<decltype(line)>);

    template <typename T>
    inline auto fail(std::string_view input) -> Result<T> {
        return failure;
    }
    static_assert(AnyParser<decltype(fail<char>)>);

    inline auto tag(std::string_view prefix) -> Parser<std::string> auto {
        return [prefix](std::string_view input) -> Result<std::string> {
            if (input.starts_with(prefix)) {
                return success<std::string>(std::string(prefix), input.substr(prefix.size()));
            }
            return failure;
        };
    }

    inline auto tag(char prefix) -> Parser<char> auto {
        return [prefix](std::string_view input) -> Result<char> {
            if (!input.empty() && input.at(0) == prefix) {
                return success(prefix, input.substr(1));
            }
            return failure;
        };
    }

    auto unit(auto value) -> Parser<decltype(value)> auto {
        return [value](std::string_view input) -> Result<decltype(value)> {
            return success(value, input);
        };
    }
    static_assert(Combinator<decltype(unit<char>), char>);

    auto first_char_match(std::predicate<char> auto fn) -> Parser<char> auto {
        return [fn](std::string_view input) -> Result<char> {
            auto iter = std::ranges::find_if(input, fn);
            if (iter != input.end()) {
                return success(*iter, input.substr((iter - input.begin()) + 1));
            }
            return failure;
        };
    }

    auto last_char_match(std::predicate<char> auto fn) -> Parser<char> auto {
        return [fn](std::string_view input) -> Result<char> {
            auto iter = std::ranges::find_if(input.rbegin(), input.rend(), fn);
            if (iter != input.rend()) {
                auto pos = std::distance(iter, input.rend()) - 1;
                return success(*iter, input.substr(0, pos));
            }
            return failure;
        };
    }
} // namespace pc::parsers
