#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <positional_argument_test_fixture.hpp>

#include <string_view>

using namespace ap_testing;
using ap::detail::positional_argument;

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

} // namespace

TEST_SUITE_BEGIN("test_positional_argument");

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "is_optional() should return false"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut.is_optional());
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "has_value() should return false by default"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "has_value() should return true is value is set"
) {
    auto sut = prepare_argument(long_name);

    test_value_type value{};
    sut_set_value(sut, std::to_string(value));

    REQUIRE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "value(any) should throw when value_type cannot be obtained from given string"
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
    positional_argument_test_fixture,
    "value() should return default any object if argument's value has not been set"
) {
    auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
    REQUIRE_THROWS_AS(std::any_cast<test_value_type>(sut_get_value(sut)), std::bad_any_cast);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "value() should return the argument's value if it has been set"
) {
    auto sut = prepare_argument(long_name);

    test_value_type value{};
    sut_set_value(sut, std::to_string(value));

    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "name() should return value passed to the optional argument constructor for long name"
) {
    const auto sut = prepare_argument(long_name);

    const auto name = sut_get_name(sut);

    REQUIRE_EQ(name, long_name);
    REQUIRE_NE(name, short_name);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "name() and short_name() should return value passed to the optional"
    "argument constructor for both long and short names"
) {
    const auto sut = prepare_argument(long_name, short_name);

    const auto name = sut_get_name(sut);

    REQUIRE_EQ(name, long_name);
    REQUIRE_EQ(name, short_name);
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "is_required() should return true"
) {
    auto sut = prepare_argument(long_name);

    REQUIRE(sut_is_required(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
    "help() should return nullopt by default"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_get_help(sut));
}

TEST_CASE_FIXTURE(
    positional_argument_test_fixture,
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
