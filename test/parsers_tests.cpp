
#include <pc/pc.hpp>
#include <pc/parsers.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string_view>

using namespace pc::parsers;
using namespace pc;
using namespace std::literals::string_view_literals;

TEST_CASE("character", "[parsers]") {
    SECTION("empty input") {
        auto result = character(""sv);
        CHECK(!result.has_value());
    }

    SECTION("one character input") {
        auto result = character("a"sv);
        CHECK(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second.empty());
    }

    SECTION("two character input") {
        auto result = character("ab"sv);
        CHECK(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "b"sv);
    }

    SECTION("many character input") {
        auto result = character("hello"sv);
        CHECK(result.has_value());
        CHECK(result.value().first == 'h');
        CHECK(result.value().second == "ello"sv);
    }
}
