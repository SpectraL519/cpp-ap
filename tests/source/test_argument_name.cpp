#include "doctest.h"

#include <ap/detail/argument_name.hpp>

#include <sstream>

using namespace ap::detail;

TEST_SUITE_BEGIN("test_argument_name");

namespace {

constexpr std::string_view primary_1 = "primary_1";
constexpr std::string_view secondary_1 = "s1";

const auto primary_1_opt = std::make_optional<std::string>(primary_1);
const auto secondary_1_opt = std::make_optional<std::string>(secondary_1);

const argument_name arg_name_primary_1{primary_1_opt};
const argument_name arg_name_secondary_1{std::nullopt, secondary_1_opt};
const argument_name arg_name_full_1{primary_1_opt, secondary_1_opt};

constexpr std::string_view primary_2 = "primary_2";
constexpr std::string_view secondary_2 = "s2";

const auto primary_2_opt = std::make_optional<std::string>(primary_2);
const auto secondary_2_opt = std::make_optional<std::string>(secondary_2);

const argument_name arg_name_primary_2{primary_2_opt};
const argument_name arg_name_secondary_2{std::nullopt, secondary_2_opt};
const argument_name arg_name_full_2{primary_2_opt, secondary_2_opt};

} // namespace

TEST_CASE("arugment_name construction should throw if both primary and secondary names are null") {
    CHECK_THROWS_WITH_AS(
        (argument_name{std::nullopt, std::nullopt}),
        "An argument name cannot be empty! At least one of primary/secondary must be specified",
        std::logic_error
    );
}

TEST_CASE("argument_name members should be initialzed properly") {
    CHECK_EQ(arg_name_primary_1.primary, primary_1);
    CHECK_FALSE(arg_name_primary_1.secondary.has_value());

    CHECK_FALSE(arg_name_secondary_1.primary.has_value());
    CHECK_EQ(arg_name_secondary_1.secondary, secondary_1);

    CHECK_EQ(arg_name_full_1.primary, primary_1);
    CHECK_EQ(arg_name_full_1.secondary, secondary_1);
}

TEST_CASE("operator==(argument_name) should return true only if both primary and secondary names "
          "have the same values (or no values)") {
    // exact match
    CHECK_EQ(arg_name_primary_1, arg_name_primary_1);
    CHECK_EQ(arg_name_secondary_1, arg_name_secondary_1);
    CHECK_EQ(arg_name_full_1, arg_name_full_1);

    // values of different name members set
    CHECK_NE(arg_name_full_1, arg_name_primary_1);
    CHECK_NE(arg_name_full_1, arg_name_secondary_1);
    CHECK_NE(arg_name_primary_1, arg_name_secondary_1);

    // different values of name members set
    CHECK_NE(arg_name_primary_1, arg_name_primary_2);
    CHECK_NE(arg_name_secondary_1, arg_name_secondary_2);
    CHECK_NE(arg_name_full_1, arg_name_full_2);
}

TEST_CASE("match(string_view, any) should return true if the given string matches at least one name"
) {
    // argument_name with primary name only
    CHECK(arg_name_primary_1.match(primary_1));

    // argument_name with secondary name only
    CHECK(arg_name_secondary_1.match(secondary_1));

    // argument_name with both names
    CHECK(arg_name_full_1.match(primary_1));
    CHECK(arg_name_full_1.match(secondary_1));
}

TEST_CASE("match(string_view, any) should return false if the given string dosn't match any name") {
    // argument_name with primary name only
    CHECK_FALSE(arg_name_primary_1.match(primary_2));
    CHECK_FALSE(arg_name_primary_1.match(secondary_2));

    // argument_name with secondary name only
    CHECK_FALSE(arg_name_secondary_1.match(primary_2));
    CHECK_FALSE(arg_name_secondary_1.match(secondary_2));

    // argument_name with both names
    CHECK_FALSE(arg_name_full_1.match(primary_2));
    CHECK_FALSE(arg_name_full_1.match(secondary_2));
}

TEST_CASE("match(string_view, primary) should return true only if the given string matches the "
          "primary name") {
    // argument_name with primary name only
    CHECK(arg_name_primary_1.match(primary_1, argument_name::m_primary));
    CHECK_FALSE(arg_name_primary_1.match(secondary_1, argument_name::m_primary));

    // argument_name with secondary name only
    CHECK_FALSE(arg_name_secondary_1.match(primary_1, argument_name::m_primary));
    CHECK_FALSE(arg_name_secondary_1.match(secondary_1, argument_name::m_primary));

    // argument_name with both names
    CHECK(arg_name_full_1.match(primary_1, argument_name::m_primary));
    CHECK_FALSE(arg_name_full_1.match(secondary_1, argument_name::m_primary));
}

TEST_CASE("match(string_view, secondary) should return true only if the given string matches the "
          "secondary name") {
    // argument_name with primary name only
    CHECK_FALSE(arg_name_primary_1.match(primary_1, argument_name::m_secondary));
    CHECK_FALSE(arg_name_primary_1.match(secondary_1, argument_name::m_secondary));

    // argument_name with secondary name only
    CHECK_FALSE(arg_name_secondary_1.match(primary_1, argument_name::m_secondary));
    CHECK(arg_name_secondary_1.match(secondary_1, argument_name::m_secondary));

    // argument_name with both names
    CHECK_FALSE(arg_name_full_1.match(primary_1, argument_name::m_secondary));
    CHECK(arg_name_full_1.match(secondary_1, argument_name::m_secondary));
}

TEST_CASE("match(argument_name) should return true if either the primary or the secondary name of "
          "the passed argument_name matches at least one name") {
    // argument_name with primary name only
    CHECK(arg_name_primary_1.match(arg_name_primary_1));
    CHECK(arg_name_primary_1.match(argument_name{primary_2_opt, primary_1_opt}));

    // argument_name with primary name only
    CHECK(arg_name_secondary_1.match(arg_name_secondary_1));
    CHECK(arg_name_secondary_1.match(argument_name{secondary_1_opt, secondary_2_opt}));

    // argument_name with both names
    CHECK(arg_name_full_1.match(argument_name{primary_1_opt, secondary_2_opt}));
    CHECK(arg_name_full_1.match(argument_name{secondary_1_opt, primary_1_opt}));
    CHECK(arg_name_full_1.match(argument_name{primary_2_opt, primary_1_opt}));
    CHECK(arg_name_full_1.match(argument_name{primary_2_opt, secondary_1_opt}));
}

TEST_CASE("match(argument_name) should return false if neither the primary nor the secondary name "
          "of the passed argument_name matches at least one name") {
    CHECK_FALSE(arg_name_primary_1.match(arg_name_full_2));
    CHECK_FALSE(arg_name_secondary_1.match(arg_name_full_2));
    CHECK_FALSE(arg_name_full_1.match(arg_name_full_2));
}

TEST_CASE("operator<< should push correct data to the output stream") {
    std::stringstream ss, expected_ss;

    SUBCASE("argument_name with primary name only") {
        ss << arg_name_primary_1;
        expected_ss << primary_1;
    }

    SUBCASE("argument_name with secondary name only") {
        ss << arg_name_secondary_1;
        expected_ss << secondary_1;
    }

    SUBCASE("argument_name with both names") {
        ss << arg_name_full_1;
        expected_ss << primary_1 << ", " << secondary_1;
    }

    CAPTURE(ss);
    CAPTURE(expected_ss);

    REQUIRE_EQ(ss.str(), expected_ss.str());
}

TEST_SUITE_END(); // test_argument_name
