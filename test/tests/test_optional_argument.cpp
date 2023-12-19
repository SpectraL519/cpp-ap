#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <optional_argument_test_fixture.hpp>

#include <ranges>
#include <string_view>

using namespace ap_testing;
using namespace ap::nargs;

using ap::argument::optional_argument;

namespace {

constexpr std::string_view long_name = "test";
constexpr std::string_view short_name = "t";

using test_value_type = int;
using invalid_value_type = double;
using sut_type = optional_argument<test_value_type>;

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
constexpr test_value_type default_value = 0;

const range non_default_range = at_least(1);

const std::vector<test_value_type> default_choices{1, 2, 3};
constexpr test_value_type invalid_choice = 4;

} // namespace

TEST_SUITE_BEGIN("test_optional_argument");

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_optional() should return true"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE(sut.is_optional());
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return false by default"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return true if value is set"
) {
    auto sut = prepare_argument(long_name);

    sut_set_value(sut, std::to_string(value_1));

    REQUIRE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return true if a default value is set"
) {
    auto sut = prepare_argument(long_name);

    sut.default_value(default_value);

    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), default_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should throw when value_type cannot be obtained from given string"
) {
    auto sut = prepare_argument(long_name);

    SUBCASE("given string is empty") {
        REQUIRE_THROWS_AS(sut_set_value(sut, empty_str), std::invalid_argument);
        REQUIRE_FALSE(sut_has_value(sut));
    }
    SUBCASE("given string is non-convertible to value_type") {
        REQUIRE_THROWS_AS(sut_set_value(sut, invalid_value_str), std::invalid_argument);
        REQUIRE_FALSE(sut_has_value(sut));
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "value() should return default any object if argument's value has not been set"
) {
    auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
    REQUIRE_THROWS_AS(std::any_cast<test_value_type>(sut_get_value(sut)), std::bad_any_cast);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "value() should return the argument's value if it has been set"
) {
    auto sut = prepare_argument(long_name);

    sut_set_value(sut, std::to_string(value_1));

    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "value() should return the default value if one has been provided and value has not been parsed"
) {
    auto sut = prepare_argument(long_name);

    sut.default_value(value_1);

    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should throw when a value has already benn set when nargs is default"
) { // TODO: replace with "when action is store"
    auto sut = prepare_argument(long_name);

    REQUIRE_NOTHROW(sut_set_value(sut, std::to_string(value_1)));
    REQUIRE(sut_has_value(sut));

    REQUIRE_THROWS_AS(sut_set_value(sut, std::to_string(value_2)), std::runtime_error);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should accept multiple values if nargs is not default"
) { // TODO: replace with "when action is append"
    auto sut = prepare_argument(long_name);
    sut.nargs(non_default_range);

    for (const auto value : default_choices) {
        REQUIRE_NOTHROW(sut_set_value(sut, std::to_string(value)));
    }

    const auto stored_values = sut_get_values(sut);

    REQUIRE_EQ(stored_values.size(), default_choices.size());
    for (std::size_t i = 0; i < stored_values.size(); i++) {
        REQUIRE_EQ(std::any_cast<test_value_type>(stored_values[i]), default_choices[i]);
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should accept the given value only when it's present in the choices set"
) {
    auto sut = prepare_argument(long_name);
    sut_set_choices(sut, default_choices);

    const std::vector<test_value_type> correct_values = default_choices;
    test_value_type value;

    for (const auto& v : correct_values) {
        SUBCASE("correct value") { value = v; }
    }

    CAPTURE(value);

    REQUIRE_NOTHROW(sut_set_value(sut, std::to_string(value)));
    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should throw when parameter passed to value() is not present in the choices set"
) {
    auto sut = prepare_argument(long_name);

    sut_set_choices(sut, default_choices);

    REQUIRE_THROWS_AS(sut_set_value(sut, std::to_string(invalid_choice)), std::invalid_argument);
    REQUIRE_FALSE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "default_value(any) should throw when value_type cannot be obtained from given any object"
) {
    auto sut = prepare_argument(long_name);

    REQUIRE_THROWS_AS(sut.default_value(invalid_value_type{}), std::invalid_argument);
    REQUIRE_FALSE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "default_value(any) should accept the given value only when it's present in the choices set"
) {
    auto sut = prepare_argument(long_name);
    sut_set_choices(sut, default_choices);

    const std::vector<test_value_type> correct_values = default_choices;
    test_value_type value;

    for (const auto& v : correct_values) {
        SUBCASE("correct value") { value = v; }
    }

    CAPTURE(value);

    REQUIRE_NOTHROW(sut.default_value(value));
    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "default_value(any) should throw when parameter passed to value() is not found in _choices"
) {
    auto sut = prepare_argument(long_name);

    test_value_type invalid_value = 4;
    sut_set_choices(sut, default_choices);

    REQUIRE_THROWS_AS(sut.default_value(invalid_value), std::invalid_argument);
    REQUIRE_FALSE(sut_has_value(sut));
}


TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "name() should return value passed to the optional argument constructor for long name"
) {
    const auto sut = prepare_argument(long_name);

    const auto name = sut_get_name(sut);

    REQUIRE_EQ(name, long_name);
    REQUIRE_NE(name, short_name);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "name() and short_name() should return value passed to the optional"
    "argument constructor for both long and short names"
) {
    const auto sut = prepare_argument(long_name, short_name);

    const auto name = sut_get_name(sut);

    REQUIRE_EQ(name, long_name);
    REQUIRE_EQ(name, short_name);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_required() should return false by default"
) {
    auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_is_required(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_required() should return true is argument is set to be required"
) {
    auto sut = prepare_argument(long_name);

    sut.required();

    REQUIRE(sut_is_required(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "help() should return nullopt by default"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_get_help(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "help() should return message if one has been provided"
) {
    auto sut = prepare_argument(long_name);

    constexpr std::string_view help_msg = "test help msg";
    sut.help(help_msg);

    const auto stored_help_msg = sut_get_help(sut);

    REQUIRE(stored_help_msg);
    REQUIRE_EQ(stored_help_msg, help_msg);
}

TEST_SUITE_END();
