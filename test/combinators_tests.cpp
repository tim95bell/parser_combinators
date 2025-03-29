
#include <pc/pc.hpp>
#include <pc/parsers.hpp>
#include <pc/combinators.hpp>
#include <catch2/catch_test_macros.hpp>
#include <string_view>

namespace pc {
    using namespace combinators;
    using namespace parsers;
}
using namespace std::literals::string_view_literals;

TEST_CASE("manyn", "[combinators]") {
    SECTION("manyn<0> with empty input, no match") {
        const auto result = pc::manyn<0>(pc::tag("hello"))("");
        REQUIRE(result.has_value());
        CHECK(result.value().first.empty());
        CHECK(result.value().second.empty());
    }

    SECTION("manyn<0> with empty input, match") {
        const auto result = pc::manyn<0>(pc::tag(""))("");
        REQUIRE(result.has_value());
        CHECK(result.value().first.empty());
        CHECK(result.value().second.empty());
    }

    SECTION("manyn<0> with non empty input, no match") {
        const auto result = pc::manyn<0>(pc::tag("hello"))("worldworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first.empty());
        CHECK(result.value().second == "worldworld"sv);
    }

    SECTION("manyn<0> with non empty input, match") {
        const auto result = pc::manyn<0>(pc::tag("hello"))("hellohello");
        REQUIRE(result.has_value());
        CHECK(result.value().first.empty());
        CHECK(result.value().second == "hellohello"sv);
    }

    SECTION("manyn<1> with empty input, no match") {
        const auto result = pc::manyn<1>(pc::tag("hello"))("");
        REQUIRE(!result.has_value());
    }

    SECTION("manyn<1> with empty input, match") {
        const auto result = pc::manyn<1>(pc::tag(""))("");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::array<std::string, 1>{""});
        CHECK(result.value().second.empty());
    }

    SECTION("manyn<1> with non empty input, no match") {
        const auto result = pc::manyn<1>(pc::tag("hello"))("worldworld");
        REQUIRE(!result.has_value());
    }

    SECTION("manyn<1> with non empty input, match") {
        const auto result = pc::manyn<1>(pc::tag("hello"))("hellohello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::array<std::string, 1>{"hello"});
        CHECK(result.value().second == "hello"sv);
    }

    SECTION("manyn<3> with empty input, no match") {
        const auto result = pc::manyn<3>(pc::tag("hello"))("");
        REQUIRE(!result.has_value());
    }

    SECTION("manyn<3> with empty input, match") {
        const auto result = pc::manyn<3>(pc::tag(""))("");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::array<std::string, 3>{"", "", ""});
        CHECK(result.value().second.empty());
    }

    SECTION("manyn<3> with empty input, match by unit") {
        const auto result = pc::manyn<3>(pc::unit<int>(20))("");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::array<int, 3>{20, 20, 20});
        CHECK(result.value().second.empty());
    }

    SECTION("manyn<3> with non empty input, no match") {
        const auto result = pc::manyn<3>(pc::tag("hello"))("worldworld");
        REQUIRE(!result.has_value());
    }

    SECTION("manyn<3> with non empty input, partial match") {
        const auto result = pc::manyn<3>(pc::tag("hello"))("hellohelloworld");
        REQUIRE(!result.has_value());
    }

    SECTION("manyn<3> with non empty input, match") {
        const auto result = pc::manyn<3>(pc::tag("hello"))("hellohellohellohello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::array<std::string, 3>{"hello", "hello", "hello"});
        CHECK(result.value().second == "hello"sv);
    }
}

TEST_CASE("trim", "[combinators]") {
    SECTION("trim with input that does NOT need trimming, match") {
        const auto result = pc::trim(pc::tag("hello"))("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second.empty());
    }

    SECTION("trim with input that does NOT need trimming, no match") {
        const auto result = pc::trim(pc::tag("hello"))("world");
        REQUIRE(!result.has_value());
    }

    SECTION("trim with input that needs trimming, match") {
        const auto result = pc::trim(pc::tag("hello"))(" \n \t  hello \n\t ");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second.empty());
    }

    SECTION("trim with input that needs trimming, no match") {
        const auto result = pc::trim(pc::tag("hello"))("   \n\t world\n\t \t ");
        REQUIRE(!result.has_value());
    }

    SECTION("trim with input that needs NOT trimming, would be match") {
        const auto result = pc::trim(pc::tag(" \n \t  "))(" \n \t  hello \n\t ");
        REQUIRE(!result.has_value());
    }

    SECTION("NO trim with input that needs NOT trimming, match") {
        const auto result = pc::tag(" \n \t  ")(" \n \t  hello \n\t ");
        REQUIRE(result.has_value());
        CHECK(result.value().first == " \n \t  "sv);
        CHECK(result.value().second == "hello \n\t "sv);
    }
}

TEST_CASE("choice", "[combinators]") {
    const auto hello = pc::tag("hello");
    const auto world = pc::tag("world");
    const auto hel = pc::tag("hel");

    SECTION("choice with no match") {
        const auto result = pc::choice(hello, world)("parser combinators");
        REQUIRE(!result.has_value());
    }

    SECTION("choice with first match") {
        const auto result = pc::choice(hello, world)("helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second == "world"sv);
    }

    SECTION("choice with second match") {
        const auto result = pc::choice(hello, world)("worldhello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "world"sv);
        CHECK(result.value().second == "hello"sv);
    }

    SECTION("choice with both match, should be first") {
        const auto result = pc::choice(hello, hel)("helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hello"sv);
        CHECK(result.value().second == "world"sv);
    }

    SECTION("choice with both match (other way around), should be first") {
        const auto result = pc::choice(hel, hello)("helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == "hel"sv);
        CHECK(result.value().second == "loworld"sv);
    }
}

TEST_CASE("many0_to_many1", "[combinators]") {
    const auto empty = pc::unit(std::vector<int>{});
    const auto one = pc::unit(std::vector<int>{1});
    const auto many = pc::unit(std::vector<int>{1, 2, 3});

    SECTION("empty output fails") {
        const auto result = pc::many0_to_many1(empty)("hello world");
        REQUIRE(!result.has_value());
    }

    SECTION("failure fails") {
        const auto result = pc::many0_to_many1(pc::many0(pc::tag("world")))("hello world");
        // TODO(TB): why does this work without pc namespace? but the next line does not
        // const auto result = many0_to_many1(pc::many0(pc::tag("world")))("hello world");
        // const auto result2 = many0_to_many1(pc::tag("world"))("hello world");
        REQUIRE(!result.has_value());
    }

    SECTION("size one output succeeds") {
        const auto result = pc::many0_to_many1(one)("hello world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<int>{1});
        CHECK(result.value().second == "hello world"sv);
    }

    SECTION("size many output succeeds") {
        const auto result = pc::many0_to_many1(many)("hello world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<int>{1, 2, 3});
        CHECK(result.value().second == "hello world"sv);
    }
}
