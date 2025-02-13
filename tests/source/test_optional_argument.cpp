#define AP_TESTING

#include "doctest.h"
#include "optional_argument_test_fixture.hpp"

using namespace ap_testing;
using namespace ap::nargs;

using ap::argument::optional;
using ap::detail::argument_name;

namespace {

constexpr std::string_view primary_name = "test";
constexpr std::string_view secondary_name = "t";

using sut_value_type = int;
using invalid_value_type = double;
using sut_type = optional<sut_value_type>;

sut_type init_arg(std::string_view primary_name) {
    return sut_type(argument_name{primary_name});
}

sut_type init_arg(std::string_view primary_name, std::string_view secondary_name) {
    return sut_type(argument_name{primary_name, secondary_name});
}

constexpr std::string empty_str = "";
constexpr std::string invalid_value_str = "invalid_value";

constexpr sut_value_type value_1 = 1;
constexpr sut_value_type value_2 = 2;
constexpr sut_value_type default_value = 0;
constexpr sut_value_type implicit_value = -1;

const std::vector<sut_value_type> choices{1, 2, 3};
constexpr sut_value_type invalid_choice = 4;

const range non_default_range = range{1ull, choices.size()};

} // namespace

TEST_SUITE_BEGIN("test_optional_argument");

TEST_CASE_FIXTURE(optional_argument_test_fixture, "is_optional() should return true") {
    const auto sut = init_arg(primary_name);
    CHECK(sut.is_optional());
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "name() should return the proper argument_name instance"
) {
    SUBCASE("initialized with the primary name only") {
        const auto sut = init_arg(primary_name);
        const auto name = get_name(sut);

        CHECK(name.match(primary_name));
        CHECK_FALSE(name.match(secondary_name));
    }

    SUBCASE("initialized with the primary and secondary names") {
        const auto sut = init_arg(primary_name, secondary_name);
        const auto name = get_name(sut);

        CHECK(name.match(primary_name));
        CHECK(name.match(secondary_name));
    }
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "help() should return nullopt by default") {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(get_help(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "help() should return a massage set for the argument"
) {
    auto sut = init_arg(primary_name);

    constexpr std::string_view help_msg = "test help msg";
    sut.help(help_msg);

    const auto stored_help_msg = get_help(sut);

    REQUIRE(stored_help_msg);
    CHECK_EQ(stored_help_msg, help_msg);
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "is_required() should return false by default") {
    auto sut = init_arg(primary_name);
    CHECK_FALSE(is_required(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_required() should return true if the argument is set to be required"
) {
    auto sut = init_arg(primary_name);
    sut.required();
    CHECK(is_required(sut));
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "is_used() should return false by default") {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(is_used(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_used() should return true if the argument's flag has been used"
) {
    auto sut = init_arg(primary_name);
    REQUIRE_FALSE(is_used(sut));

    mark_used(sut);
    CHECK(is_used(sut));
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "nused() should return 0 by default") {
    const auto sut = init_arg(primary_name);
    CHECK_EQ(get_nused(sut), 0ull);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_used() should return the number of times the argument's flag has been used "
    "[number of mark_used() function calls]"
) {
    auto sut = init_arg(primary_name);

    constexpr std::size_t nused = 5ull;
    for (std::size_t n = 0ull; n < nused; ++n)
        mark_used(sut);

    CHECK_EQ(get_nused(sut), nused);
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "has_value() should return false by default") {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "has_value() should return true if value is set") {
    auto sut = init_arg(primary_name);
    set_value(sut, value_1);
    CHECK(has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return true if a default value is set and value() should return the "
    "default value"
) {
    auto sut = init_arg(primary_name);
    sut.default_value(default_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), default_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return false if only the implicit value is set but the argument is not used"
) {
    auto sut = init_arg(primary_name);
    sut.implicit_value(implicit_value);

    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return true if the implicit value is set and the agument is used"
) {
    auto sut = init_arg(primary_name);
    sut.implicit_value(implicit_value);

    mark_used(sut);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), implicit_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "has_parsed_values() should return false by default"
) {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_parsed_values() should return false regardles of the "
    "default_value and implicit_value parameters"
) {
    auto sut = init_arg(primary_name);

    SUBCASE("default_value") {
        sut.default_value(default_value);
        CHECK_FALSE(has_parsed_values(sut));
    }

    SUBCASE("implicit_value") {
        sut.implicit_value(implicit_value);
        CHECK_FALSE(has_parsed_values(sut));
    }

    SUBCASE("default_value and implicit_value") {
        sut.default_value(default_value);
        sut.implicit_value(implicit_value);
        CHECK_FALSE(has_parsed_values(sut));
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "has_parsed_values() should true if the value is set"
) {
    auto sut = init_arg(primary_name);
    set_value(sut, value_1);
    CHECK(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "value() should throw if the argument's value has not been set"
) {
    auto sut = init_arg(primary_name);

    REQUIRE_FALSE(has_value(sut));
    CHECK_THROWS_AS(static_cast<void>(get_value(sut)), std::logic_error);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "value() should return the argument's value if it has been set"
) {
    auto sut = init_arg(primary_name);
    set_value(sut, value_1);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "value() should return the default value if one has been provided and argument is not used"
) {
    auto sut = init_arg(primary_name);
    sut.default_value(value_1);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "value() should return the implicit value if one has been provided and argument is used"
) {
    auto sut = init_arg(primary_name);
    sut.implicit_value(implicit_value);

    mark_used(sut);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), implicit_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should throw when the given string cannot be converted to an instance of "
    "value_type"
) {
    auto sut = init_arg(primary_name);

    SUBCASE("given string is empty") {
        REQUIRE_THROWS_AS(set_value(sut, empty_str), ap::error::invalid_value);
        CHECK_FALSE(has_value(sut));
    }
    SUBCASE("given string is non-convertible to value_type") {
        REQUIRE_THROWS_AS(set_value(sut, invalid_value_str), ap::error::invalid_value);
        CHECK_FALSE(has_value(sut));
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should throw when the choices set does not contain the parsed value"
) {
    auto sut = init_arg(primary_name);
    sut.choices(choices);

    REQUIRE_THROWS_AS(set_value(sut, invalid_choice), ap::error::invalid_choice);
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should accept the given value only when no value has been set yet "
    "and if the given value is present in the choices set"
) {
    auto sut = init_arg(primary_name);
    sut.choices(choices);

    for (const sut_value_type& value : choices) {
        reset_value(sut);
        REQUIRE_FALSE(has_value(sut));

        REQUIRE_NOTHROW(set_value(sut, value));
        REQUIRE(has_value(sut));
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), value);
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should throw when a value has already been set (when nargs is default)"
) {
    auto sut = init_arg(primary_name);

    REQUIRE_NOTHROW(set_value(sut, value_1));
    REQUIRE(has_value(sut));
    CHECK_THROWS_AS(set_value(sut, value_2), ap::error::value_already_set);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should accept multiple values if nargs is not detault"
) {
    auto sut = init_arg(primary_name);
    sut.nargs(non_default_range);

    for (const auto value : choices) {
        REQUIRE_NOTHROW(set_value(sut, value));
    }

    const auto stored_values = get_values(sut);

    REQUIRE_EQ(stored_values.size(), choices.size());
    for (std::size_t i = 0; i < stored_values.size(); ++i) {
        REQUIRE_EQ(std::any_cast<sut_value_type>(stored_values[i]), choices[i]);
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "set_value(any) should perform the specified action"
) {
    auto sut = init_arg(primary_name);

    SUBCASE("valued action") {
        const auto double_valued_action = [](const sut_value_type& value) { return 2 * value; };
        sut.action<ap::action_type::transform>(double_valued_action);

        set_value(sut, value_1);

        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), double_valued_action(value_1));
    }

    SUBCASE("void action") {
        const auto double_void_action = [](sut_value_type& value) { value *= 2; };
        sut.action<ap::action_type::modify>(double_void_action);

        auto test_value = value_1;

        set_value(sut, test_value);

        double_void_action(test_value);
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), test_value);
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "nvalues_in_range() should return equivalent if nargs has not been set"
) {
    const auto sut = init_arg(primary_name);
    CHECK(std::is_eq(nvalues_in_range(sut)));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "nvalues_in_range() should return equivalent if a default value has been set"
) {
    auto sut = init_arg(primary_name);
    sut.nargs(non_default_range);

    sut.default_value(default_value);

    CHECK(std::is_eq(nvalues_in_range(sut)));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "nvalues_in_range() should return equivalent only when the number of values "
    "is in the specified range"
) {
    auto sut = init_arg(primary_name);
    sut.nargs(non_default_range);

    REQUIRE(std::is_lt(nvalues_in_range(sut)));

    for (const auto value : choices) {
        REQUIRE_NOTHROW(set_value(sut, value));
        CHECK(std::is_eq(nvalues_in_range(sut)));
    }

    REQUIRE_NOTHROW(set_value(sut, invalid_choice));
    CHECK(std::is_gt(nvalues_in_range(sut)));
}

TEST_SUITE_END();
