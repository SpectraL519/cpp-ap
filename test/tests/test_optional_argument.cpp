#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <argument_test_fixture.hpp>

#include <string_view>

using namespace ap_testing;
using ap::detail::optional_argument;



namespace {

using test_value_type = int;

constexpr std::string_view long_name = "test";
constexpr std::string_view short_name = "t";


argument_test_fixture::argument_ptr_type prepare_argument(std::string_view name) {
    return std::make_unique<optional_argument<test_value_type>>(name);
}

argument_test_fixture::argument_ptr_type prepare_argument(
    std::string_view name, std::string_view long_name
) {
    return std::make_unique<optional_argument<test_value_type>>(name, long_name);
}

} // namespace



TEST_SUITE_BEGIN("test_optional_argument");

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "is_optional() should return true"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE(sut->is_optional());
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "has_value() should return false by default"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "has_value() should return true is value is set"
) {
    auto sut = prepare_argument(long_name);

    test_value_type value{};
    sut_set_value(sut, std::to_string(value));

    REQUIRE(sut_has_value(sut));
}

// TODO: default value tests
// TODO: test invalid type

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "value() should return default any object if argument's value has not been set"
) {
    auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_has_value(sut));
    REQUIRE_THROWS_AS(
        std::any_cast<test_value_type>(sut_get_value(sut)), std::bad_any_cast
    );
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "value() should return the argument's value if it has been set"
) {
    auto sut = prepare_argument(long_name);

    test_value_type value{};
    sut_set_value(sut, std::to_string(value));

    REQUIRE(sut_has_value(sut));
    REQUIRE_EQ(std::any_cast<test_value_type>(sut_get_value(sut)), value);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "name() should return value passed to the optional argument constructor for long name"
) {
    const auto sut = prepare_argument(long_name);

    const auto name = sut_get_name(sut);;

    REQUIRE_EQ(name, long_name);
    REQUIRE_NE(name, short_name);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "name() and short_name() should return value passed to the optional argument constructor for both long and short names"
) {
    const auto sut = prepare_argument(long_name, short_name);

    const auto name = sut_get_name(sut);;

    REQUIRE_EQ(name, long_name);
    REQUIRE_EQ(name, short_name);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "required() should return false by default"
) {
    auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_is_required(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "required() should return the value it has been set to"
) {
    auto sut = prepare_argument(long_name);
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);
    sut->required(required);

    REQUIRE_EQ(sut_is_required(sut), required);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "help() should return nullopt by default"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_get_help(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "help() should return message if one has been provided"
) {
    auto sut = prepare_argument(long_name);

    constexpr std::string_view help_msg = "test help msg";
    sut->help(help_msg);

    const auto stored_help_msg = sut_get_help(sut);

    REQUIRE(stored_help_msg);
    REQUIRE_EQ(stored_help_msg, help_msg);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "defaul_value() should return null any instance by default"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(sut_get_default_value(sut).has_value());
}

// TODO: test invalid type

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "defaul_value() should return value if one has been provided"
) {
    auto sut = prepare_argument(long_name);

    test_value_type default_value{};
    sut->default_value(default_value);

    const auto stored_default_value = sut_get_default_value(sut);

    REQUIRE(stored_default_value.has_value());
    REQUIRE_EQ(std::any_cast<test_value_type>(stored_default_value), default_value);
}

TEST_SUITE_END();
