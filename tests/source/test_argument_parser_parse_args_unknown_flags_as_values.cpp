#include "argument_parser_test_fixture.hpp"
#include "doctest.h"
#include "utility.hpp"

using namespace ap_testing;
using namespace ap::argument;
using namespace ap::nargs;
using ap::invalid_configuration;
using ap::parsing_failure;

TEST_SUITE_BEGIN("test_argument_parser_parse_args_unknown_flags_as_values");

struct test_argument_parser_parse_args_unknown_flags_as_values
: public argument_parser_test_fixture {
    const std::size_t no_args = 0ull;
};

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args_unknown_flags_as_values,
    "parse_args should treat an unknown argument flag as a positional value if it's not preceeded "
    "by any valid argument flags"
) {
    CHECK(false);

    // add_arguments(no_args, no_args);

    // constexpr std::size_t n_opt_clargs = 1ull;
    // constexpr std::size_t opt_arg_idx = 0ull;

    // auto argc = get_argc(no_args, n_opt_clargs);
    // auto argv = init_argv(no_args, n_opt_clargs);

    // const auto unknown_arg_name = init_arg_flag_primary(opt_arg_idx);

    // CHECK_THROWS_WITH_AS(
    //     sut.parse_args(argc, argv),
    //     parsing_failure::unknown_argument(unknown_arg_name).what(),
    //     parsing_failure
    // );

    // free_argv(argc, argv);
}

TEST_SUITE_END(); // test_argument_parser_parse_args_unknown_flags_as_values
