#include "argument_parser_test_fixture.hpp"
#include "doctest.h"
#include "utility.hpp"

using namespace ap_testing;
using ap::parsing_failure;

struct test_argument_parser_parse_args_unknown_flags_as_values
: public argument_parser_test_fixture {
    const std::string test_program_name = "test program name";
    const std::string unknown_arg_flag = "--unknown-arg";

    const std::size_t no_args = 0ull;
};

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args_unknown_flags_as_values,
    "parse_args should treat an unknown argument flag as a positional value if it's not preceeded "
    "by any valid argument flags"
) {
    const std::vector<std::string> args{test_program_name, unknown_arg_flag};

    const auto argc = static_cast<int>(args.size());
    auto argv = to_char_2d_array(args);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        parsing_failure::argument_deduction_failure({unknown_arg_flag}).what(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args_unknown_flags_as_values,
    "parse_args should treat an unknown argument flag as an optional argument's value if it's "
    "proceeded by an optional argument's flag"
) {
    const std::string opt_arg_name = "known-opt-arg";
    sut.add_optional_argument(opt_arg_name);

    const std::vector<std::string> args{
        test_program_name, std::format("--{}", opt_arg_name), unknown_arg_flag
    };

    const auto argc = static_cast<int>(args.size());
    auto argv = to_char_2d_array(args);

    CHECK_NOTHROW(sut.parse_args(argc, argv));

    CHECK_EQ(sut.value(opt_arg_name), unknown_arg_flag);

    free_argv(argc, argv);
}
