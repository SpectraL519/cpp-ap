#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <argument_parser_test_fixture.hpp>

using namespace ap::detail;


namespace {

constexpr std::string_view test_program_name = "test program name";

constexpr int default_argc = 1;
constexpr int non_default_argc = 11;



} // namespace


namespace ap_testing {


TEST_SUITE_BEGIN("test_argument_parser_parse_args");

TEST_SUITE("_process_input") {
    TEST_CASE_FIXTURE(
        argument_parser_test_fixture,
        "_process_input should return an empty vector for no command-line arguments"
    ) {
        auto argv = prepare_argv(default_argc, default_argc);

        const auto args = _process_input(default_argc, argv);

        REQUIRE(args.empty());

        free_argv(default_argc, argv);
    }

    TEST_CASE_FIXTURE(
        argument_parser_test_fixture,
        "_process_input should return a vector of correct arguments"
    ) {
        auto argv = prepare_argv(non_default_argc, non_default_argc / 2 + 1);

        const auto args = _process_input(non_default_argc, argv);

        REQUIRE_EQ(args.size(), non_default_argc - 1);
        for (std::size_t i = 0; i < args.size(); i++)
            REQUIRE_EQ(args.at(i), prepare_arg_name(static_cast<int>(i)));
    }
}

TEST_SUITE_BEGIN("_parse_args_impl");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when input argument list is empty"
) {
    const auto argc_split = non_default_argc / 2 + 1;
    add_arguments(sut, non_default_argc, argc_split);

    cmd_argument_list cmd_args = prepare_cmd_arg_list(0);

    REQUIRE_THROWS_AS(_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is less input arguments than there are added positional arguments"
) {
    const auto argc_split = non_default_argc / 2 + 1;
    add_arguments(sut, non_default_argc, argc_split);

    const auto cmd_args = prepare_cmd_arg_list(non_default_argc / 4);

    REQUIRE_THROWS_AS(_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_SUITE_END(); // _parse_args_impl
TEST_SUITE_END(); // test_argument_parser_parse_args

} // namespace ap_testing
