
#pragma once

#include <string_view>
#include <type_traits>
#include <utility>

namespace pc {
    // #region types
    template <typename T>
    using Result = std::optional<std::pair<T, std::string_view>>;

    template <typename Parser>
    using ParserResult = std::invoke_result_t<Parser, std::string_view>;

    template <typename Parser>
    using ParserValueType = ParserResult<Parser>::value_type::first_type;
    // #endregion

    // #region helpers
    template <typename T>
    Result<T> success(T&& value, std::string_view input) {
        return {{std::forward<T>(value), std::move(input)}};
    }

    static constexpr const auto failure = std::nullopt;

    bool is_digit(char c);
    // #endregion
}
