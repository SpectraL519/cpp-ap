#define AP_TESTING

#include "../doctest.h"
#include "../testing_friend_definitions.hpp"

#include <ap/argument_parser.hpp>

#include <string_view>

using namespace ap::detail;


namespace {

using test_value_type = int;

constexpr std::string_view long_name = "test";
constexpr std::string_view short_name = "t";


optional_argument<test_value_type> default_optional_argument_long_name() {
    return optional_argument<test_value_type>(long_name);
}

optional_argument<test_value_type> default_optional_argument_both_names() {
    return optional_argument<test_value_type>(long_name, short_name);
}

} // namespace



TEST_CASE("optional argument with only long name should be optional and not "
          "positional") {
    const auto argument = default_optional_argument_long_name();

    REQUIRE(testing_is_optional(argument));
    REQUIRE_FALSE(testing_is_positional(argument));
}

TEST_CASE("optional argument with both long and short name should be optional "
          "and not positional") {
    const auto argument = default_optional_argument_both_names();

    REQUIRE(testing_is_optional(argument));
    REQUIRE_FALSE(testing_is_positional(argument));
}

TEST_CASE("has_value() should return false by default for long name") {
    const auto argument = default_optional_argument_long_name();

    REQUIRE_FALSE(testing_has_value(argument));
}

TEST_CASE("has_value() should return false by default for both long nad short "
          "names") {
    const auto argument = default_optional_argument_both_names();

    REQUIRE_FALSE(testing_has_value(argument));
}

TEST_CASE("has_value() should return true is value is set for long name") {
    auto argument = default_optional_argument_long_name();

    test_value_type value{};
    testing_set_value(argument, value);

    REQUIRE(testing_has_value(argument));
}

TEST_CASE("has_value() should return true is value is set for both long and "
          "short names") {
    auto argument = default_optional_argument_both_names();

    test_value_type value{};
    testing_set_value(argument, value);

    REQUIRE(testing_has_value(argument));
}

TEST_CASE("value() should throw if argument's value has not been set for long "
          "name") {
    auto argument = default_optional_argument_long_name();

    REQUIRE_FALSE(testing_has_value(argument));
    REQUIRE_THROWS_AS(testing_get_value(argument), std::bad_optional_access);
}

TEST_CASE("value() should throw if argument's value has not been set for both "
          "long and short names") {
    auto argument = default_optional_argument_both_names();

    REQUIRE_FALSE(testing_has_value(argument));
    REQUIRE_THROWS_AS(testing_get_value(argument), std::bad_optional_access);
}

TEST_CASE("value() should return the argument's value if it has been set for "
          "long name") {
    auto argument = default_optional_argument_long_name();

    test_value_type value{};
    testing_set_value(argument, value);

    REQUIRE(testing_has_value(argument));
    REQUIRE_EQ(testing_get_value(argument), value);
}

TEST_CASE("value() should return the argument's value if it has been set for "
          "both long and short names") {
    auto argument = default_optional_argument_both_names();

    test_value_type value{};
    testing_set_value(argument, value);

    REQUIRE(testing_has_value(argument));
    REQUIRE_EQ(testing_get_value(argument), value);
}

TEST_CASE("name() should return value passed to the optional argument "
          "constructor for long name") {
    const auto argument = default_optional_argument_long_name();

    REQUIRE_EQ(testing_get_name(argument), long_name);
}

TEST_CASE("name() and short_name() should return value passed to the optional "
          "argument "
          "constructor for both long and short names") {
    const auto argument = default_optional_argument_both_names();

    REQUIRE_EQ(testing_get_name(argument), long_name);
    REQUIRE_EQ(testing_get_short_name(argument), short_name);
}

TEST_CASE("required() should return false by default for long name") {
    auto argument = default_optional_argument_long_name();

    REQUIRE_FALSE(testing_is_required(argument));
}

TEST_CASE("required() should return false by default for both long and short "
          "names") {
    auto argument = default_optional_argument_both_names();

    REQUIRE_FALSE(testing_is_required(argument));
}

TEST_CASE("required() should return the value it has been set to for long name") {
    auto argument = default_optional_argument_long_name();
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);

    argument.required(required);

    REQUIRE_EQ(testing_is_required(argument), required);
}

TEST_CASE("required() should return the value it has been set to for both long "
          "and short names") {
    auto argument = default_optional_argument_both_names();
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);

    argument.required(required);

    REQUIRE_EQ(testing_is_required(argument), required);
}

TEST_CASE("help() should return nullopt by default for long name") {
    const auto argument = default_optional_argument_long_name();

    REQUIRE_FALSE(testing_get_help(argument));
}

TEST_CASE("help() should return nullopt by default for both long and short "
          "names") {
    const auto argument = default_optional_argument_both_names();

    REQUIRE_FALSE(testing_get_help(argument));
}

TEST_CASE("help() should return message if one has been provided for long name") {
    auto argument = default_optional_argument_long_name();

    constexpr std::string_view help_msg = "test help msg";
    argument.help(help_msg);

    const auto returned_help_msg = testing_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
}

TEST_CASE("help() should return message if one has been provided for both long "
          "and short names") {
    auto argument = default_optional_argument_both_names();

    constexpr std::string_view help_msg = "test help msg";
    argument.help(help_msg);

    const auto returned_help_msg = testing_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
}

TEST_CASE("defaul_value() should return nullopt by default for long name") {
    const auto argument = default_optional_argument_long_name();

    REQUIRE_FALSE(testing_get_default_value(argument));
}
TEST_CASE("defaul_value() should return nullopt by default for both long nad "
          "short names") {
    const auto argument = default_optional_argument_both_names();

    REQUIRE_FALSE(testing_get_default_value(argument));
}

TEST_CASE("defaul_value() should return value if one has been provided for "
          "long name") {
    auto argument = default_optional_argument_long_name();

    test_value_type default_value{};
    argument.default_value(default_value);

    const auto returned_default_value = testing_get_default_value(argument);

    REQUIRE(returned_default_value);
    REQUIRE_EQ(returned_default_value, default_value);
}

TEST_CASE("defaul_value() should return value if one has been provided for "
          "both long and short names") {
    auto argument = default_optional_argument_both_names();

    test_value_type default_value{};
    argument.default_value(default_value);

    const auto returned_default_value = testing_get_default_value(argument);

    REQUIRE(returned_default_value);
    REQUIRE_EQ(returned_default_value, default_value);
}

// testing for setter functions for optional argument

TEST_CASE("value(const value_type&) should set value and return the argument "
          "instance for long name") {
    auto argument = default_optional_argument_long_name();

    test_value_type value{};

    const auto returned_argument = testing_set_value(argument, value);

    REQUIRE(testing_has_value(argument));
    REQUIRE_EQ(testing_get_value(argument), value);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("value(const value_type&) should set value and return the argument "
          "instance for both long and short names") {
    auto argument = default_optional_argument_both_names();

    test_value_type value{};

    const auto returned_argument = testing_set_value(argument, value);

    REQUIRE(testing_has_value(argument));
    REQUIRE_EQ(testing_get_value(argument), value);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("required(bool) should set required attribute and return the "
          "argument for long name") {
    auto argument = default_optional_argument_long_name();
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);

    const auto returned_argument = argument.required(required);

    REQUIRE_EQ(testing_is_required(argument), required);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("required(bool) should set required attribute and return the "
          "argument for both long and short name") {
    auto argument = default_optional_argument_both_names();
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);

    const auto returned_argument = argument.required(required);

    REQUIRE_EQ(testing_is_required(argument), required);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("help(string_view) should set help message and return the argument "
          "for long name") {
    auto argument = default_optional_argument_long_name();

    constexpr std::string_view help_msg = "test help msg";

    const auto returned_argument = argument.help(help_msg);

    const auto returned_help_msg = testing_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("help(string_view) should set help message and return the argument "
          "for both long and short names") {
    auto argument = default_optional_argument_both_names();

    constexpr std::string_view help_msg = "test help msg";

    const auto returned_argument = argument.help(help_msg);

    const auto returned_help_msg = testing_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("default_value(value_type) should set default value and return the "
          "argument for long name") {
    auto argument = default_optional_argument_long_name();

    test_value_type default_value{};

    const auto returned_argument = argument.default_value(default_value);

    const auto returned_default_value = testing_get_default_value(argument);

    REQUIRE(returned_default_value);
    REQUIRE_EQ(returned_default_value, default_value);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("default_value(value_type) should set default value and return the "
          "argument for both long and short names") {
    auto argument = default_optional_argument_both_names();

    test_value_type default_value{};

    const auto returned_argument = argument.default_value(default_value);

    const auto returned_default_value = testing_get_default_value(argument);

    REQUIRE(returned_default_value);
    REQUIRE_EQ(returned_default_value, default_value);
    REQUIRE_EQ(returned_argument, argument);
}
