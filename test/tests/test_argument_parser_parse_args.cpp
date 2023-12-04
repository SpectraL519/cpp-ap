#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <argument_parser_test_fixture.hpp>

using namespace ap::detail;


namespace {

constexpr std::string_view test_program_name = "test program name";

constexpr std::size_t default_num_args = 0;
constexpr std::size_t non_default_num_args = 10;
constexpr std::size_t non_default_args_split = non_default_num_args / 2;

} // namespace


namespace ap_testing {


TEST_SUITE_BEGIN("test_argument_parser_parse_args");

TEST_SUITE("_process_input") {
    TEST_CASE_FIXTURE(
        argument_parser_test_fixture,
        "_process_input should return an empty vector for no command-line arguments"
    ) {
        const auto argc = get_argc(default_num_args, default_num_args);
        auto argv = prepare_argv(default_num_args, default_num_args);

        const auto args = _process_input(argc, argv);

        REQUIRE(args.empty());

        free_argv(argc, argv);
    }

    TEST_CASE_FIXTURE(
        argument_parser_test_fixture,
        "_process_input should return a vector of correct arguments"
    ) {
        const auto argc = get_argc(non_default_num_args, non_default_args_split);
        auto argv = prepare_argv(non_default_num_args, non_default_args_split);

        const auto args = _process_input(argc, argv);

        REQUIRE_EQ(args.size(), get_args_length(non_default_num_args, non_default_args_split));

        for (std::size_t i = 0; i < non_default_args_split; i++) { // positional args
            REQUIRE_EQ(args.at(i), prepare_arg_value(i));
        }

        std::size_t opt_arg_i = non_default_args_split;
        for (std::size_t i = non_default_args_split; i < args.size(); i += 2) { // optional args
            REQUIRE_EQ(args.at(i), prepare_arg_name(opt_arg_i));
            REQUIRE_EQ(args.at(i + 1), prepare_arg_value(opt_arg_i));
            opt_arg_i++;
        }

        free_argv(argc, argv);
    }
}

TEST_SUITE_BEGIN("_parse_args_impl");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when input argument list is empty"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    constexpr int no_args = 0;
    cmd_argument_list cmd_args = prepare_cmd_arg_list(no_args, no_args);

    REQUIRE_THROWS_AS(_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is less input arguments than there are added positional arguments"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto num_positional_values = non_default_num_args / 4;
    const auto cmd_args = prepare_cmd_arg_list(num_positional_values, num_positional_values);

    REQUIRE_THROWS_AS(_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should not throw when the arguments are correct"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto cmd_args = prepare_cmd_arg_list(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(_parse_args_impl(cmd_args));
}

TEST_SUITE_END(); // _parse_args_impl
TEST_SUITE_END(); // test_argument_parser_parse_args

} // namespace ap_testing
