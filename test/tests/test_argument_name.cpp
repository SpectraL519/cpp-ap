#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>

#include <sstream>
#include <string_view>

using namespace ap::detail;

namespace {

constexpr std::string_view name = "test";
constexpr std::string_view short_name = "t";

constexpr std::string_view other_name = "other";
constexpr std::string_view other_short_name = "o";

argument_name default_argument_name_long_name() {
    return argument_name(name);
}

argument_name default_argument_name_both_names() {
    return argument_name(name, short_name);
}

} // namespace

TEST_SUITE_BEGIN("test_argument_name");

TEST_CASE("argument_name.name member should be correctly initialized") {
    const auto arg_name = default_argument_name_long_name();

    REQUIRE_EQ(arg_name.name, name);
}

TEST_CASE("argument_name members should be correctly initialized") {
    const auto arg_name = default_argument_name_both_names();

    REQUIRE_EQ(arg_name.name, name);

    REQUIRE(arg_name.short_name);
    REQUIRE_EQ(arg_name.short_name.value(), short_name);
}

TEST_CASE("argument_name::operator==(argument_name) should return true if "
          "long "
          "names are equal") {
    const auto arg_name_a = default_argument_name_long_name();
    const auto arg_name_b = default_argument_name_both_names();

    REQUIRE(arg_name_a == arg_name_b);
}

TEST_CASE("argument_name::operator==(argument_name) should return false if "
          "long names are not equal") {
    const auto arg_name_a = default_argument_name_long_name();
    const auto arg_name_b = argument_name(other_name, other_short_name);

    REQUIRE_FALSE(arg_name_a == arg_name_b);
}

TEST_CASE("argument_name::operator==(string_view) should return true if at "
          "least one name matches") {
    SUBCASE("argument_name with long name only") {
        const auto arg_name = default_argument_name_long_name();

        REQUIRE(arg_name == name);
    }

    SUBCASE("argument_name with both names") {
        const auto arg_name = default_argument_name_both_names();

        REQUIRE(arg_name == name);
        REQUIRE(arg_name == short_name);
    }
}

TEST_CASE("argument_name::operator==(string_view) should return false if no "
          "name matches") {
    SUBCASE("argument_name with long name only") {
        const auto arg_name = default_argument_name_long_name();

        REQUIRE_FALSE(arg_name == other_name);
        REQUIRE_FALSE(arg_name == other_short_name);
    }

    SUBCASE("argument_name with both names") {
        const auto arg_name = default_argument_name_both_names();

        REQUIRE_FALSE(arg_name == other_name);
        REQUIRE_FALSE(arg_name == other_short_name);
    }
}

TEST_CASE("argument_name should be correctly constructed from another "
          "argument_name object") {
    SUBCASE("with long name only") {
        const auto arg_name = default_argument_name_long_name();

        SUBCASE("using copy constructor") {
            const auto arg_name_copy = argument_name(arg_name);
            REQUIRE_EQ(arg_name_copy, arg_name);
        }

        SUBCASE("using move constructor") {
            const auto arg_name_moved = argument_name(std::move(arg_name));
            REQUIRE_EQ(arg_name_moved, arg_name);
        }

        SUBCASE("using assignment operator") {
            const auto arg_name_copy = arg_name;
            REQUIRE_EQ(arg_name_copy, arg_name);
        }
    }

    SUBCASE("with both names") {
        const auto arg_name = default_argument_name_both_names();

        SUBCASE("using copy constructor") {
            const auto arg_name_copy = argument_name(arg_name);
            REQUIRE_EQ(arg_name_copy, arg_name);
        }

        SUBCASE("using move constructor") {
            const auto arg_name_moved = argument_name(std::move(arg_name));
            REQUIRE_EQ(arg_name_moved, arg_name);
        }

        SUBCASE("using assignment operator") {
            const auto arg_name_copy = arg_name;
            REQUIRE_EQ(arg_name_copy, arg_name);
        }
    }
}

TEST_CASE("operator<< should push correct data to the output stream") {
    std::stringstream ss, expected_ss;

    SUBCASE("argument_name with long name only") {
        expected_ss << default_argument_name_long_name();
        ss << "[" << name << "]";
    }

    SUBCASE("argument_name with both names") {
        expected_ss << default_argument_name_both_names();
        ss << "[" << name << "," << short_name << "]";
    }

    REQUIRE_EQ(ss.str(), expected_ss.str());
}

TEST_SUITE_END();
