#define AP_TESTING

#include "argument_parser_test_fixture.hpp"
#include "doctest.h"
#include "optional_argument_test_fixture.hpp"

#include <ap/argument_parser.hpp>

#include <iostream>

using namespace ap_testing;
using namespace ap::argument;

namespace {

constexpr std::string_view primary_name = "test";
constexpr std::string_view secondary_name = "t";

constexpr std::string_view other_primary_name = "other";
constexpr std::string_view other_secondary_name = "o";

} // namespace

TEST_SUITE_BEGIN("test_argument_parser_add_argument");

TEST_CASE_FIXTURE(argument_parser_test_fixture, "default_positional_arguments should add the specified arguments") {
    sut.default_positional_arguments(
        { ap::default_argument::positional::input, ap::default_argument::positional::output }
    );

    const auto input_arg = sut_get_argument("input");
    REQUIRE(input_arg);
    REQUIRE_FALSE(input_arg->get().is_optional());

    const auto output_arg = sut_get_argument("output");
    REQUIRE(output_arg);
    REQUIRE_FALSE(output_arg->get().is_optional());
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "default_optional_arguments should add the specified arguments") {
    sut.default_optional_arguments(
        { ap::default_argument::optional::help,
          ap::default_argument::optional::input,
          ap::default_argument::optional::output }
    );

    const auto help_arg = sut_get_argument("help");
    REQUIRE(help_arg);
    REQUIRE(help_arg->get().is_optional());
    // TODO: secondary flag

    const auto input_arg = sut_get_argument("help");
    REQUIRE(input_arg);
    REQUIRE(input_arg->get().is_optional());
    // TODO: secondary flag

    const auto output_arg = sut_get_argument("output");
    REQUIRE(output_arg);
    REQUIRE_FALSE(output_arg->get().is_optional());
    // TODO: secondary flag
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "add_positional_argument should return a positional argument reference") {
    const auto& argument = sut.add_positional_argument(primary_name, secondary_name);
    REQUIRE_FALSE(argument.is_optional());
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_positional_argument should throw only when adding an"
    "argument with a previously used name"
) {
    sut.add_positional_argument(primary_name, secondary_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_positional_argument(other_primary_name, other_secondary_name));
    }

    SUBCASE("adding argument with a previously used primary name") {
        REQUIRE_THROWS_AS(sut.add_positional_argument(primary_name, other_secondary_name), ap::error::argument_name_used_error);
    }

    SUBCASE("adding argument with a previously used secondary name") {
        REQUIRE_THROWS_AS(sut.add_positional_argument(other_primary_name, secondary_name), ap::error::argument_name_used_error);
    }
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "add_optional_argument should return an optional argument reference") {
    const auto& argument = sut.add_optional_argument(primary_name, secondary_name);
    REQUIRE(argument.is_optional());
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "add_optional_argument should throw only when adding an"
    "argument with a previously used name"
) {
    sut.add_optional_argument(primary_name, secondary_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_optional_argument(other_primary_name, other_secondary_name));
    }

    SUBCASE("adding argument with a previously used primary name") {
        REQUIRE_THROWS_AS(sut.add_optional_argument(primary_name, other_secondary_name), ap::error::argument_name_used_error);
    }

    SUBCASE("adding argument with a previously used secondary name") {
        REQUIRE_THROWS_AS(sut.add_optional_argument(other_primary_name, secondary_name), ap::error::argument_name_used_error);
    }
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "add_flag should return an optional argument reference with flag parameters") {
    const optional_argument_test_fixture opt_arg_fixture;

    SUBCASE("StoreImplicitly = true") {
        auto& argument = sut.add_flag(primary_name, secondary_name);

        REQUIRE(argument.is_optional());
        REQUIRE_FALSE(sut.value<bool>(primary_name));

        opt_arg_fixture.sut_set_used(argument);
        REQUIRE(sut.value<bool>(primary_name));
    }

    SUBCASE("StoreImplicitly = false") {
        auto& argument = sut.add_flag<false>(primary_name, secondary_name);

        REQUIRE(argument.is_optional());
        REQUIRE(sut.value<bool>(primary_name));

        opt_arg_fixture.sut_set_used(argument);
        REQUIRE_FALSE(sut.value<bool>(primary_name));
    }
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "add_flag should throw only when adding and argument with a previously used name") {
    sut.add_flag(primary_name, secondary_name);

    SUBCASE("adding argument with a unique name") {
        REQUIRE_NOTHROW(sut.add_flag(other_primary_name, other_secondary_name));
    }

    SUBCASE("adding argument with a previously used primary name") {
        REQUIRE_THROWS_AS(sut.add_flag(primary_name, other_secondary_name), ap::error::argument_name_used_error);
    }

    SUBCASE("adding argument with a previously used secondary name") {
        REQUIRE_THROWS_AS(sut.add_flag(other_primary_name, secondary_name), ap::error::argument_name_used_error);
    }
}

TEST_SUITE_END();
