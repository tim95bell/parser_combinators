
#include <pc/pc.hpp>
#include <pc/parsers.hpp>
#include <pc/combinators.hpp>
#include <catch2/catch_test_macros.hpp>
#include <ranges>
#include <string>
#include <string_view>
#include <utility>

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

TEST_CASE("many0", "[combinators]") {
    SECTION("many0 matching 0 times, empty input") {
        const auto result = pc::many0(pc::tag("hello"))("");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many0 matching 0 times, non empty input") {
        const auto result = pc::many0(pc::tag("hello"))("world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many0 matching exactly 1 times, non empty input") {
        const auto result = pc::many0(pc::tag("hello"))("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many0 matching 1 times, non empty input") {
        const auto result = pc::many0(pc::tag("hello"))("helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many0 matching exactly 3 times, non empty input") {
        const auto result = pc::many0(pc::tag("hello"))("hellohellohello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many0 matching 3 times, non empty input") {
        const auto result = pc::many0(pc::tag("hello"))("hellohellohelloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == "world"sv);
    }
}

TEST_CASE("many1", "[combinators]") {
    SECTION("many1 matching 0 times, empty input") {
        const auto result = pc::many1(pc::tag("hello"))("");
        REQUIRE(!result.has_value());
    }

    SECTION("many1 matching 0 times, non empty input") {
        const auto result = pc::many1(pc::tag("hello"))("world");
        REQUIRE(!result.has_value());
    }

    SECTION("many1 matching exactly 1 times, non empty input") {
        const auto result = pc::many1(pc::tag("hello"))("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many1 matching 1 times, non empty input") {
        const auto result = pc::many1(pc::tag("hello"))("helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many1 matching exactly 3 times, non empty input") {
        const auto result = pc::many1(pc::tag("hello"))("hellohellohello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many1 matching 3 times, non empty input") {
        const auto result = pc::many1(pc::tag("hello"))("hellohellohelloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == "world"sv);
    }
}

TEST_CASE("many_seperated_by0", "[combinators]") {
    SECTION("many_seperated_by0 matching 0 times, empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many_seperated_by0 matching 0 times, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many_seperated_by0 matching exactly 1 times, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many_seperated_by0 matching 1 times, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many_seperated_by0 matching 1 times then matching seperator, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("hello,world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ",world"sv);
    }

    SECTION("many_seperated_by0 matching 3 times exactly, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("hello,hello,hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many_seperated_by0 matching 3 times, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("hello,hello,helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many_seperated_by0 matching 1 times then matching seperator, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("hello,hello,hello,world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == ",world"sv);
    }

    SECTION("many_seperated_by0 would match 3 times but no seperator, non empty input") {
        const auto result = pc::many_seperated_by0(pc::tag("hello"), pc::tag(","))("hellohellohelloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == "hellohelloworld"sv);
    }
}

TEST_CASE("many_seperated_by1", "[combinators]") {
    SECTION("many_seperated_by1 matching 0 times, empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("");
        REQUIRE(!result.has_value());
    }

    SECTION("many_seperated_by1 matching 0 times, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("world");
        REQUIRE(!result.has_value());
    }

    SECTION("many_seperated_by1 matching exactly 1 times, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many_seperated_by1 matching 1 times, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many_seperated_by1 matching 1 times then matching seperator, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("hello,world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ",world"sv);
    }

    SECTION("many_seperated_by1 matching 3 times exactly, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("hello,hello,hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("many_seperated_by1 matching 3 times, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("hello,hello,helloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == "world"sv);
    }

    SECTION("many_seperated_by1 matching 1 times then matching seperator, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("hello,hello,hello,world");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result.value().second == ",world"sv);
    }

    SECTION("many_seperated_by1 would match 3 times but no seperator, non empty input") {
        const auto result = pc::many_seperated_by1(pc::tag("hello"), pc::tag(","))("hellohellohelloworld");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == "hellohelloworld"sv);
    }
}

TEST_CASE("many_split_by0", "[combinators]") {
    SECTION("no match, empty input") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("");
        REQUIRE(!result.has_value());
    }

    SECTION("1 match, empty input") {
        const auto result = pc::many_split_by0(pc::tag(""), "\n")("");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{""});
        CHECK(result.value().second == ""sv);
    }

    SECTION("no match, non empty input") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("world");
        REQUIRE(!result.has_value());
    }

    SECTION("1 exact match, non empty input") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("1 match, split to one (no delimiter), non empty input - should fail if not all input in any split is consumed") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("helloworld");
        REQUIRE(!result.has_value());
    }

    SECTION("1 match, split to one (no delimiter), non empty input") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("hello");
        REQUIRE(result.has_value());
        CHECK(result->first == std::vector<std::string>{"hello"});
        CHECK(result->second == ""sv);
    }

    SECTION("1 match, split to 2, non empty input - should fail as 2nd split will not match") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("hello\nworld");
        REQUIRE(!result.has_value());
    }

    SECTION("1 match, split to 2 with second split empty, non empty input - should fail as 2nd split will not match") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("hello\n");
        REQUIRE(!result.has_value());
    }

    SECTION("3 matches, split to 3, non empty input") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("hello\nhello\nhello");
        REQUIRE(result.has_value());
        CHECK(result->first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result->second == ""sv);
    }

    SECTION("2 matches, split to 3, non empty input - should fail because third split does not match") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("hello\nhello\nworld");
        REQUIRE(!result.has_value());
    }

    SECTION("3 matches, split to 3, all empty splits, non empty input") {
        const auto result = pc::many_split_by0(pc::tag(""), "\n")("\n\n");
        REQUIRE(result.has_value());
        CHECK(result->first == std::vector<std::string>{"", "", ""});
        CHECK(result->second == ""sv);
    }

    SECTION("0 matches, split to 3, all empty splits, non empty input") {
        const auto result = pc::many_split_by0(pc::tag("hello"), "\n")("\n\n");
        REQUIRE(!result.has_value());
    }
}

TEST_CASE("many_split_by1", "[combinators]") {
    SECTION("no match, empty input") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("");
        REQUIRE(!result.has_value());
    }

    SECTION("1 match, empty input") {
        const auto result = pc::many_split_by1(pc::tag(""), "\n")("");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{""});
        CHECK(result.value().second == ""sv);
    }

    SECTION("no match, non empty input") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("world");
        REQUIRE(!result.has_value());
    }

    SECTION("1 exact match, non empty input") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("hello");
        REQUIRE(result.has_value());
        CHECK(result.value().first == std::vector<std::string>{"hello"});
        CHECK(result.value().second == ""sv);
    }

    SECTION("1 match, split to one (no delimiter), non empty input - should fail if not all input in any split is consumed") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("helloworld");
        REQUIRE(!result.has_value());
    }

    SECTION("1 match, split to one (no delimiter), non empty input") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("hello");
        REQUIRE(result.has_value());
        CHECK(result->first == std::vector<std::string>{"hello"});
        CHECK(result->second == ""sv);
    }

    SECTION("1 match, split to 2, non empty input - should fail as 2nd split will not match") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("hello\nworld");
        REQUIRE(!result.has_value());
    }

    SECTION("1 match, split to 2 with second split empty, non empty input - should fail as 2nd split will not match") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("hello\n");
        REQUIRE(!result.has_value());
    }

    SECTION("3 matches, split to 3, non empty input") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("hello\nhello\nhello");
        REQUIRE(result.has_value());
        CHECK(result->first == std::vector<std::string>{"hello", "hello", "hello"});
        CHECK(result->second == ""sv);
    }

    SECTION("2 matches, split to 3, non empty input - should fail because third split does not match") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("hello\nhello\nworld");
        REQUIRE(!result.has_value());
    }

    SECTION("3 matches, split to 3, all empty splits, non empty input") {
        const auto result = pc::many_split_by1(pc::tag(""), "\n")("\n\n");
        REQUIRE(result.has_value());
        CHECK(result->first == std::vector<std::string>{"", "", ""});
        CHECK(result->second == ""sv);
    }

    SECTION("0 matches, split to 3, all empty splits, non empty input") {
        const auto result = pc::many_split_by1(pc::tag("hello"), "\n")("\n\n");
        REQUIRE(!result.has_value());
    }
}

// pair works with different types
static_assert(requires {
    pc::pair(pc::tag("hello"), pc::tag("world"));
    pc::pair(pc::tag('h'), pc::tag("world"));
    pc::pair(pc::tag("hello"), pc::tag('w'));
    pc::pair(pc::tag('h'), pc::tag('w'));
    pc::pair(pc::unit<int>(1), pc::tag('w'));
    pc::pair(pc::unit<int>(1), pc::unit<double>(1.0));
});
TEST_CASE("pair", "[combinators]") {
    const auto hello = pc::tag("hello");
    const auto world = pc::tag("world");
    const auto hello_world = pc::pair(hello, world);

    SECTION("pair<std::string, std::string>(p1, p2) fails when p or r fails") {
        {
            const auto result = hello_world("");
            REQUIRE(!result);
        }
        {
            const auto result = hello_world("hello");
            REQUIRE(!result);
        }
        {
            const auto result = hello_world("world");
            REQUIRE(!result);
        }
        {
            const auto result = hello_world("worldhello");
            REQUIRE(!result);
        }
    }

    SECTION("pair<std::string>(p1, p2) on empty input succeeds as p1 and p2 succeed") {
        const auto result = pc::pair(pc::tag(""), pc::tag(""))("");
        REQUIRE(result);
        CHECK(result->first == std::make_pair(std::string(""), std::string("")));
        CHECK(result->second == ""sv);
    }

    SECTION("pair<std::string>(p1, p2) on non empty input succeeds as p1 and p2 succeed") {
        {
            const auto result = hello_world("helloworld");
            REQUIRE(result);
            CHECK(result->first == std::make_pair(std::string("hello"), std::string("world")));
            CHECK(result->second == ""sv);
        }
        {
            const auto result = hello_world("helloworld123");
            REQUIRE(result);
            CHECK(result->first == std::make_pair(std::string("hello"), std::string("world")));
            CHECK(result->second == "123"sv);
        }
    }
}

// seperated_pair works with different types
static_assert(requires {
    pc::seperated_pair(pc::tag("hello"), pc::tag(","), pc::tag("world"));
    pc::seperated_pair(pc::tag('h'), pc::unit<unsigned long long>(3), pc::tag("world"));
    pc::seperated_pair(pc::tag("hello"), pc::unit('a'), pc::tag('w'));
    pc::seperated_pair(pc::tag('h'), pc::unit(4.0f), pc::tag('w'));
    pc::seperated_pair(pc::unit<int>(1), pc::unit(""sv), pc::tag('w'));
    pc::seperated_pair(pc::unit<int>(1), pc::unit(true), pc::unit<double>(1.0));
});
TEST_CASE("seperated_pair", "[combinators]") {
    const auto hello = pc::tag("hello");
    const auto world = pc::tag("world");
    const auto comma = pc::tag(',');
    const auto hello_world = pc::seperated_pair(hello, comma, world);

    SECTION("seperated_pair<std::string, std::string>(lhs, sep, rhs) fails when lhs or sep or rhs fails") {
        {
            const auto result = hello_world("");
            REQUIRE(!result);
        }
        {
            const auto result = hello_world("hello");
            REQUIRE(!result);
        }
        {
            const auto result = hello_world("world");
            REQUIRE(!result);
        }
        {
            const auto result = hello_world("world,hello");
            REQUIRE(!result);
        }
        {
            const auto result = hello_world("hello,");
            REQUIRE(!result);
        }
    }

    SECTION("seperated_pair(lhs, sep, rhs) on empty input succeeds as lhs and sep and rhs succeed") {
        const auto result = pc::seperated_pair(pc::tag(""), pc::tag(""), pc::tag(""))("");
        REQUIRE(result);
        CHECK(result->first == std::make_pair(std::string(""), std::string("")));
        CHECK(result->second == ""sv);
    }

    SECTION("seperated_pair(lhs, sep, rhs) on non empty input succeeds as lhs and sep and rhs succeed") {
        {
            const auto result = hello_world("hello,world");
            REQUIRE(result);
            CHECK(result->first == std::make_pair(std::string("hello"), std::string("world")));
            CHECK(result->second == ""sv);
        }
        {
            const auto result = hello_world("hello,world123");
            REQUIRE(result);
            CHECK(result->first == std::make_pair(std::string("hello"), std::string("world")));
            CHECK(result->second == "123"sv);
        }
    }
}

static_assert(requires {
    pc::tuple(pc::unit<std::string>(""));
    pc::tuple(pc::unit<int>(1));
    pc::tuple(pc::unit<long long>(1));
    pc::tuple(pc::unit<unsigned int>(1));
    pc::tuple(pc::unit<unsigned long long>(1));
    pc::tuple(pc::unit<bool>(true));
    pc::tuple(pc::unit<float>(1.0f));
    pc::tuple(pc::unit<double>(1.0));

    pc::tuple(pc::unit<std::string>(""), pc::unit<int>(1));
    pc::tuple(pc::unit<int>(1), pc::unit<std::string>(""));

    pc::tuple(pc::unit<long>(1), pc::unit<char>(' '), pc::unit<bool>(true));

    pc::tuple(pc::unit<std::string>(""), pc::unit<unsigned long long>(4), pc::unit<char>('a'), pc::unit<bool>(false));
});
TEST_CASE("tuple", "[combinators]") {
    SECTION("tuple(p1) succeeds on empty input as p1 succeeds") {
        const auto result = pc::tuple(pc::tag(""))("");
        REQUIRE(result);
        CHECK(result->first == std::tuple<std::string>{""});
        CHECK(result->second == ""sv);
    }

    SECTION("tuple(p1) fails on empty input as p1 fails") {
        const auto result = pc::tuple(pc::tag("hello"))("");
        REQUIRE(!result);
    }

    SECTION("tuple(p1, p2) succeeds on empty input as p1 and p2 succeed") {
        const auto result = pc::tuple(pc::tag(""), pc::unit<int>(1))("");
        REQUIRE(result);
        CHECK(result->first == std::tuple<std::string, int>{"", 1});
        CHECK(result->second == ""sv);
    }

    SECTION("tuple(p1, p2) fails on empty input as p1 or p2 fails") {
        {
            const auto result = pc::tuple(pc::tag("hello"), pc::unit<int>(1))("");
            REQUIRE(!result);
        }
        {
            const auto result = pc::tuple(pc::tag(""), pc::tag('h'))("");
            REQUIRE(!result);
        }
        {
            const auto result = pc::tuple(pc::tag("hello"), pc::tag('h'))("");
            REQUIRE(!result);
        }
    }

    SECTION("tuple(p1, p2, p3) succeeds on empty input as p1 and p2 and p3 succeed") {
        const auto result = pc::tuple(pc::tag(""), pc::unit<int>(1), pc::unit('a'))("");
        REQUIRE(result);
        CHECK(result->first == std::tuple<std::string, int, char>{"", 1, 'a'});
        CHECK(result->second == ""sv);
    }

    SECTION("tuple(p1, p2, p3) fails on empty input as p1 or p2 or p3 fails") {
        {
            const auto result = pc::tuple(pc::tag("hello"), pc::unit<int>(1), pc::unit('a'))("");
            REQUIRE(!result);
        }
        {
            const auto result = pc::tuple(pc::tag(""), pc::tag('h'), pc::unit('a'))("");
            REQUIRE(!result);
        }
        {
            const auto result = pc::tuple(pc::tag("hello"), pc::tag('h'), pc::unit('a'))("");
            REQUIRE(!result);
        }
        {
            const auto result = pc::tuple(pc::tag("hello"), pc::tag('h'), pc::tag('a'))("");
            REQUIRE(!result);
        }
    }
}

TEST_CASE("map", "[combinators]") {
    const auto inc = [](int x) { return x + 1; };
    const auto one = pc::unit<int>(1);
    const auto capitalize = [](std::string_view str) {
        auto capitalized_view = str | std::views::transform([](char c) { return std::toupper(c); });
        return std::string(capitalized_view.begin(), capitalized_view.end());
    };

    SECTION("map failure gives failure, with empty input") {
        const auto result = pc::map(pc::fail<int>, inc)("");
        REQUIRE(!result);
    }

    SECTION("map failure gives failure, with non empty input") {
        const auto result = pc::map(pc::fail<int>, inc)("hello");
        REQUIRE(!result);
    }

    SECTION("map success updates value by running mapping function on it, input remains unchanged, with empty input") {
        const auto result = pc::map(one, inc)("");
        REQUIRE(result);
        CHECK(result->first == 2);
        CHECK(result->second == ""sv);
    }

    SECTION("map success updates value by running mapping function on it, input remains unchanged, with non empty input") {
        const auto result = pc::map(one, inc)("hello");
        REQUIRE(result);
        CHECK(result->first == 2);
        CHECK(result->second == "hello"sv);
    }

    SECTION("the parser being mapped gets passed the correct input") {
        const auto result = pc::map(pc::tag("hello"), capitalize)("hello world");
        REQUIRE(result);
        CHECK(result->first == std::string("HELLO"));
        CHECK(result->second == " world"sv);
    }
}

TEST_CASE("filter", "[combinators]") {
    const auto positive = [](int x) { return x > 0; };
    const auto not_empty = [](std::string_view s) { return !s.empty(); };

    SECTION("filter failure gives failure, with empty input") {
        const auto result = pc::filter(pc::fail<int>, positive)("");
        REQUIRE(!result);
    }

    SECTION("filter failure gives failure, with non empty input") {
        const auto result = pc::filter(pc::fail<int>, positive)("hello");
        REQUIRE(!result);
    }

    SECTION("filter success does nothing if success value passes the predicate, with empty input") {
        const auto result = pc::filter(pc::unit<int>(1), positive)("");
        REQUIRE(result);
        CHECK(result->first == 1);
        CHECK(result->second == ""sv);
    }

    SECTION("filter success does nothing if success value passes the predicate, with non empty input") {
        const auto result = pc::filter(pc::unit<int>(1), positive)("hello");
        REQUIRE(result);
        CHECK(result->first == 1);
        CHECK(result->second == "hello"sv);
    }

    SECTION("filter success produces failure if success value passes the predicate, with empty input") {
        const auto result = pc::filter(pc::unit<int>(-1), positive)("");
        REQUIRE(!result);
    }

    SECTION("filter success produces failure if success value passes the predicate, with non empty input") {
        const auto result = pc::filter(pc::unit<int>(-1), positive)("hello");
        REQUIRE(!result);
    }
}
