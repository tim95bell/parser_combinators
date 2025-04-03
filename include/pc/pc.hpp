
#pragma once

#include <string_view>
#include <type_traits>
#include <utility>

namespace pc {
    // #region types
    template <typename T>
    using Result = std::optional<std::pair<T, std::string_view>>;

    template <typename P>
    using ParserResult = std::invoke_result_t<P, std::string_view>;

    template <typename P>
    using ParserValueType = ParserResult<P>::value_type::first_type;
    // #endregion

    // #region concepts
    template <typename P, typename ValueType>
    concept Parser = std::invocable<P, std::string_view> &&
        std::same_as<ParserValueType<P>, ValueType>;

    template <typename P>
    concept AnyParser = std::invocable<P, std::string_view> &&
        std::same_as<ParserResult<P>, Result<ParserValueType<P>>>;

    template <typename P, typename Other>
    concept SameParser = AnyParser<P> &&
        AnyParser<Other> &&
        std::same_as<ParserValueType<P>, ParserValueType<Other>>;

    template <typename P, typename Other>
    concept WrappedParser = AnyParser<P> &&
        AnyParser<Other> &&
        std::same_as<typename ParserValueType<P>::value_type, ParserValueType<Other>>;

    template <typename C, typename ...Args>
    concept Combinator = std::invocable<C, Args...>
        && AnyParser<std::invoke_result_t<C, Args...>>;
    // #endregion

    // #region helpers
    template <typename T>
    Result<T> success(T&& value, std::string_view input) {
        return {{std::forward<T>(value), input}};
    }

    static constexpr const auto failure = std::nullopt;

    bool is_digit(char c);
    // #endregion
}
