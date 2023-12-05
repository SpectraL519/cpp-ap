#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>

#include <iostream>

using namespace ap::detail;


namespace {

constexpr std::string_view name = "test";
constexpr std::string_view short_name = "t";

constexpr std::string_view other_name = "other";
constexpr std::string_view other_short_name = "o";

} // namespace


TEST_SUITE_BEGIN("test_argument_parser_add_argument");

TEST_CASE("add_positional_argument should return a positional argument "
          "reference") {
    ap::argument_parser ap;

    SUBCASE("with just the long name") {
        const auto& argument = ap.add_positional_argument(name);
        // REQUIRE_FALSE(testing_argument_is_optional(argument));
    }
    SUBCASE("with both names") {
        const auto& argument = ap.add_positional_argument(name, short_name);
        // REQUIRE_FALSE(testing_argument_is_optional(argument));
    }
}

TEST_CASE("add_optional_argument should return a positional argument reference") {
    ap::argument_parser ap;

    SUBCASE("with just the long name") {
        const auto& argument = ap.add_optional_argument(name);
        // REQUIRE(testing_argument_is_optional(argument));
    }
    SUBCASE("with both names") {
        const auto& argument = ap.add_optional_argument(name, short_name);
        // REQUIRE(testing_argument_is_optional(argument));
    }
}

TEST_CASE("add_positional_argument should throw only when adding an argument "
          "with "
          "previously used name") {
    ap::argument_parser ap;

    ap.add_positional_argument(name, short_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(ap.add_positional_argument(other_name, other_short_name));
    }

    SUBCASE("adding argument with a previously used long name") {
        REQUIRE_THROWS_AS(ap.add_positional_argument(name), std::invalid_argument);
    }

    SUBCASE("adding argument with a previously used short name") {
        REQUIRE_THROWS_AS(
            ap.add_positional_argument(other_name, short_name), std::invalid_argument
        );
    }
}

TEST_CASE("add_optional_argument should throw only when adding an argument "
          "with "
          "previously used name") {
    ap::argument_parser ap;

    ap.add_optional_argument(name, short_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(ap.add_optional_argument(other_name, other_short_name));
    }

    SUBCASE("adding argument with a previously used long name") {
        REQUIRE_THROWS_AS(ap.add_optional_argument(name), std::invalid_argument);
    }

    SUBCASE("adding argument with a previously used short name") {
        REQUIRE_THROWS_AS(
            ap.add_optional_argument(other_name, short_name), std::invalid_argument
        );
    }
}

TEST_SUITE_END();
