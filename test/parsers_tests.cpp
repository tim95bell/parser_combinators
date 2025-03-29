
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

TEST_CASE("tag", "[parsers]") {
    SECTION("empty input") {
        const auto result = tag("hello")("");
        REQUIRE(!result.has_value());
    }

    SECTION("empty input, empty tag") {
        const auto result = tag("")("");
        REQUIRE(result.has_value());
        CHECK(result.value().first.empty());
        CHECK(result.value().second.empty());
    }

    SECTION("non matching input") {
        const auto result = tag("hello")("helo");
        REQUIRE(!result.has_value());
    }

    SECTION("non matching input, matching in middle") {
        const auto result = tag("hello")("helohellohelo");
        REQUIRE(!result.has_value());
    }

    SECTION("non matching input, matching at end") {
        const auto result = tag("hello")("helohello");
        REQUIRE(!result.has_value());
    }

    SECTION("non empty input, empty tag") {
        const auto result = tag("")("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first.empty());
        CHECK(result.value().second == "hello"sv);
    }

    SECTION("exactly matching input") {
        const auto result = tag("hello")("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second.empty());
    }

    SECTION("matching input, extra after") {
        const auto result = tag("hello")("hello123");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second == "123"sv);
    }

    SECTION("matching twice input") {
        const auto result = tag("hello")("hellohello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second == "hello"sv);
    }
}

TEST_CASE("unit", "[parsers]") {
    SECTION("unit<int> with empty input") {
        const auto result = unit(20)(""sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 20);
        CHECK(result.value().second.empty());
    }

    SECTION("unit<std::string> with empty input") {
        const auto result = unit(std::string("hello"))(""sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::string("hello"));
        CHECK(result.value().second.empty());
    }

    SECTION("unit<int> with non empty input") {
        const auto result = unit(20)("hello"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 20);
        CHECK(result.value().second == "hello"sv);
    }

    SECTION("unit<std::string> with non empty input") {
        const auto result = unit(std::string("hello"))("world"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::string("hello"));
        CHECK(result.value().second == "world"sv);
    }
}

TEST_CASE("first_char_match", "[parsers]") {
    const auto is_a = [](char x) { return x == 'a'; };

    SECTION("empty input, no match") {
        const auto result = first_char_match(is_a)(""sv);
        REQUIRE(!result.has_value());
    }

    SECTION("non empty input, no match") {
        const auto result = first_char_match(is_a)("hello"sv);
        REQUIRE(!result.has_value());
    }

    SECTION("non empty input, match first character") {
        const auto result = first_char_match(is_a)("ahello"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "hello"sv);
    }

    SECTION("non empty input, match middle character") {
        const auto result = first_char_match(is_a)("helloaworld"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "world"sv);
    }

    SECTION("non empty input, match last character") {
        const auto result = first_char_match(is_a)("helloa"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second.empty());
    }

    SECTION("non empty input, match middle character with multiple contiguous matches") {
        const auto result = first_char_match(is_a)("helloaaworld"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "aworld"sv);
    }

    SECTION("non empty input, match middle character with multiple non contiguous matches") {
        const auto result = first_char_match(is_a)("helloaworldaparser"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "worldaparser"sv);
    }
}

TEST_CASE("last_char_match", "[parsers]") {
    const auto is_a = [](char x) { return x == 'a'; };

    SECTION("empty input, no match") {
        const auto result = last_char_match(is_a)(""sv);
        REQUIRE(!result.has_value());
    }

    SECTION("non empty input, no match") {
        const auto result = last_char_match(is_a)("hello"sv);
        REQUIRE(!result.has_value());
    }

    SECTION("non empty input, match first character") {
        const auto result = last_char_match(is_a)("ahello"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second.empty());
    }

    SECTION("non empty input, match middle character") {
        const auto result = last_char_match(is_a)("helloaworld"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "hello"sv);
    }

    SECTION("non empty input, match last character") {
        const auto result = last_char_match(is_a)("helloa"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "hello"sv);
    }

    SECTION("non empty input, match middle character with multiple contiguous matches") {
        const auto result = last_char_match(is_a)("helloaaworld"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "helloa"sv);
    }

    SECTION("non empty input, match middle character with multiple non contiguous matches") {
        const auto result = last_char_match(is_a)("helloaworldaparser"sv);
        REQUIRE(result.has_value());
        CHECK(result.value().first == 'a');
        CHECK(result.value().second == "helloaworldap"sv);
    }
}
