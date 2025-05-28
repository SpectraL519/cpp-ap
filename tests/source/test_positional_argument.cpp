#define AP_TESTING

#include "doctest.h"
#include "positional_argument_test_fixture.hpp"

using namespace ap_testing;

using ap::argument::positional;
using ap::detail::argument_name;

namespace {

constexpr std::string_view primary_name = "test";
constexpr std::string_view secondary_name = "t";
constexpr std::string_view help_msg = "test help msg";

using sut_value_type = int;
using sut_type = positional<sut_value_type>;

sut_type init_arg(std::string_view primary_name) {
    return sut_type{argument_name{primary_name}};
}

sut_type init_arg(std::string_view primary_name, std::string_view secondary_name) {
    return sut_type{
        argument_name{primary_name, secondary_name}
    };
}

constexpr std::string empty_str = "";
constexpr std::string invalid_value_str = "invalid value";

constexpr sut_value_type value_1 = 1;
constexpr sut_value_type value_2 = 2;

const std::vector<sut_value_type> choices{1, 2, 3};
constexpr sut_value_type invalid_choice = 4;

} // namespace

TEST_SUITE_BEGIN("test_positional_argument");

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "name() should return the proper argument_name instance"
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

TEST_CASE_FIXTURE(positional_argument_test_fixture, "help() should return nullopt by default") {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(get_help(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "help() should return a massage set for the argument"
) {
    auto sut = init_arg(primary_name);
    sut.help(help_msg);

    const auto stored_help_msg = get_help(sut);

    REQUIRE(stored_help_msg);
    CHECK_EQ(stored_help_msg, help_msg);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "desc(verbose=false) should return an argument_descriptor with no params"
) {
    constexpr bool verbose = false;

    auto sut = init_arg(primary_name, secondary_name);

    auto desc = get_desc(sut, verbose);
    REQUIRE_EQ(desc.name, get_name(sut).str());
    CHECK_FALSE(desc.help);
    CHECK(desc.params.empty());

    // with a help msg
    sut.help(help_msg);
    desc = get_desc(sut, verbose);
    REQUIRE_EQ(desc.name, get_name(sut).str());
    CHECK(desc.help);
    CHECK_EQ(desc.help.value(), help_msg);
    CHECK(desc.params.empty());
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "desc(verbose=true) should return an argument_descriptor with non-default params"
) {
    constexpr bool verbose = true;

    auto sut = init_arg(primary_name, secondary_name);

    auto desc = get_desc(sut, verbose);
    REQUIRE_EQ(desc.name, get_name(sut).str());
    CHECK_FALSE(desc.help);
    CHECK(desc.params.empty());

    // with a help msg
    sut.help(help_msg);

    desc = get_desc(sut, verbose);
    REQUIRE(desc.help);
    CHECK_EQ(desc.help.value(), help_msg);
    CHECK(desc.params.empty());

    // with the choices parameter
    sut.choices(choices);
    desc = get_desc(sut, verbose);
    REQUIRE_FALSE(desc.params.empty());
    const auto& choices_param = desc.params.back();
    CHECK_EQ(choices_param.name, "choices");
    CHECK_EQ(choices_param.value, ap::detail::join(choices, ", "));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "is_required() should return true") {
    auto sut = init_arg(primary_name);
    CHECK(is_required(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "is_used() should return false by default") {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(is_used(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "is_used() should return true when argument contains a value"
) {
    auto sut = init_arg(primary_name);
    REQUIRE_FALSE(is_used(sut));

    set_value(sut, value_1);
    CHECK(is_used(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "count() should return 0 by default") {
    const auto sut = init_arg(primary_name);
    CHECK_EQ(get_count(sut), 0ull);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "count() should return 1 when argument contains a value"
) {
    auto sut = init_arg(primary_name);
    set_value(sut, value_1);

    CHECK_EQ(get_count(sut), 1ull);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "has_value() should return false by default") {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "has_value() should return true if the value is set"
) {
    auto sut = init_arg(primary_name);
    set_value(sut, value_1);

    CHECK(has_value(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "has_parsed_values() should return false by default"
) {
    const auto sut = init_arg(primary_name);
    CHECK_FALSE(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "has_parsed_values() should true if the value is set"
) {
    auto sut = init_arg(primary_name);
    set_value(sut, value_1);

    CHECK(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "set_value(any) should throw when a value has already been set"
) {
    auto sut = init_arg(primary_name);

    REQUIRE_NOTHROW(set_value(sut, value_1));
    REQUIRE(has_value(sut));

    CHECK_THROWS_AS(set_value(sut, value_2), ap::parsing_failure);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "set_value(any) should throw when the given string cannot be converted to an instance of "
    "value_type"
) {
    auto sut = init_arg(primary_name);

    SUBCASE("given string is empty") {
        REQUIRE_THROWS_AS(set_value(sut, empty_str), ap::parsing_failure);
        CHECK_FALSE(has_value(sut));
    }

    SUBCASE("given string is non-convertible to value_type") {
        REQUIRE_THROWS_AS(set_value(sut, invalid_value_str), ap::parsing_failure);
        CHECK_FALSE(has_value(sut));
    }
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "set_value(any) should throw when the choices set does not contain the parsed value"
) {
    auto sut = init_arg(primary_name);
    sut.choices(choices);

    REQUIRE_THROWS_AS(set_value(sut, invalid_choice), ap::parsing_failure);
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
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
    positional_argument_test_fixture, "set_value(any) should perform the specified action"
) {
    auto sut = init_arg(primary_name);

    SUBCASE("observe action") {
        const auto is_power_of_two = [](const sut_value_type n) {
            if (not ((n > 0) and (n & (n - 1)) == 0)) {
                throw std::runtime_error(std::format("Value `{}` is not a power of 2", n));
            }
        };

        sut.action<ap::action_type::observe>(is_power_of_two);

        CHECK_THROWS_AS(set_value(sut, 3), std::runtime_error);

        sut_value_type valid_value = 16;
        REQUIRE_NOTHROW(set_value(sut, valid_value));
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), valid_value);
    }

    SUBCASE("transform action") {
        const auto double_action = [](const sut_value_type& value) { return 2 * value; };
        sut.action<ap::action_type::transform>(double_action);

        set_value(sut, value_1);

        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), double_action(value_1));
    }

    SUBCASE("modify action") {
        const auto double_action = [](sut_value_type& value) { value *= 2; };
        sut.action<ap::action_type::modify>(double_action);

        auto test_value = value_1;

        set_value(sut, test_value);

        double_action(test_value);
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), test_value);
    }
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "value() should throw if the argument's value has not been set"
) {
    auto sut = init_arg(primary_name);

    REQUIRE_FALSE(has_value(sut));
    CHECK_THROWS_AS(static_cast<void>(get_value(sut)), std::logic_error);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "value() should return the argument's value if it has been set"
) {
    auto sut = init_arg(primary_name);

    set_value(sut, value_1);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "values() should throw logic_error") {
    auto sut = init_arg(primary_name);
    CHECK_THROWS_AS(get_values(sut), std::logic_error);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "nvalues_ordering() should return less by default"
) {
    const auto sut = init_arg(primary_name);
    CHECK(std::is_lt(nvalues_ordering(sut)));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "nvalues_ordering() should return equivalent if a value has been set"
) {
    auto sut = init_arg(primary_name);
    set_value(sut, value_1);

    CHECK(std::is_eq(nvalues_ordering(sut)));
}

TEST_SUITE_END();
