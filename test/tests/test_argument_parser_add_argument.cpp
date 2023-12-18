#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <argument_parser_test_fixture.hpp>

#include <iostream>

using namespace ap::argument;


namespace {

constexpr std::string_view name = "test";
constexpr std::string_view short_name = "t";

constexpr std::string_view other_name = "other";
constexpr std::string_view other_short_name = "o";

} // namespace


namespace ap_testing {

TEST_SUITE_BEGIN("test_argument_parser_add_argument");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_positional_argument should return a positional argument reference"
) {
    SUBCASE("with just the long name") {
        const auto& argument = sut.add_positional_argument(name);
        REQUIRE_FALSE(argument.is_optional());
    }
    SUBCASE("with both names") {
        const auto& argument = sut.add_positional_argument(name, short_name);
        REQUIRE_FALSE(argument.is_optional());
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_optional_argument should return a positional argument reference"
) {
    SUBCASE("with just the long name") {
        const auto& argument = sut.add_optional_argument(name);
        REQUIRE(argument.is_optional());
    }
    SUBCASE("with both names") {
        const auto& argument = sut.add_optional_argument(name, short_name);
        REQUIRE(argument.is_optional());
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_positional_argument should throw only when adding an"
    "argument with previously used name"
) {
    sut.add_positional_argument(name, short_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_positional_argument(other_name, other_short_name));
    }

    SUBCASE("adding argument with a previously used long name") {
        REQUIRE_THROWS_AS(sut.add_positional_argument(name), std::invalid_argument);
    }

    SUBCASE("adding argument with a previously used short name") {
        REQUIRE_THROWS_AS(
            sut.add_positional_argument(other_name, short_name), std::invalid_argument
        );
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_optional_argument should throw only when adding an"
    "argument with previously used name"
) {
    sut.add_optional_argument(name, short_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_optional_argument(other_name, other_short_name));
    }

    SUBCASE("adding argument with a previously used long name") {
        REQUIRE_THROWS_AS(sut.add_optional_argument(name), std::invalid_argument);
    }

    SUBCASE("adding argument with a previously used short name") {
        REQUIRE_THROWS_AS(sut.add_optional_argument(other_name, short_name), std::invalid_argument);
    }
}

TEST_SUITE_END();

} // namespace ap_testing
