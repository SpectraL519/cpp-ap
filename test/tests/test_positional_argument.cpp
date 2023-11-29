#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <testing_friend_definitions.hpp>

#include <string_view>

using namespace ap::detail;



namespace {

using test_value_type = int;

constexpr std::string_view long_name = "test";
constexpr std::string_view short_name = "t";


positional_argument<test_value_type> default_positional_argument_long_name() {
    return positional_argument<test_value_type>(long_name);
}

positional_argument<test_value_type> default_positional_argument_both_names() {
    return positional_argument<test_value_type>(long_name, short_name);
}

} // namespace



TEST_SUITE_BEGIN("test_positional_argument");

// TEST_CASE("positonal argument should be positional and not optional") {
//     const auto argument = default_positional_argument_long_name();

//     REQUIRE(is_positional<decltype(argument)>());
//     REQUIRE_FALSE(is_optional<decltype(argument)>());
// }

TEST_CASE("has_value() should return false by default") {
    const auto argument = default_positional_argument_long_name();

    REQUIRE_FALSE(testing_argument_has_value(argument));
}

// TEST_CASE("has_value() should return true is value is set") {
//     auto argument = default_positional_argument_long_name();

//     test_value_type value{};
//     testing_argument_set_value(argument, value);

//     REQUIRE(testing_argument_has_value(argument));
// }

TEST_CASE("value() should return default any object if argument's value has "
          "not been set") {
    auto argument = default_positional_argument_long_name();

    REQUIRE_FALSE(testing_argument_has_value(argument));
    REQUIRE_THROWS_AS(
        std::any_cast<test_value_type>(testing_argument_get_value(argument)), std::bad_any_cast
    );
}

// TEST_CASE("value() should return the argument's value if it has been set") {
//     auto argument = default_positional_argument_long_name();

//     test_value_type value{};
//     testing_argument_set_value(argument, value);

//     REQUIRE(testing_argument_has_value(argument));
//     REQUIRE_EQ(std::any_cast<test_value_type>(testing_argument_get_value(argument)), value);
// }

TEST_CASE("name() should return value passed to the optional argument "
          "constructor for long name") {
    const auto argument = default_positional_argument_long_name();

    const auto name = testing_argument_get_name(argument);

    REQUIRE_EQ(name, long_name);
    REQUIRE_NE(name, short_name);
}

TEST_CASE("name() and short_name() should return value passed to the optional "
          "argument "
          "constructor for both long and short names") {
    const auto argument = default_positional_argument_both_names();

    const auto name = testing_argument_get_name(argument);

    REQUIRE_EQ(name, long_name);
    REQUIRE_EQ(name, short_name);
}


TEST_CASE("required() should return true by default") {
    auto argument = default_positional_argument_long_name();

    REQUIRE(testing_argument_is_required(argument));
}

TEST_CASE("required() should return the value it has been set to") {
    auto argument = default_positional_argument_long_name();
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

TEST_CASE("help() should return nullopt by default") {
    const auto argument = default_positional_argument_long_name();

    REQUIRE_FALSE(testing_argument_get_help(argument));
}

TEST_CASE("help() should return message if one has been provided") {
    auto argument = default_positional_argument_long_name();

    constexpr std::string_view help_msg = "test help msg";
    argument.help(help_msg);

    const auto returned_help_msg = testing_argument_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
}

TEST_CASE("defaul_value() should return nullopt by default") {
    const auto argument = default_positional_argument_long_name();

    const auto& arg_default_value = testing_argument_get_default_value(argument);

    REQUIRE_FALSE(arg_default_value.has_value());
}

TEST_CASE("defaul_value() should return value if one has been provided") {
    auto argument = default_positional_argument_long_name();

    test_value_type default_value{};
    argument.default_value(default_value);

    const auto& arg_default_value = testing_argument_get_default_value(argument);

    REQUIRE(arg_default_value.has_value());
    REQUIRE_EQ(std::any_cast<test_value_type>(arg_default_value), default_value);
}


// positional_argument class' setter functions

// TEST_CASE("value(const any&) should set value and return the argument "
//           "instance") {
//     auto argument = default_positional_argument_long_name();

//     test_value_type value{};

//     const auto returned_argument = testing_argument_set_value(argument, value);

//     REQUIRE(testing_argument_has_value(argument));

//     const auto& arg_value = testing_argument_get_value(argument);

//     REQUIRE_EQ(std::any_cast<test_value_type>(arg_value), value);
//     REQUIRE_EQ(returned_argument, argument);
// }

TEST_CASE("required(bool) should set required attribute and return the argument") {
    auto argument = default_positional_argument_long_name();
    bool required;

    SUBCASE("set to true") {
        required = true;
    }
    SUBCASE("set to false") {
        required = false;
    }

    CAPTURE(required);

    const auto returned_argument = argument.required(required);

    REQUIRE_EQ(testing_argument_is_required(argument), required);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("help(string_view) should set help message and return the argument") {
    auto argument = default_positional_argument_long_name();

    constexpr std::string_view help_msg = "test help msg";

    const auto returned_argument = argument.help(help_msg);

    const auto returned_help_msg = testing_argument_get_help(argument);

    REQUIRE(returned_help_msg);
    REQUIRE_EQ(returned_help_msg, help_msg);
    REQUIRE_EQ(returned_argument, argument);
}

TEST_CASE("default_value(const any&) should set help message and return the "
          "argument") {
    auto argument = default_positional_argument_long_name();

    test_value_type default_value{};

    const auto returned_argument = argument.default_value(default_value);

    const auto& arg_default_value = testing_argument_get_default_value(argument);

    REQUIRE(arg_default_value.has_value());
    REQUIRE_EQ(std::any_cast<test_value_type>(arg_default_value), default_value);
}

TEST_SUITE_END();
