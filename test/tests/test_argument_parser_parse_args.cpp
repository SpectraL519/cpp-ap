// TODO: split this file into sepparate test files for each test_suite
// These test files should be in a common directory

#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <argument_parser_test_fixture.hpp>

using namespace ap::argument;
using namespace ap::nargs;

namespace {

constexpr std::string_view test_program_name = "test program name";

constexpr std::size_t default_num_args = 0;
constexpr std::size_t non_default_num_args = 10;
constexpr std::size_t non_default_args_split = non_default_num_args / 2;

const std::string invalid_arg_name = "invalid_arg";

const std::string positional_arg_name = "positional_arg";
const std::string positional_arg_short_name = "pa";
const std::string optional_arg_name = "optional_arg";
const std::string optional_arg_short_name = "oa";

} // namespace

namespace ap_testing {

TEST_SUITE_BEGIN("test_argument_parser_parse_args");

TEST_SUITE_BEGIN("test_argument_parser_parse_args::_process_input");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_process_input should return an empty vector for no command-line arguments"
) {
    const auto argc = get_argc(default_num_args, default_num_args);
    auto argv = prepare_argv(default_num_args, default_num_args);

    const auto args = sut_process_input(argc, argv);

    REQUIRE(args.empty());

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_process_input should return a vector of correct arguments"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    const auto cmd_args = sut_process_input(argc, argv);

    REQUIRE_EQ(
        cmd_args.size(),
        get_args_length(non_default_num_args, non_default_args_split)
    );

    for (std::size_t i = 0; i < non_default_args_split; i++) { // positional args
        REQUIRE_EQ(
            cmd_args.at(i).discriminator, cmd_argument::type_discriminator::value);
        REQUIRE_EQ(cmd_args.at(i).value, prepare_arg_value(i));
    }

    std::size_t opt_arg_idx = non_default_args_split;
    for (std::size_t i = non_default_args_split; i < cmd_args.size(); i += 2) { // optional args
        REQUIRE_EQ(
            cmd_args.at(i).discriminator, cmd_argument::type_discriminator::flag);
        REQUIRE_EQ(cmd_args.at(i).value, prepare_arg_name(opt_arg_idx));
        REQUIRE_EQ(
            cmd_args.at(i + 1).discriminator, cmd_argument::type_discriminator::value);
        REQUIRE_EQ(cmd_args.at(i + 1).value, prepare_arg_value(opt_arg_idx));
        opt_arg_idx++;
    }

    free_argv(argc, argv);
}

TEST_SUITE_END(); // test_argument_parser_parse_args::_process_input


TEST_SUITE_BEGIN("test_argument_parser_parse_args::_parse_args_impl");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when input argument list is empty"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    constexpr int no_args = 0;
    const auto cmd_args = prepare_cmd_arg_list(no_args, no_args);

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is less input"
    "arguments than there are positional arguments"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto num_positional_values = non_default_num_args / 4;
    const auto cmd_args =
        prepare_cmd_arg_list(num_positional_values, num_positional_values);

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is less input positional"
    "argument values than there are positional arguments"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    auto cmd_args = prepare_cmd_arg_list(non_default_num_args, non_default_args_split);
    cmd_args.erase(std::next(cmd_args.begin(), non_default_args_split - 1));

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is a non-positional value specified "
    "without an argument flag present before the value"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    auto cmd_args = prepare_cmd_arg_list(non_default_num_args, non_default_args_split);
    cmd_args.erase(std::next(cmd_args.begin(), non_default_args_split));

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), std::runtime_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should not throw when the arguments are correct"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto cmd_args =
        prepare_cmd_arg_list(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut_parse_args_impl(cmd_args));
}

TEST_SUITE_END(); // test_argument_parser_parse_args::_parse_args_impl


TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "parse_args should throw when there is no value specified for a required optional argument"
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
    "parse_args should throw when an optional argument's nvalues is not in a specified range"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    const auto range_arg_name = prepare_arg_name(non_default_num_args);
    sut.add_optional_argument(
        range_arg_name.name, range_arg_name.short_name.value()
    ).nargs(at_least(1));

    REQUIRE_THROWS_AS(sut.parse_args(argc, argv), std::runtime_error);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "parse_args should not throw if input and numer of positional argument"
    "values are correct and all required optional arguments have values"
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

TEST_SUITE_END(); // test_argument_parser_parse_args::_get_argument


TEST_SUITE_BEGIN("test_argument_parser_parse_args::has_value");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "has_value should return false if there is no argument with given name present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    REQUIRE_FALSE(sut.has_value(invalid_arg_name));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "has_value should return false before calling parse_args"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_FALSE(sut.has_value(arg_name.name));
        REQUIRE_FALSE(sut.has_value(arg_name.short_name.value()));
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "has_value should return false if there is no argument with given name present"
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


TEST_SUITE_BEGIN("test_argument_parser_parse_args::value");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "value should throw if there is no argument with given name present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    REQUIRE_THROWS_AS(sut.value(invalid_arg_name), std::invalid_argument);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "value should throw before calling parse_args"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_THROWS_AS(sut.value(arg_name.name), std::invalid_argument);
        REQUIRE_THROWS_AS(sut.value(arg_name.short_name.value()), std::invalid_argument);
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "value should throw if the given argument doesn't have a value"
) {
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
    "value should throw if an argument has a value but the given type is invalid"
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
    "value should return a correct value when there is an argument"
    "with the given name and a parsed value present"
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


TEST_SUITE_BEGIN("test_argument_parser_parse_args::values");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "values() should throw when calling with a positional argument's name"
) {
    sut.add_positional_argument(positional_arg_name, positional_arg_short_name);

    REQUIRE_THROWS_AS(sut.values(positional_arg_name), std::logic_error);
    REQUIRE_THROWS_AS(sut.values(positional_arg_short_name), std::logic_error);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "values() should return an empty vector if an argument has no values"
) {
    sut.add_optional_argument(optional_arg_name, optional_arg_short_name);

    SUBCASE("calling with argument's long name") {
        const auto& values = sut.values(optional_arg_name);
        REQUIRE(values.empty());
    }

    SUBCASE("calling with argument's short name") {
        const auto& values = sut.values(optional_arg_short_name);
        REQUIRE(values.empty());
    }
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "values() should throw when an argument has values but the given type is invalid"
) {
    sut.add_optional_argument(optional_arg_name, optional_arg_short_name)
       .nargs(at_least(1));

    // prepare argc & argv
    const int argc = 5;
    char** argv = new char*[argc];

    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    const std::string flag = "--" + optional_arg_name;
    argv[1] = new char[flag.length() + 1];
    std::strcpy(argv[1], flag.c_str());
    for (int i = 2; i < argc; i++) {
        const auto value = prepare_arg_value(i - 1);
        argv[i] = new char[value.length() + 1];
        std::strcpy(argv[i], value.c_str());
    }

    // parse args
    sut.parse_args(argc, argv);

    REQUIRE_THROWS_AS(
        sut.values<invalid_argument_value_type>(optional_arg_name), std::logic_error);
    REQUIRE_THROWS_AS(
        sut.values<invalid_argument_value_type>(optional_arg_short_name), std::logic_error);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "values() should return a correct vector of values when there is an argument with "
    "a given name and parsed values present"
) {
    sut.add_optional_argument(optional_arg_name, optional_arg_short_name)
       .nargs(at_least(1));

    // prepare argc & argv
    const int argc = 5;
    char** argv = new char*[argc];

    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    const std::string flag = "--" + optional_arg_name;
    argv[1] = new char[flag.length() + 1];
    std::strcpy(argv[1], flag.c_str());

    std::vector<std::string> values;

    for (int i = 2; i < argc; i++) {
        const auto value = prepare_arg_value(i - 1);
        argv[i] = new char[value.length() + 1];
        std::strcpy(argv[i], value.c_str());
        values.push_back(value);
    }

    // parse args
    sut.parse_args(argc, argv);

    const auto& stored_values = sut.values(optional_arg_name);

    REQUIRE_EQ(stored_values.size(), values.size());
    for (std::size_t i = 0; i < stored_values.size(); i++)
        REQUIRE_EQ(stored_values[i], values[i]);

    free_argv(argc, argv);
}

TEST_SUITE_END(); // test_argument_parser_parse_args::values

TEST_SUITE_END(); // test_argument_parser_parse_args

} // namespace ap_testing
