
#include <pc/pc.hpp>
#include <pc/parsers.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string_view>

using namespace pc::parsers;
using namespace pc;
using namespace std::literals::string_view_literals;

TEST_CASE("character", "[parsers]") {
    SECTION("empty input") {
        const auto result = character(""sv);
        REQUIRE(!result.has_value());
    }

    SECTION("one character input") {
        const auto result = character("a"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second.empty());
    }

    SECTION("two character input") {
        const auto result = character("ab"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "b"sv);
    }

    SECTION("many character input") {
        const auto result = character("hello"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'h');
        CHECK(result.value().second == "ello"sv);
    }
}

TEST_CASE("newline", "[parsers]") {
    SECTION("empty input") {
        const auto result = newline(""sv);
        REQUIRE(!result.has_value());
    }

    SECTION("one character input, no match") {
        const auto result = newline("a"sv);
        REQUIRE(!result.has_value());
    }

    SECTION("one character input, match") {
        const auto result = newline("\n"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == '\n');
        CHECK(result.value().second.empty());
    }

    SECTION("two character input, no match") {
        const auto result = newline("a\n"sv);
        REQUIRE(!result.has_value());
    }

    SECTION("two character input, match") {
        const auto result = newline("\na"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == '\n');
        CHECK(result.value().second == "a"sv);
    }

    SECTION("two character input, both newline, match") {
        const auto result = newline("\n\n"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == '\n');
        CHECK(result.value().second == "\n"sv);
    }

    SECTION("many character input, no match") {
        const auto result = newline("hel\nlo\n"sv);
        REQUIRE(!result.has_value());
    }

    SECTION("many character input, match") {
        const auto result = newline("\nhel\nlo\n"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == '\n');
        CHECK(result.value().second == "hel\nlo\n"sv);
    }
}

TEST_CASE("line", "[parsers]") {
    SECTION("empty input") {
        const auto result = line(""sv);
        REQUIRE(!result.has_value());
    }

    SECTION("one character input") {
        const auto result = line("a"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == "a"sv);
        CHECK(result.value().second.empty());
    }

    SECTION("one character input, newline character") {
        const auto result = line("\n"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == ""sv);
        CHECK(result.value().second.empty());
    }

    SECTION("many character input") {
        const auto result = line("hello"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second.empty());
    }

    SECTION("many character input, ending in newline") {
        const auto result = line("hello\n"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second.empty());
    }

    SECTION("many character input, newline in middle") {
        const auto result = line("hello\nworld"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many character input, starting with newline") {
        const auto result = line("\nhello"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == ""sv);
        CHECK(result.value().second == "hello"sv);
    }
}
