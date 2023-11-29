#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <argument_parser_test_fixture.hpp>

#include <iostream>
#include <cstring>

using namespace ap::detail;


namespace {

constexpr std::string_view test_program_name = "test program name";

constexpr int default_argc = 1;
constexpr int non_default_argc = 5;


std::string arg_value(int i) {
    return "test_argv_" + std::to_string(i);
}

char** prepare_argv(int argc) {
    char** argv = new char*[argc];

    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    for (int i = 1; i < argc; i++) {
        std::string arg = arg_value(i - 1);
        argv[i] = new char[arg.length() + 1];
        std::strcpy(argv[i], arg.c_str());
    }

    return argv;
}

void free_argv(int argc, char** argv) {
    for (int i = 0; i < argc; i++)
        delete[] argv[i];
    delete[] argv;
}

} // namespace


namespace ap_testing {


TEST_SUITE_BEGIN("test_argument_parser_parse_args");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_process_input should return an empty vector for no command-line arguments"
) {
    auto argv = prepare_argv(default_argc);

    const auto args = _process_input(default_argc, argv);

    REQUIRE(args.empty());

    free_argv(default_argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_process_input should return a vector of correct arguments"
) {
    auto argv = prepare_argv(non_default_argc);

    const auto args = _process_input(non_default_argc, argv);

    REQUIRE_EQ(args.size(), non_default_argc - 1);
    for (std::size_t i = 0; i < args.size(); i++)
        REQUIRE_EQ(args.at(i), arg_value(static_cast<int>(i)));
}

TEST_SUITE_END();

} // namespace ap_testing
