#define AP_TESTING

#include "argument_parser_test_fixture.hpp"
#include "doctest.h"
#include "optional_argument_test_fixture.hpp"

using namespace ap_testing;
using namespace ap::argument;

TEST_SUITE_BEGIN("test_argument_parser_add_argument");

struct test_argument_parser_add_argument : public argument_parser_test_fixture {
    const std::string_view primary_name_1 = "primary_name_1";
    const std::string_view secondary_name_1 = "s1";

    const std::string_view primary_name_2 = "primary_name_2";
    const std::string_view secondary_name_2 = "s2";
};

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "default_positional_arguments should add the specified arguments"
) {
    sut.default_positional_arguments({default_positional::input, default_positional::output});

    const auto input_arg = get_argument("input");
    REQUIRE(input_arg);
    REQUIRE_FALSE(input_arg->get().is_optional());

    const auto output_arg = get_argument("output");
    REQUIRE(output_arg);
    REQUIRE_FALSE(output_arg->get().is_optional());
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "default_optional_arguments should add the specified arguments"
) {
    sut.default_optional_arguments(
        {default_optional::help, default_optional::input, default_optional::output}
    );

    std::string help_flag;
    std::string input_flag;
    std::string output_flag;

    SUBCASE("using primary flags") {
        help_flag = "help";
        input_flag = "input";
        output_flag = "output";
    }

    SUBCASE("using secondary flags") {
        help_flag = "h";
        input_flag = "i";
        output_flag = "o";
    }

    CAPTURE(help_flag);
    CAPTURE(input_flag);
    CAPTURE(output_flag);

    const auto help_arg = get_argument(help_flag);
    REQUIRE(help_arg);
    CHECK(help_arg->get().is_optional());

    const auto input_arg = get_argument(input_flag);
    REQUIRE(input_arg);
    CHECK(input_arg->get().is_optional());

    const auto output_arg = get_argument(output_flag);
    REQUIRE(output_arg);
    CHECK(output_arg->get().is_optional());
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_positional_argument should return a positional argument reference"
) {
    const auto& argument = sut.add_positional_argument(primary_name_1, secondary_name_1);
    CHECK_FALSE(argument.is_optional());
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_positional_argument should throw only when adding an"
    "argument with a previously used name"
) {
    sut.add_positional_argument(primary_name_1, secondary_name_1);

    SUBCASE("adding argument with a unique name") {
        CHECK_NOTHROW(sut.add_positional_argument(primary_name_2, secondary_name_2));
    }

    SUBCASE("adding argument with a previously used primary name") {
        CHECK_THROWS_AS(
            sut.add_positional_argument(primary_name_1, secondary_name_2),
            ap::error::argument_name_used
        );
    }

    SUBCASE("adding argument with a previously used secondary name") {
        CHECK_THROWS_AS(
            sut.add_positional_argument(primary_name_2, secondary_name_1),
            ap::error::argument_name_used
        );
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_optional_argument should return an optional argument reference"
) {
    const auto& argument = sut.add_optional_argument(primary_name_1, secondary_name_1);
    CHECK(argument.is_optional());
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_optional_argument should throw only when adding an"
    "argument with a previously used name"
) {
    sut.add_optional_argument(primary_name_1, secondary_name_1);

    SUBCASE("adding argument with a unique name") {
        CHECK_NOTHROW(sut.add_optional_argument(primary_name_2, secondary_name_2));
    }

    SUBCASE("adding argument with a previously used primary name") {
        CHECK_THROWS_AS(
            sut.add_optional_argument(primary_name_1, secondary_name_2),
            ap::error::argument_name_used
        );
    }

    SUBCASE("adding argument with a previously used secondary name") {
        CHECK_THROWS_AS(
            sut.add_optional_argument(primary_name_2, secondary_name_1),
            ap::error::argument_name_used
        );
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_flag should return an optional argument reference with flag parameters"
) {
    const optional_argument_test_fixture opt_arg_fixture;

    SUBCASE("StoreImplicitly = true") {
        auto& argument = sut.add_flag(primary_name_1, secondary_name_1);

        REQUIRE(argument.is_optional());
        CHECK_FALSE(sut.value<bool>(primary_name_1));

        opt_arg_fixture.mark_used(argument);
        CHECK(sut.value<bool>(primary_name_1));
    }

    SUBCASE("StoreImplicitly = false") {
        auto& argument = sut.add_flag<false>(primary_name_1, secondary_name_1);

        REQUIRE(argument.is_optional());
        CHECK(sut.value<bool>(primary_name_1));

        opt_arg_fixture.mark_used(argument);
        CHECK_FALSE(sut.value<bool>(primary_name_1));
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_flag should throw only when adding and argument with a previously used name"
) {
    sut.add_flag(primary_name_1, secondary_name_1);

    SUBCASE("adding argument with a unique name") {
        CHECK_NOTHROW(sut.add_flag(primary_name_2, secondary_name_2));
    }

    SUBCASE("adding argument with a previously used primary name") {
        CHECK_THROWS_AS(
            sut.add_flag(primary_name_1, secondary_name_2), ap::error::argument_name_used
        );
    }

    SUBCASE("adding argument with a previously used secondary name") {
        CHECK_THROWS_AS(
            sut.add_flag(primary_name_2, secondary_name_1), ap::error::argument_name_used
        );
    }
}

TEST_SUITE_END();
