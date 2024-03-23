#define AP_TESTING

#include "doctest.h"
#include "argument_parser_test_fixture.hpp"
#include "optional_argument_test_fixture.hpp"

#include <ap/argument_parser.hpp>

#include <iostream>

using namespace ap::argument;


namespace {

constexpr std::string_view name = "test";
constexpr std::string_view short_name = "t";

constexpr std::string_view other_name = "other";
constexpr std::string_view other_short_name = "o";

} // namespace


namespace ap_testing {

TEST_SUITE_BEGIN("test_argument_parser_add_argument");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "default_positional_arguments should add the specified arguments"
) {
    sut.default_positional_arguments({
        ap::default_argument::positional::input,
        ap::default_argument::positional::output
    });

    REQUIRE_FALSE(sut_get_argument("input")->get().is_optional());
    REQUIRE_FALSE(sut_get_argument("output")->get().is_optional());
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "default_optional_arguments should add the specified arguments"
) {
    sut.default_optional_arguments({
        ap::default_argument::optional::help,
        ap::default_argument::optional::input,
        ap::default_argument::optional::output
    });

    REQUIRE(sut_get_argument("help")->get().is_optional());
    REQUIRE(sut_get_argument("h")->get().is_optional());

    REQUIRE(sut_get_argument("input")->get().is_optional());
    REQUIRE(sut_get_argument("i")->get().is_optional());

    REQUIRE(sut_get_argument("output")->get().is_optional());
    REQUIRE(sut_get_argument("o")->get().is_optional());
}


TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_positional_argument should return a positional argument reference"
) {
    const auto& argument = sut.add_positional_argument(name, short_name);
    REQUIRE_FALSE(argument.is_optional());
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_positional_argument should throw only when adding an"
    "argument with a previously used name"
) {
    sut.add_positional_argument(name, short_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_positional_argument(other_name, other_short_name));
    }

    SUBCASE("adding argument with a previously used long name") {
        REQUIRE_THROWS_AS(
            sut.add_positional_argument(name, other_short_name),
            ap::error::argument_name_used_error);
    }

    SUBCASE("adding argument with a previously used short name") {
        REQUIRE_THROWS_AS(
            sut.add_positional_argument(other_name, short_name),
            ap::error::argument_name_used_error);
    }
}


TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_optional_argument should return an optional argument reference"
) {
    const auto& argument = sut.add_optional_argument(name, short_name);
    REQUIRE(argument.is_optional());
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_optional_argument should throw only when adding an"
    "argument with a previously used name"
) {
    sut.add_optional_argument(name, short_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_optional_argument(other_name, other_short_name));
    }

    SUBCASE("adding argument with a previously used long name") {
        REQUIRE_THROWS_AS(
            sut.add_optional_argument(name, other_short_name),
            ap::error::argument_name_used_error);
    }

    SUBCASE("adding argument with a previously used short name") {
        REQUIRE_THROWS_AS(
            sut.add_optional_argument(other_name, short_name),
            ap::error::argument_name_used_error);
    }
}


TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_flag should return an optional argument reference with flag parameters"
) {
    const optional_argument_test_fixture opt_arg_fixture;

    SUBCASE("StoreImplicitly = true") {
        auto& argument = sut.add_flag(name, short_name);

        REQUIRE(argument.is_optional());
        REQUIRE_FALSE(sut.value<bool>(name));

        opt_arg_fixture.sut_set_used(argument);
        REQUIRE(sut.value<bool>(name));
    }

    SUBCASE("StoreImplicitly = false") {
        auto& argument = sut.add_flag<false>(name, short_name);

        REQUIRE(argument.is_optional());
        REQUIRE(sut.value<bool>(name));

        opt_arg_fixture.sut_set_used(argument);
        REQUIRE_FALSE(sut.value<bool>(name));
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_flag should throw only when adding and argument with a previously used name"
) {
    sut.add_flag(name, short_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_flag(other_name, other_short_name));
    }

    SUBCASE("adding argument with a previously used long name") {
        REQUIRE_THROWS_AS(
            sut.add_flag(name, other_short_name),
            ap::error::argument_name_used_error);
    }

    SUBCASE("adding argument with a previously used short name") {
        REQUIRE_THROWS_AS(
            sut.add_flag(other_name, short_name),
            ap::error::argument_name_used_error);
    }
}

TEST_SUITE_END();

} // namespace ap_testing
