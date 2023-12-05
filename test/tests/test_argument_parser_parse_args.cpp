// TODO: split this file into sepparate test files for each test_suite
// These test files should be in a common directory

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

const std::string invalid_arg_name = "invalid_arg";

} // namespace


namespace ap_testing {


TEST_SUITE_BEGIN("test_argument_parser_parse_args");

TEST_SUITE_BEGIN("test_argument_parser_parse_args::_process_input");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_process_input should return an empty "
    "vector for no command-line arguments"
) {
    const auto argc = get_argc(default_num_args, default_num_args);
    auto argv = prepare_argv(default_num_args, default_num_args);

    const auto args = sut_process_input(argc, argv);

    REQUIRE(args.empty());

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "_process_input should return a vector of correct arguments") {
    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    const auto args = sut_process_input(argc, argv);

    REQUIRE_EQ(args.size(), get_args_length(non_default_num_args, non_default_args_split));

    for (std::size_t i = 0; i < non_default_args_split; i++) { // positional
                                                               // args
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

TEST_SUITE_END(); // test_argument_parser_parse_args::_process_input


TEST_SUITE_BEGIN("test_argument_parser_parse_args::_parse_args_impl");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when input "
    "argument list is empty"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    constexpr int no_args = 0;
    cmd_argument_list cmd_args = prepare_cmd_arg_list(no_args, no_args);

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is "
    "less input arguments than there are "
    "positional arguments"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto num_positional_values = non_default_num_args / 4;
    const auto cmd_args =
        prepare_cmd_arg_list(num_positional_values, num_positional_values);

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is "
    "less input positional argument values than "
    "there are positional arguments"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    auto cmd_args =
        prepare_cmd_arg_list(non_default_num_args, non_default_args_split);
    cmd_args.erase(std::next(cmd_args.begin(), non_default_args_split - 1));

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is "
    "no value specified for an optional argument "
    "present in the input list"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    auto cmd_args =
        prepare_cmd_arg_list(non_default_num_args, non_default_args_split);
    cmd_args.erase(std::next(cmd_args.begin(), non_default_args_split + 1));

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should not throw when the "
    "arguments are correct"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto cmd_args =
        prepare_cmd_arg_list(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut_parse_args_impl(cmd_args));
}

TEST_SUITE_END(); // test_argument_parser_parse_args::_parse_args_impl


TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "parse_args should throw when there is no "
    "value specified for a required optional "
    "argument"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);
    sut.add_optional_argument(
        required_arg_name.name, required_arg_name.short_name.value()
    ).required();

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_THROWS_AS(sut.parse_args(argc, argv), std::runtime_error);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "parse_args should not throw if input and "
    "numer of positional argument values are "
    "correct and all required optional arguments "
    "have values"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);
    sut.add_optional_argument(
        required_arg_name.name, required_arg_name.short_name.value()
    ).required();

    int argc;
    char** argv;

    SUBCASE("all arguments have values") {
        std::size_t correct_num_args = non_default_num_args + 1;
        argc = get_argc(correct_num_args, non_default_args_split);
        argv = prepare_argv(correct_num_args, non_default_args_split);
    }
    SUBCASE("only the necessary arguments have values") {
        std::size_t correct_num_args = non_default_args_split + 1;
        argc = get_argc(correct_num_args, non_default_args_split);
        argv = prepare_argv(correct_num_args, non_default_args_split);

        const auto arg_i = non_default_num_args;
        std::strcpy(argv[argc - 2], prepare_arg_flag(arg_i).c_str());
        std::strcpy(argv[argc - 1], prepare_arg_value(arg_i).c_str());
    }

    CAPTURE(argc);
    CAPTURE(argv);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    free_argv(argc, argv);
}


TEST_SUITE_BEGIN("test_argument_parser_parse_args::_get_argument");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_get_argument should return nullopt if "
    "there is no argument with given name present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    REQUIRE_FALSE(sut_get_argument(invalid_arg_name));
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_get_argument should return valid argument "
    "if there is an argument with the given name"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE(sut_get_argument(arg_name.name));
        REQUIRE(sut_get_argument(arg_name.short_name.value()));
    }
}

TEST_SUITE_END(); // test_argument_parser_parse_args::has_value


TEST_SUITE_BEGIN("test_argument_parser_parse_args::has_value");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "has_value should return false if there is "
    "no argument with given name present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    REQUIRE_FALSE(sut.has_value(invalid_arg_name));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "has_value should return false before calling parse_args") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_FALSE(sut.has_value(arg_name.name));
        REQUIRE_FALSE(sut.has_value(arg_name.short_name.value()));
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "has_value should return false if there is "
    "no argument with given name present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    sut.add_optional_argument(
        required_arg_name.name, required_arg_name.short_name.value()
    ).required();

    const auto num_args = non_default_num_args + 1;

    int argc;
    char** argv;

    SUBCASE("all arguments have values") {
        std::size_t num_args_passed_as_input = num_args;
        argc = get_argc(num_args_passed_as_input, non_default_args_split);
        argv = prepare_argv(num_args_passed_as_input, non_default_args_split);

        REQUIRE_NOTHROW(sut.parse_args(argc, argv));

        for (std::size_t i = 0; i < non_default_num_args; i++) {
            const auto arg_name = prepare_arg_name(i);
            REQUIRE(sut.has_value(arg_name.name));
            REQUIRE(sut.has_value(arg_name.short_name.value()));
        }
    }
    SUBCASE("only the necessary arguments have values") {
        std::size_t num_args_passed_as_input = non_default_args_split + 1;
        argc = get_argc(num_args_passed_as_input, non_default_args_split);
        argv = prepare_argv(num_args_passed_as_input, non_default_args_split);

        const auto arg_i = non_default_num_args;
        std::strcpy(argv[argc - 2], prepare_arg_flag(arg_i).c_str());
        std::strcpy(argv[argc - 1], prepare_arg_value(arg_i).c_str());

        REQUIRE_NOTHROW(sut.parse_args(argc, argv));

        for (std::size_t i = 0; i < non_default_args_split; i++) {
            const auto arg_name = prepare_arg_name(i);
            REQUIRE(sut.has_value(arg_name.name));
            REQUIRE(sut.has_value(arg_name.short_name.value()));
        }
        for (std::size_t i = non_default_args_split; i < non_default_num_args; i++) {
            const auto arg_name = prepare_arg_name(i);
            REQUIRE_FALSE(sut.has_value(arg_name.name));
            REQUIRE_FALSE(sut.has_value(arg_name.short_name.value()));
        }
        for (std::size_t i = non_default_num_args; i < num_args; i++) {
            const auto arg_name = prepare_arg_name(i);
            REQUIRE(sut.has_value(arg_name.name));
            REQUIRE(sut.has_value(arg_name.short_name.value()));
        }
    }

    CAPTURE(argc);
    CAPTURE(argv);

    free_argv(argc, argv);
}

TEST_SUITE_END(); // test_argument_parser_parse_args::has_value


TEST_SUITE_BEGIN("value");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "value should throw if there is no argument "
    "with given name present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    REQUIRE_THROWS_AS(sut.value(invalid_arg_name), std::invalid_argument);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "value should throw before calling parse_args") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_THROWS_AS(sut.value(arg_name.name), std::invalid_argument);
        REQUIRE_THROWS_AS(sut.value(arg_name.short_name.value()), std::invalid_argument);
    }
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "value should throw if the given argument doesn't have a value") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    sut.add_optional_argument(
        required_arg_name.name, required_arg_name.short_name.value()
    ).required();

    const auto num_args = non_default_num_args + 1;

    std::size_t num_args_passed_as_input = non_default_args_split + 1;
    const auto argc = get_argc(num_args_passed_as_input, non_default_args_split);
    auto argv = prepare_argv(num_args_passed_as_input, non_default_args_split);

    const auto arg_i = non_default_num_args;
    std::strcpy(argv[argc - 2], prepare_arg_flag(arg_i).c_str());
    std::strcpy(argv[argc - 1], prepare_arg_value(arg_i).c_str());

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    for (std::size_t i = 0; i < non_default_args_split; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_NOTHROW(sut.value(arg_name.name));
        REQUIRE_NOTHROW(sut.value(arg_name.short_name.value()));
    }
    for (std::size_t i = non_default_args_split; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_THROWS_AS(sut.value(arg_name.name), std::invalid_argument);
        REQUIRE_THROWS_AS(sut.value(arg_name.short_name.value()), std::invalid_argument);
    }
    for (std::size_t i = non_default_num_args; i < num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_NOTHROW(sut.value(arg_name.name));
        REQUIRE_NOTHROW(sut.value(arg_name.short_name.value()));
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "value should throw if an argument has a "
    "value but the template parameter is invalid"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    using invalid_value_type = int;

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);

        REQUIRE(sut.has_value(arg_name.name));
        REQUIRE_THROWS_AS(sut.value<invalid_value_type>(arg_name.name), std::invalid_argument);
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "value should return a correct value when "
    "there is an argument with the given name "
    "and a parsed value present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    sut.add_optional_argument(
        required_arg_name.name, required_arg_name.short_name.value()
    ).required();

    const auto num_args = non_default_num_args + 1;

    const auto argc = get_argc(num_args, non_default_args_split);
    auto argv = prepare_argv(num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        const auto arg_value = prepare_arg_value(i);

        REQUIRE(sut.has_value(arg_name.name));
        REQUIRE_EQ(sut.value(arg_name.name), arg_value);
        REQUIRE_EQ(sut.value(arg_name.short_name.value()), arg_value);
    }

    free_argv(argc, argv);
}

TEST_SUITE_END(); // test_argument_parser_parse_args::value
TEST_SUITE_END(); // test_argument_parser_parse_args

} // namespace ap_testing
