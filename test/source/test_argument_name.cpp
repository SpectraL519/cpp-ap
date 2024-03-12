#define AP_TESTING

#include "doctest.h"

#include <ap/argument_parser.hpp>

#include <sstream>
#include <string_view>

using namespace ap_testing;
using namespace ap::argument::detail;

namespace {

constexpr std::string_view primary_name = "test";
constexpr std::string_view secondary_name = "t";

constexpr std::string_view other_primary_name = "other";
constexpr std::string_view other_secondary_name = "o";

argument_name default_argument_name_primary_only() {
    return argument_name(primary_name);
}

argument_name default_argument_name_primary_and_secondary() {
    return argument_name(primary_name, secondary_name);
}

} // namespace

TEST_SUITE_BEGIN("test_argument_name");

TEST_CASE("argument_name.primary member should be correctly "
          "initialized") {
    const auto arg_name = default_argument_name_primary_only();

    REQUIRE_EQ(arg_name.primary, primary_name);
}

TEST_CASE("argument_name members should be correctly "
          "initialized") {
    const auto arg_name = default_argument_name_primary_and_secondary();

    REQUIRE_EQ(arg_name.primary, primary_name);

    REQUIRE(arg_name.secondary);
    REQUIRE_EQ(arg_name.secondary.value(), secondary_name);
}

TEST_CASE("argument_name::operator==(argument_name) should "
          "return true if "
          "primary names are equal") {
    const auto arg_name_a = default_argument_name_primary_only();
    const auto arg_name_b = default_argument_name_primary_and_secondary();

    REQUIRE(arg_name_a == arg_name_b);
}

TEST_CASE("argument_name::operator==(argument_name) should "
          "return false if "
          "primary names are not equal") {
    const auto arg_name_a = default_argument_name_primary_only();
    const auto arg_name_b = argument_name(other_primary_name, other_secondary_name);

    REQUIRE_FALSE(arg_name_a == arg_name_b);
}

TEST_CASE("argument_name::operator==(string_view) should "
          "return true if at "
          "least one name matches") {
    SUBCASE("argument_name with primary name only") {
        const auto arg_name = default_argument_name_primary_only();

        REQUIRE(arg_name == primary_name);
    }

    SUBCASE("argument_name with both names") {
        const auto arg_name = default_argument_name_primary_and_secondary();

        REQUIRE(arg_name == primary_name);
        REQUIRE(arg_name == secondary_name);
    }
}

TEST_CASE("argument_name::operator==(string_view) should "
          "return false if no "
          "name matches") {
    SUBCASE("argument_name with primary name only") {
        const auto arg_name = default_argument_name_primary_only();

        REQUIRE_FALSE(arg_name == other_primary_name);
        REQUIRE_FALSE(arg_name == other_secondary_name);
    }

    SUBCASE("argument_name with both names") {
        const auto arg_name = default_argument_name_primary_and_secondary();

        REQUIRE_FALSE(arg_name == other_primary_name);
        REQUIRE_FALSE(arg_name == other_secondary_name);
    }
}

TEST_CASE("operator<< should push correct data to the output stream") {
    std::stringstream ss, expected_ss;

    SUBCASE("argument_name with primary name only") {
        ss << default_argument_name_primary_only();
        expected_ss << "[" << primary_name << "]";
    }

    SUBCASE("argument_name with both names") {
        ss << default_argument_name_primary_and_secondary();
        expected_ss << "[" << primary_name << "," << secondary_name << "]";
    }

    CAPTURE(ss);
    CAPTURE(expected_ss);

    REQUIRE_EQ(ss.str(), expected_ss.str());
}

TEST_SUITE_END();
