#define AP_TESTING

#include "doctest.h"
#include "positional_argument_test_fixture.hpp"

#include <ap/argument_parser.hpp>

#include <string_view>

using namespace ap_testing;
using ap::argument::positional_argument;

namespace {

constexpr std::string_view long_name = "test";
constexpr std::string_view short_name = "t";

using test_value_type = int;
using sut_type = positional_argument<test_value_type>;

sut_type prepare_argument(std::string_view name) {
    return sut_type(name);
}

sut_type prepare_argument(std::string_view name, std::string_view long_name) {
    return sut_type(name, long_name);
}

const std::string empty_str = "";
const std::string invalid_value_str = "invalid_value";

constexpr test_value_type value_1 = 1;
constexpr test_value_type value_2 = 2;

const std::vector<test_value_type> default_choices{ 1, 2, 3 };
constexpr test_value_type invalid_choice = 4;

} // namespace

TEST_SUITE_BEGIN("test_positional_argument");

TEST_CASE_FIXTURE(positional_argument_test_fixture, "is_optional() should return false") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut.is_optional());
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "name() should return value passed to the optional argument constructor for primary name") {
    const auto sut = prepare_argument(long_name);

    const auto name = sut_get_name(sut);

    REQUIRE_EQ(name, long_name);
    REQUIRE_NE(name, short_name);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "name() and short_name() should return value passed to the optional"
    "argument constructor for both primary and secondary names"
) {
    const auto sut = prepare_argument(long_name, short_name);

    const auto name = sut_get_name(sut);

    REQUIRE_EQ(name, long_name);
    REQUIRE_EQ(name, short_name);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "help() should return nullopt by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_get_help(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "help() should return message if one has been provided") {
    auto sut = prepare_argument(long_name);

    constexpr std::string_view help_msg = "test help msg";
    sut.help(help_msg);

    const auto stored_help_msg = sut_get_help(sut);

    REQUIRE(stored_help_msg);
    REQUIRE_EQ(stored_help_msg, help_msg);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "is_required() should return true") {
    auto sut = prepare_argument(long_name);

    REQUIRE(sut_is_required(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "is_used() should return false by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_is_used(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "is_used() should return true when argument contains value") {
    auto sut = prepare_argument(long_name);
    sut_set_value(sut, std::to_string(value_1));

    REQUIRE(sut_is_used(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "nused() should return 0 by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_EQ(sut_get_nused(sut), 0u);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "is_used() should return 1 when argument contains value") {
    auto sut = prepare_argument(long_name);
    sut_set_value(sut, std::to_string(value_1));

    REQUIRE_EQ(sut_get_nused(sut), 1u);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "has_value() should return false by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "has_value() should return true is value is set") {
    auto sut = prepare_argument(long_name);

    sut_set_value(sut, std::to_string(value_1));

    REQUIRE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "has_parsed_values() should return false by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_parsed_values(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "has_parsed_values() should true if the value is set") {
    auto sut = prepare_argument(long_name);

    sut_set_value(sut, std::to_string(value_1));

    REQUIRE(sut_has_parsed_values(sut));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "set_value(any) should throw when a value has already been set") {
    auto sut = prepare_argument(long_name);

    REQUIRE_NOTHROW(sut_set_value(sut, std::to_string(value_1)));
    REQUIRE(sut_has_value(sut));

    REQUIRE_THROWS_AS(sut_set_value(sut, std::to_string(value_2)), ap::error::value_already_set_error);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "set_value(any) should throw when value_type cannot be obtained from given string") {
    auto sut = prepare_argument(long_name);

    SUBCASE("given string is empty") {
        REQUIRE_THROWS_AS(sut_set_value(sut, empty_str), ap::error::invalid_value_error);

        REQUIRE_FALSE(sut_has_value(sut));
    }
    SUBCASE("given string is non-convertible to value_type") {
        REQUIRE_THROWS_AS(sut_set_value(sut, invalid_value_str), ap::error::invalid_value_error);

        REQUIRE_FALSE(sut_has_value(sut));
    }
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture, "set_value(any) should throw when parameter passed to value() is not present in the choices set"
) {
    auto sut = prepare_argument(long_name);

    sut_set_choices(sut, default_choices);

    REQUIRE_THROWS_AS(sut_set_value(sut, std::to_string(invalid_choice)), ap::error::invalid_choice_error);

    REQUIRE_FALSE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "set_value(any) should accept the given value only when no value has been set yet "
    "and if the given value is present in the choices set"
) {
    auto sut = prepare_argument(long_name);
    sut_set_choices(sut, default_choices);

    const std::vector<test_value_type> correct_values = default_choices;
    test_value_type value;

    for (const auto& v : correct_values) {
        SUBCASE("correct value") {
            value = v;
        }
    }

    CAPTURE(value);

    REQUIRE_NOTHROW(sut_set_value(sut, std::to_string(value)));
    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "set_value(any) should perform the specified action") {
    auto sut = prepare_argument(long_name);

    SUBCASE("valued action") {
        const auto double_valued_action = [](const test_value_type& value) { return 2 * value; };
        sut.action<ap::valued_action>(double_valued_action);

        sut_set_value(sut, std::to_string(value_1));

        REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), double_valued_action(value_1));
    }

    SUBCASE("void action") {
        const auto double_void_action = [](test_value_type& value) { value *= 2; };
        sut.action<ap::void_action>(double_void_action);

        auto test_value = value_1;

        sut_set_value(sut, std::to_string(test_value));

        double_void_action(test_value);
        REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), test_value);
    }
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "value() should return default any object if argument's value has not been set") {
    auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
    REQUIRE_THROWS_AS(std::any_cast<test_value_type>(sut_get_value(sut)), std::bad_any_cast);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "value() should return the argument's value if it has been set") {
    auto sut = prepare_argument(long_name);

    sut_set_value(sut, std::to_string(value_1));

    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "values() should throw logic_error") {
    auto sut = prepare_argument(long_name);

    REQUIRE_THROWS_AS(sut_get_values(sut), std::logic_error);
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "nvalues_in_range() should return less by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE(std::is_lt(sut_nvalues_in_range(sut)));
}

TEST_CASE_FIXTURE(positional_argument_test_fixture, "nvalues_in_range() should return equivalent if a value has been set") {
    auto sut = prepare_argument(long_name);

    sut_set_value(sut, std::to_string(value_1));

    REQUIRE(std::is_eq(sut_nvalues_in_range(sut)));
}

TEST_SUITE_END();
