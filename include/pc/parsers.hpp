
#pragma once

#include <pc/pc.hpp>
#include <pc/combinators.hpp>
#include <string_view>

namespace pc::parsers {
    Result<char> character(std::string_view input);
    Result<char> newline(std::string_view input);
    Result<std::string> line(std::string_view input);

    template <typename T>
    inline Result<T> fail(std::string_view input) {
        return failure;
    }

    inline auto tag(std::string_view prefix) {
        return [prefix](std::string_view input) -> Result<std::string> {
            if (input.starts_with(prefix)) {
                return success<std::string>(std::string(prefix), input.substr(prefix.size()));
            }
            return failure;
        };
    }

    inline auto tag(char prefix) {
        return [prefix](std::string_view input) -> Result<char> {
            if (!input.empty() && input.at(0) == prefix) {
                return success(prefix, input.substr(1));
            }
            return failure;
        };
    }

    auto unit(auto value) {
        return [value](std::string_view input) -> Result<decltype(value)> {
            return success(value, input);
        };
    }

    auto first_char_match(std::predicate<char> auto fn) {
        return [fn](std::string_view input) -> Result<char> {
            auto iter = std::ranges::find_if(input, fn);
            if (iter != input.end()) {
                return success(*iter, input.substr((iter - input.begin()) + 1));
            }
            return failure;
        };
    }

    auto last_char_match(std::predicate<char> auto fn) {
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
