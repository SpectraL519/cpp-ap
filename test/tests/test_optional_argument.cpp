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
    "optional argument should be optional and not positional"
) {
    const auto sut = prepare_argument(long_name);

    REQUIRE(sut_is_optional(sut));
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

// TEST_CASE_FIXTURE(argument_test_fixture, "value() should return the argument's value if it has been set") {
//     auto sut = prepare_argument(long_name);

//     test_value_type value{};
//     testing_argument_set_value(argument, value);

//     REQUIRE(sut_has_value(sut));
//     REQUIRE_EQ(std::any_cast<test_value_type>(sut_value(sut)),
//     value);
// }

TEST_CASE_FIXTURE(argument_test_fixture, "name() should return value passed to the optional argument "
          "constructor for long name") {
    const auto sut = prepare_argument(long_name);

    const auto name = testing_argument_get_name(argument);

    REQUIRE_EQ(name, long_name);
    REQUIRE_NE(name, short_name);
}

TEST_CASE_FIXTURE(argument_test_fixture, "name() and short_name() should return value passed to the optional "
          "argument "
          "constructor for both long and short names") {
    const auto argument = default_optional_argument_both_names();

    const auto name = testing_argument_get_name(argument);

    REQUIRE_EQ(name, long_name);
    REQUIRE_EQ(name, short_name);
}

TEST_CASE_FIXTURE(argument_test_fixture, "required() should return false by default") {
    auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(testing_argument_is_required(argument));
}

TEST_CASE_FIXTURE(argument_test_fixture, "required() should return the value it has been set to") {
    auto sut = prepare_argument(long_name);
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);

    argument.required(required);

    REQUIRE_EQ(testing_argument_is_required(argument), required);
}

TEST_CASE_FIXTURE(argument_test_fixture, "help() should return nullopt by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(testing_argument_get_help(argument));
}

TEST_CASE_FIXTURE(argument_test_fixture, "help() should return message if one has been provided") {
    auto sut = prepare_argument(long_name);

    constexpr std::string_view help_msg = "test help msg";
    argument.help(help_msg);

    const auto returned_help_msg = testing_argument_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
}

TEST_CASE_FIXTURE(argument_test_fixture, "defaul_value() should return nullopt by default") {
    const auto sut = prepare_argument(long_name);

    REQUIRE_FALSE(testing_argument_get_default_value(argument).has_value());
}

TEST_CASE_FIXTURE(argument_test_fixture, "defaul_value() should return value if one has been provided") {
    auto sut = prepare_argument(long_name);

    test_value_type default_value{};
    argument.default_value(default_value);

    const auto returned_default_value = testing_argument_get_default_value(argument);

    REQUIRE(returned_default_value.has_value());
    REQUIRE_EQ(std::any_cast<test_value_type>(returned_default_value), default_value);
}

// testing for setter functions for optional argument

// TEST_CASE_FIXTURE(argument_test_fixture, "value(const value_type&) should set value and return the argument
// "
//           "instance") {
//     auto sut = prepare_argument(long_name);

//     test_value_type value{};

//     const auto returned_argument = testing_argument_set_value(argument,
//     value);

//     REQUIRE(sut_has_value(sut));
//     REQUIRE_EQ(std::any_cast<test_value_type>(sut_value(sut)),
//     value); REQUIRE_EQ(returned_argument, argument);
// }

TEST_CASE_FIXTURE(argument_test_fixture, "required(bool) should set required attribute and return the "
          "argument") {
    auto sut = prepare_argument(long_name);
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);

    const auto& returned_argument = argument.required(required);

    REQUIRE_EQ(testing_argument_is_required(argument), required);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE_FIXTURE(argument_test_fixture, "help(string_view) should set help message and return the argument") {
    auto sut = prepare_argument(long_name);

    constexpr std::string_view help_msg = "test help msg";

    const auto& returned_argument = argument.help(help_msg);

    const auto returned_help_msg = testing_argument_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE_FIXTURE(argument_test_fixture, "default_value(value_type) should set default value and return the "
          "argument") {
    auto sut = prepare_argument(long_name);

    test_value_type default_value{};

    const auto& returned_argument = argument.default_value(default_value);

    const auto returned_default_value = testing_argument_get_default_value(argument);

    REQUIRE(returned_default_value.has_value());
    REQUIRE_EQ(std::any_cast<test_value_type>(returned_default_value), default_value);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_SUITE_END();
