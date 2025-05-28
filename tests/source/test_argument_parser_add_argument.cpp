#define AP_TESTING

#include "argument_parser_test_fixture.hpp"
#include "doctest.h"
#include "optional_argument_test_fixture.hpp"
#include "utility.hpp"

using namespace ap_testing;
using namespace ap::argument;

TEST_SUITE_BEGIN("test_argument_parser_add_argument");

struct test_argument_parser_add_argument : public argument_parser_test_fixture {
    const std::string_view primary_name_1 = "primary_name_1";
    const std::string_view secondary_name_1 = "s1";

    const std::string_view primary_name_2 = "primary_name_2";
    const std::string_view secondary_name_2 = "s2";

    const std::string_view invalid_name_empty = "";
    const std::string_view invalid_name_flag_prefix = "-invalid";
    const std::string_view invalid_name_digit = "1invalid";

    [[nodiscard]] auto invalid_name_pattern_err_msg(
        const std::string_view arg_name, const std::string_view reason
    ) const noexcept {
        return ap::error::invalid_argument_name_pattern(arg_name, reason).what();
    }
};

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_{positional,optional}_argument(primary) should throw if the passed argument name is "
    "invalid"
) {
    std::string primary_name, reason;

    SUBCASE("The name is empty") {
        primary_name = invalid_name_empty;
        reason = "An argument name cannot be empty.";
    }

    SUBCASE("The name begins with the flag prefix character") {
        primary_name = invalid_name_flag_prefix;
        reason = "An argument name cannot begin with a flag prefix character (-).";
    }

    SUBCASE("The name begins with a digit") {
        primary_name = invalid_name_digit;
        reason = "An argument name cannot begin with a digit.";
    }

    CAPTURE(primary_name);
    CAPTURE(reason);

    CHECK_THROWS_WITH_AS(
        sut.add_positional_argument(primary_name),
        invalid_name_pattern_err_msg(primary_name, reason),
        ap::error::invalid_argument_name_pattern
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(primary_name),
        invalid_name_pattern_err_msg(primary_name, reason),
        ap::error::invalid_argument_name_pattern
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_{positional,optional}_argument(primary, secondary) should throw if the primary name is "
    "invalid"
) {
    std::string primary_name, reason;

    SUBCASE("The name is empty") {
        primary_name = invalid_name_empty;
        reason = "An argument name cannot be empty.";
    }

    SUBCASE("The name begins with the flag prefix character") {
        primary_name = invalid_name_flag_prefix;
        reason = "An argument name cannot begin with a flag prefix character (-).";
    }

    SUBCASE("The name begins with a digit") {
        primary_name = invalid_name_digit;
        reason = "An argument name cannot begin with a digit.";
    }

    CAPTURE(primary_name);
    CAPTURE(reason);

    CHECK_THROWS_WITH_AS(
        sut.add_positional_argument(primary_name, secondary_name_1),
        invalid_name_pattern_err_msg(primary_name, reason),
        ap::error::invalid_argument_name_pattern
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(primary_name, secondary_name_1),
        invalid_name_pattern_err_msg(primary_name, reason),
        ap::error::invalid_argument_name_pattern
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "add_{positional,optional}_argument(primary, secondary) should throw if the secondary name is "
    "invalid"
) {
    std::string secondary_name, reason;

    SUBCASE("The name is empty") {
        secondary_name = invalid_name_empty;
        reason = "An argument name cannot be empty.";
    }

    SUBCASE("The name begins with the flag prefix character") {
        secondary_name = invalid_name_flag_prefix;
        reason = "An argument name cannot begin with a flag prefix character (-).";
    }

    SUBCASE("The name begins with a digit") {
        secondary_name = invalid_name_digit;
        reason = "An argument name cannot begin with a digit.";
    }

    CAPTURE(secondary_name);
    CAPTURE(reason);

    CHECK_THROWS_WITH_AS(
        sut.add_positional_argument(primary_name_1, secondary_name),
        invalid_name_pattern_err_msg(secondary_name, reason),
        ap::error::invalid_argument_name_pattern
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(primary_name_1, secondary_name),
        invalid_name_pattern_err_msg(secondary_name, reason),
        ap::error::invalid_argument_name_pattern
    );
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

        REQUIRE(is_optional<bool>(argument));
        CHECK_FALSE(sut.value<bool>(primary_name_1));

        opt_arg_fixture.mark_used(argument);
        CHECK(sut.value<bool>(primary_name_1));
    }

    SUBCASE("StoreImplicitly = false") {
        auto& argument = sut.add_flag<false>(primary_name_1, secondary_name_1);

        REQUIRE(is_optional<bool>(argument));
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

TEST_CASE_FIXTURE(
    test_argument_parser_add_argument,
    "default_positional_arguments should add the specified arguments"
) {
    sut.default_positional_arguments({default_positional::input, default_positional::output});

    const auto input_arg = get_argument("input");
    REQUIRE(input_arg);
    CHECK(is_positional<std::string>(input_arg.value()));

    const auto output_arg = get_argument("output");
    REQUIRE(output_arg);
    CHECK(is_positional<std::string>(output_arg.value()));
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
    CHECK(is_optional<bool>(help_arg.value()));

    const auto input_arg = get_argument(input_flag);
    REQUIRE(input_arg);
    CHECK(is_optional<std::string>(input_arg.value()));


    const auto output_arg = get_argument(output_flag);
    REQUIRE(output_arg);
    CHECK(is_optional<std::string>(output_arg.value()));
}

TEST_SUITE_END();
