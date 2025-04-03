
#include <pc/pc.hpp>
#include <pc/parsers.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string_view>

namespace pc {
    using namespace parsers;
}
using namespace std::literals::string_view_literals;

TEST_CASE("character", "[parsers]") {
    SECTION("empty input") {
        const auto result = pc::character(""sv);
        REQUIRE(!result);
    }

    SECTION("one character input") {
        const auto result = pc::character("a"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == ""sv);
    }

    SECTION("two character input") {
        const auto result = pc::character("ab"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "b"sv);
    }

    SECTION("many character input") {
        const auto result = pc::character("hello"sv);
        REQUIRE(result);
        CHECK(result->first == 'h');
        CHECK(result->second == "ello"sv);
    }
}

TEST_CASE("newline", "[parsers]") {
    SECTION("empty input") {
        const auto result = pc::newline(""sv);
        REQUIRE(!result);
    }

    SECTION("one character input, no match") {
        const auto result = pc::newline("a"sv);
        REQUIRE(!result);
    }

    SECTION("one character input, match") {
        const auto result = pc::newline("\n"sv);
        REQUIRE(result);
        CHECK(result->first == '\n');
        CHECK(result->second == ""sv);
    }

    SECTION("two character input, no match") {
        const auto result = pc::newline("a\n"sv);
        REQUIRE(!result);
    }

    SECTION("two character input, match") {
        const auto result = pc::newline("\na"sv);
        REQUIRE(result);
        CHECK(result->first == '\n');
        CHECK(result->second == "a"sv);
    }

    SECTION("two character input, both newline, match") {
        const auto result = pc::newline("\n\n"sv);
        REQUIRE(result);
        CHECK(result->first == '\n');
        CHECK(result->second == "\n"sv);
    }

    SECTION("many character input, no match") {
        const auto result = pc::newline("hel\nlo\n"sv);
        REQUIRE(!result);
    }

    SECTION("many character input, match") {
        const auto result = pc::newline("\nhel\nlo\n"sv);
        REQUIRE(result);
        CHECK(result->first == '\n');
        CHECK(result->second == "hel\nlo\n"sv);
    }
}

TEST_CASE("line", "[parsers]") {
    SECTION("empty input") {
        const auto result = pc::line(""sv);
        REQUIRE(!result);
    }

    SECTION("one character input") {
        const auto result = pc::line("a"sv);
        REQUIRE(result);
        CHECK(result->first == "a"sv);
        CHECK(result->second == ""sv);
    }

    SECTION("one character input, newline character") {
        const auto result = pc::line("\n"sv);
        REQUIRE(result);
        CHECK(result->first == ""sv);
        CHECK(result->second == ""sv);
    }

    SECTION("many character input") {
        const auto result = pc::line("hello"sv);
        REQUIRE(result);
        CHECK(result->first == "hello"sv);
        CHECK(result->second == ""sv);
    }

    SECTION("many character input, ending in newline") {
        const auto result = pc::line("hello\n"sv);
        REQUIRE(result);
        CHECK(result->first == "hello"sv);
        CHECK(result->second == ""sv);
    }

    SECTION("many character input, newline in middle") {
        const auto result = pc::line("hello\nworld"sv);
        REQUIRE(result);
        CHECK(result->first == "hello"sv);
        CHECK(result->second == "world"sv);
    }

    SECTION("many character input, starting with newline") {
        const auto result = pc::line("\nhello"sv);
        REQUIRE(result);
        CHECK(result->first == ""sv);
        CHECK(result->second == "hello"sv);
    }
}

TEST_CASE("tag", "[parsers]") {
    SECTION("empty input") {
        const auto result = pc::tag("hello")("");
        REQUIRE(!result);
    }

    SECTION("empty input, empty tag") {
        const auto result = pc::tag("")("");
        REQUIRE(result);
        CHECK(result->first == ""sv);
        CHECK(result->second == ""sv);
    }

    SECTION("non matching input") {
        const auto result = pc::tag("hello")("helo");
        REQUIRE(!result);
    }

    SECTION("non matching input, matching in middle") {
        const auto result = pc::tag("hello")("helohellohelo");
        REQUIRE(!result);
    }

    SECTION("non matching input, matching at end") {
        const auto result = pc::tag("hello")("helohello");
        REQUIRE(!result);
    }

    SECTION("non empty input, empty tag") {
        const auto result = pc::tag("")("hello");
        REQUIRE(result);
        CHECK(result->first == ""sv);
        CHECK(result->second == "hello"sv);
    }

    SECTION("exactly matching input") {
        const auto result = pc::tag("hello")("hello");
        REQUIRE(result);
        CHECK(result->first == "hello"sv);
        CHECK(result->second == ""sv);
    }

    SECTION("matching input, extra after") {
        const auto result = pc::tag("hello")("hello123");
        REQUIRE(result);
        CHECK(result->first == "hello"sv);
        CHECK(result->second == "123"sv);
    }

    SECTION("matching twice input") {
        const auto result = pc::tag("hello")("hellohello");
        REQUIRE(result);
        CHECK(result->first == "hello"sv);
        CHECK(result->second == "hello"sv);
    }
}

TEST_CASE("tag<char>", "[parsers]") {
    SECTION("empty input") {
        const auto result = pc::tag('h')("");
        REQUIRE(!result);
    }

    SECTION("non matching input") {
        const auto result = pc::tag('w')("hello");
        REQUIRE(!result);
    }

    SECTION("non matching input, matching in middle") {
        const auto result = pc::tag('e')("hello");
        REQUIRE(!result);
    }

    SECTION("non matching input, matching at end") {
        const auto result = pc::tag('o')("helohello");
        REQUIRE(!result);
    }

    SECTION("exactly matching input") {
        const auto result = pc::tag('h')("h");
        REQUIRE(result);
        CHECK(result->first == 'h');
        CHECK(result->second == ""sv);
    }

    SECTION("matching input, extra after") {
        const auto result = pc::tag('h')("hello");
        REQUIRE(result);
        CHECK(result->first == 'h');
        CHECK(result->second == "ello"sv);
    }

    SECTION("matching twice input") {
        const auto result = pc::tag('h')("hh");
        REQUIRE(result);
        CHECK(result->first == 'h');
        CHECK(result->second == "h"sv);
    }
}

TEST_CASE("unit", "[parsers]") {
    SECTION("unit<int> with empty input") {
        const auto result = pc::unit(20)(""sv);
        REQUIRE(result);
        CHECK(result->first == 20);
        CHECK(result->second == ""sv);
    }

    SECTION("unit<std::string> with empty input") {
        const auto result = pc::unit(std::string("hello"))(""sv);
        REQUIRE(result);
        CHECK(result->first == std::string("hello"));
        CHECK(result->second == ""sv);
    }

    SECTION("unit<int> with non empty input") {
        const auto result = pc::unit(20)("hello"sv);
        REQUIRE(result);
        CHECK(result->first == 20);
        CHECK(result->second == "hello"sv);
    }

    SECTION("unit<std::string> with non empty input") {
        const auto result = pc::unit(std::string("hello"))("world"sv);
        REQUIRE(result);
        CHECK(result->first == std::string("hello"));
        CHECK(result->second == "world"sv);
    }
}

TEST_CASE("first_char_match", "[parsers]") {
    const auto is_a = [](char x) { return x == 'a'; };

    SECTION("empty input, no match") {
        const auto result = pc::first_char_match(is_a)(""sv);
        REQUIRE(!result);
    }

    SECTION("non empty input, no match") {
        const auto result = pc::first_char_match(is_a)("hello"sv);
        REQUIRE(!result);
    }

    SECTION("non empty input, match first character") {
        const auto result = pc::first_char_match(is_a)("ahello"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "hello"sv);
    }

    SECTION("non empty input, match middle character") {
        const auto result = pc::first_char_match(is_a)("helloaworld"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "world"sv);
    }

    SECTION("non empty input, match last character") {
        const auto result = pc::first_char_match(is_a)("helloa"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == ""sv);
    }

    SECTION("non empty input, match middle character with multiple contiguous matches") {
        const auto result = pc::first_char_match(is_a)("helloaaworld"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "aworld"sv);
    }

    SECTION("non empty input, match middle character with multiple non contiguous matches") {
        const auto result = pc::first_char_match(is_a)("helloaworldaparser"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "worldaparser"sv);
    }
}

TEST_CASE("last_char_match", "[parsers]") {
    const auto is_a = [](char x) { return x == 'a'; };

    SECTION("empty input, no match") {
        const auto result = pc::last_char_match(is_a)(""sv);
        REQUIRE(!result);
    }

    SECTION("non empty input, no match") {
        const auto result = pc::last_char_match(is_a)("hello"sv);
        REQUIRE(!result);
    }

    SECTION("non empty input, match first character") {
        const auto result = pc::last_char_match(is_a)("ahello"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == ""sv);
    }

    SECTION("non empty input, match middle character") {
        const auto result = pc::last_char_match(is_a)("helloaworld"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "hello"sv);
    }

    SECTION("non empty input, match last character") {
        const auto result = pc::last_char_match(is_a)("helloa"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "hello"sv);
    }

    SECTION("non empty input, match middle character with multiple contiguous matches") {
        const auto result = pc::last_char_match(is_a)("helloaaworld"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "helloa"sv);
    }

    SECTION("non empty input, match middle character with multiple non contiguous matches") {
        const auto result = pc::last_char_match(is_a)("helloaworldaparser"sv);
        REQUIRE(result);
        CHECK(result->first == 'a');
        CHECK(result->second == "helloaworldap"sv);
    }
}

TEST_CASE("fail", "[parsers]") {
    SECTION("empty input") {
        const auto result = pc::fail<int>("");
        REQUIRE(!result);
    }

    SECTION("non empty input") {
        const auto result = pc::fail<int>("hello world");
        REQUIRE(!result);
    }
}
