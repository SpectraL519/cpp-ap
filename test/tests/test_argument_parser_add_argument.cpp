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

TEST_CASE("add_argument should return the correct argument type based on "
          "template parameter") {
    ap::argument_parser ap;

    SUBCASE("positional_argument when ap::positional is passed as the parameter") {
        SUBCASE("with just the long name") {
            const auto& argument = ap.add_argument<ap::positional>(name);
            REQUIRE(is_positional<decltype(argument)>());
        }
        SUBCASE("with both names") {
            const auto& argument = ap.add_argument<ap::positional>(name, short_name);
            REQUIRE(is_positional<decltype(argument)>());
        }
    }

    SUBCASE("optional_argument when ap::optional is passed as the parameter") {
        SUBCASE("with just the long name") {
            const auto& argument = ap.add_argument<ap::optional>(name);
            REQUIRE(is_optional<decltype(argument)>());
        }
        SUBCASE("with both names") {
            const auto& argument = ap.add_argument<ap::optional>(name, short_name);
            REQUIRE(is_optional<decltype(argument)>());
        }
    }
}

TEST_SUITE_END();
