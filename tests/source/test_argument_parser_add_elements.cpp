#include "argument_parser_test_fixture.hpp"
#include "argument_test_fixture.hpp"
#include "doctest.h"
#include "utility.hpp"

using namespace ap_testing;
using ap::argument;
using ap::argument_parser;
using ap::default_argument;
using ap::invalid_configuration;

struct test_argument_parser_add_elements : public argument_parser_test_fixture {
    const char flag_char = '-';

    const std::string_view primary_name_1 = "primary_name_1";
    const std::string_view secondary_name_1 = "s1";

    const std::optional<std::string> primary_name_1_opt =
        std::make_optional<std::string>(primary_name_1);
    const std::optional<std::string> secondary_name_1_opt =
        std::make_optional<std::string>(secondary_name_1);

    const std::string_view primary_name_2 = "primary_name_2";
    const std::string_view secondary_name_2 = "s2";

    const std::optional<std::string> primary_name_2_opt =
        std::make_optional<std::string>(primary_name_2);
    const std::optional<std::string> secondary_name_2_opt =
        std::make_optional<std::string>(secondary_name_2);

    const std::string_view invalid_name_empty = "";
    const std::string_view invalid_name_whitespace = "invalid name";
    const std::string_view invalid_name_flag_prefix = "-invalid";
    const std::string_view invalid_name_digit = "1invalid";
};

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_{positional,optional}_argument(primary) should throw if the passed argument name is "
    "invalid"
) {
    std::string primary_name, reason;

    SUBCASE("The name is empty") {
        primary_name = invalid_name_empty;
        reason = "An argument name cannot be empty.";
    }

    SUBCASE("The name contains whitespace characters") {
        primary_name = invalid_name_whitespace;
        reason = "An argument name cannot contain whitespaces.";
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
        invalid_configuration::invalid_argument_name(primary_name, reason).what(),
        invalid_configuration
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(primary_name),
        invalid_configuration::invalid_argument_name(primary_name, reason).what(),
        invalid_configuration
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_{positional,optional}_argument(primary, secondary) should throw if the primary name is "
    "invalid"
) {
    std::string primary_name, reason;

    SUBCASE("The name is empty") {
        primary_name = invalid_name_empty;
        reason = "An argument name cannot be empty.";
    }

    SUBCASE("The name contains whitespace characters") {
        primary_name = invalid_name_whitespace;
        reason = "An argument name cannot contain whitespaces.";
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
        invalid_configuration::invalid_argument_name(primary_name, reason).what(),
        invalid_configuration
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(primary_name),
        invalid_configuration::invalid_argument_name(primary_name, reason).what(),
        invalid_configuration
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_optional_argument(primary, secondary) should throw if the secondary name is "
    "invalid"
) {
    std::string secondary_name, reason;

    SUBCASE("The name is empty") {
        secondary_name = invalid_name_empty;
        reason = "An argument name cannot be empty.";
    }

    SUBCASE("The name contains whitespace characters") {
        secondary_name = invalid_name_whitespace;
        reason = "An argument name cannot contain whitespaces.";
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
        sut.add_optional_argument(primary_name_1, secondary_name),
        invalid_configuration::invalid_argument_name(secondary_name, reason).what(),
        invalid_configuration
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_positional_argument should throw when adding an argument with a previously used name"
) {
    sut.add_positional_argument(primary_name_1);

    // adding argument with a unique name
    CHECK_NOTHROW(sut.add_positional_argument(primary_name_2));

    // adding argument with a previously used name
    CHECK_THROWS_WITH_AS(
        sut.add_positional_argument(primary_name_1),
        invalid_configuration::argument_name_used({primary_name_1_opt}).what(),
        invalid_configuration
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_optional_argument should throw when adding an argument with a previously used name"
) {
    sut.add_optional_argument(primary_name_1, secondary_name_1);

    SUBCASE("adding argument with a unique name") {
        CHECK_NOTHROW(sut.add_optional_argument(primary_name_2, secondary_name_2));
    }

    SUBCASE("adding argument with a previously used primary name") {
        CHECK_THROWS_WITH_AS(
            sut.add_optional_argument(primary_name_1, secondary_name_2),
            invalid_configuration::argument_name_used(
                {primary_name_1_opt, secondary_name_2_opt, flag_char}
            )
                .what(),
            invalid_configuration
        );
    }

    SUBCASE("adding argument with a previously used secondary name") {
        CHECK_THROWS_WITH_AS(
            sut.add_optional_argument(primary_name_2, secondary_name_1),
            invalid_configuration::argument_name_used(
                {primary_name_2_opt, secondary_name_1_opt, flag_char}
            )
                .what(),
            invalid_configuration
        );
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_flag should return an optional argument reference with flag parameters"
) {
    const argument_test_fixture arg_fixture;

    SUBCASE("StoreImplicitly = true") {
        auto& argument = sut.add_flag(primary_name_1, secondary_name_1);

        REQUIRE(is_optional<bool>(argument));
        CHECK_FALSE(sut.value<bool>(primary_name_1));

        arg_fixture.mark_used(argument);
        CHECK(sut.value<bool>(primary_name_1));
    }

    SUBCASE("StoreImplicitly = false") {
        auto& argument = sut.add_flag<false>(primary_name_1, secondary_name_1);

        REQUIRE(is_optional<bool>(argument));
        CHECK(sut.value<bool>(primary_name_1));

        arg_fixture.mark_used(argument);
        CHECK_FALSE(sut.value<bool>(primary_name_1));
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_flag should throw when adding and argument with a previously used name"
) {
    sut.add_flag(primary_name_1, secondary_name_1);

    SUBCASE("adding argument with a unique name") {
        CHECK_NOTHROW(sut.add_flag(primary_name_2, secondary_name_2));
    }

    SUBCASE("adding argument with a previously used primary name") {
        CHECK_THROWS_WITH_AS(
            sut.add_flag(primary_name_1, secondary_name_2),
            invalid_configuration::argument_name_used(
                {primary_name_1_opt, secondary_name_2_opt, flag_char}
            )
                .what(),
            invalid_configuration
        );
    }

    SUBCASE("adding argument with a previously used secondary name") {
        CHECK_THROWS_WITH_AS(
            sut.add_flag(primary_name_2, secondary_name_1),
            invalid_configuration::argument_name_used(
                {primary_name_2_opt, secondary_name_1_opt, flag_char}
            )
                .what(),
            invalid_configuration
        );
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "default_arguments should add the specified positional arguments"
) {
    sut.default_arguments({default_argument::p_input, default_argument::p_output});

    const auto input_arg = get_argument("input");
    REQUIRE(input_arg);
    CHECK(is_positional<std::string>(*input_arg));

    const auto output_arg = get_argument("output");
    REQUIRE(output_arg);
    CHECK(is_positional<std::string>(*output_arg));
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "default_arguments should add the specified optional arguments"
) {
    sut.default_arguments(
        default_argument::o_help, default_argument::o_input, default_argument::o_output
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
    CHECK(is_optional<ap::none_type>(*help_arg));

    const auto input_arg = get_argument(input_flag);
    REQUIRE(input_arg);
    CHECK(is_optional<std::string>(*input_arg));


    const auto output_arg = get_argument(output_flag);
    REQUIRE(output_arg);
    CHECK(is_optional<std::string>(*output_arg));
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "argument adding functions should throw if a group does not belong to the parser"
) {
    argument_parser different_parser("different-program");

    const std::string group_name = "Group From a Different Parser";
    auto& group = different_parser.add_group(group_name);

    const std::string expected_err_msg =
        std::format("An argument group '{}' does not belong to the given parser.", group_name);

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(group, primary_name_1), expected_err_msg.c_str(), std::logic_error
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(group, primary_name_1), expected_err_msg.c_str(), std::logic_error
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(group, secondary_name_1, ap::n_secondary),
        expected_err_msg.c_str(),
        std::logic_error
    );

    CHECK_THROWS_WITH_AS(
        sut.add_optional_argument(group, primary_name_1, secondary_name_1),
        expected_err_msg.c_str(),
        std::logic_error
    );

    CHECK_THROWS_WITH_AS(
        sut.add_flag(group, primary_name_1), expected_err_msg.c_str(), std::logic_error
    );

    CHECK_THROWS_WITH_AS(
        sut.add_flag(group, secondary_name_1, ap::n_secondary),
        expected_err_msg.c_str(),
        std::logic_error
    );

    CHECK_THROWS_WITH_AS(
        sut.add_flag(group, primary_name_1, secondary_name_1),
        expected_err_msg.c_str(),
        std::logic_error
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_add_elements,
    "add_subparser should throw if a subparser with the given name already exists"
) {
    constexpr std::string_view subparser_name = "subprogram";

    sut.add_subparser(subparser_name);

    CHECK_THROWS_WITH_AS(
        sut.add_subparser(subparser_name),
        std::format(
            "A subparser with the given name () already exists in parser '{}'",
            subparser_name,
            sut.name()
        )
            .c_str(),
        std::logic_error
    );
}
