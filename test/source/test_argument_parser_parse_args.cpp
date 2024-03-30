#define AP_TESTING

#include "argument_parser_test_fixture.hpp"
#include "doctest.h"

#include <ap/argument_parser.hpp>

using namespace ap_testing;
using namespace ap::argument;
using namespace ap::nargs;

namespace {

constexpr std::string_view test_program_name = "test program name";

constexpr std::size_t default_num_args = 0;
constexpr std::size_t non_default_num_args = 10;
constexpr std::size_t non_default_args_split = non_default_num_args / 2;

const std::string invalid_arg_name = "invalid_arg";

const std::string positional_primary_name = "positional_arg";
const std::string positional_secondary_name = "pa";
const std::string optional_primary_name = "optional_arg";
const std::string optional_secondary_name = "oa";

} // namespace

TEST_SUITE_BEGIN("test_argument_parser_parse_args");

// _preprocess_input

TEST_CASE_FIXTURE(argument_parser_test_fixture, "_preprocess_input should return an empty vector for no command-line arguments") {
    const auto argc = get_argc(default_num_args, default_num_args);
    auto argv = prepare_argv(default_num_args, default_num_args);

    const auto args = sut_process_input(argc, argv);

    REQUIRE(args.empty());

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "_preprocess_input should return a vector of correct arguments") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    const auto cmd_args = sut_process_input(argc, argv);

    REQUIRE_EQ(cmd_args.size(), get_args_length(non_default_num_args, non_default_args_split));

    for (std::size_t i = 0; i < non_default_args_split; i++) { // positional args
        REQUIRE_EQ(cmd_args.at(i).discriminator, cmd_argument::type_discriminator::value);
        REQUIRE_EQ(cmd_args.at(i).value, prepare_arg_value(i));
    }

    std::size_t opt_arg_idx = non_default_args_split;
    for (std::size_t i = non_default_args_split; i < cmd_args.size(); i += 2) { // optional args
        REQUIRE_EQ(cmd_args.at(i).discriminator, cmd_argument::type_discriminator::flag);
        REQUIRE_EQ(cmd_args.at(i).value, prepare_arg_name(opt_arg_idx));
        REQUIRE_EQ(cmd_args.at(i + 1).discriminator, cmd_argument::type_discriminator::value);
        REQUIRE_EQ(cmd_args.at(i + 1).value, prepare_arg_value(opt_arg_idx));
        opt_arg_idx++;
    }

    free_argv(argc, argv);
}


// _parse_args_impl

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_parse_args_impl should throw when there is a non-positional value specified "
    "without an argument flag present before the value"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    auto cmd_args = prepare_cmd_arg_list(non_default_num_args, non_default_args_split);
    cmd_args.erase(std::next(cmd_args.begin(), non_default_args_split));

    REQUIRE_THROWS_AS(sut_parse_args_impl(cmd_args), ap::error::free_value_error);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "_parse_args_impl should not throw when the arguments are correct") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto cmd_args = prepare_cmd_arg_list(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut_parse_args_impl(cmd_args));
}


// _get_argument

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
        REQUIRE(sut_get_argument(arg_name.primary));
        REQUIRE(sut_get_argument(arg_name.secondary.value()));
    }
}


// parse_args

TEST_CASE_FIXTURE(argument_parser_test_fixture, "parse_args should throw when there is no value specified for a required optional argument") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);
    sut.add_optional_argument(required_arg_name.primary, required_arg_name.secondary.value()).required();

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_THROWS_AS(sut.parse_args(argc, argv), ap::error::required_argument_not_parsed_error);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "parse_args should throw when an optional argument's nvalues is not in a specified range") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    const auto range_arg_name = prepare_arg_name(non_default_num_args);
    sut.add_optional_argument(range_arg_name.primary, range_arg_name.secondary.value()).nargs(at_least(1));

    REQUIRE_THROWS_AS(sut.parse_args(argc, argv), ap::error::invalid_nvalues_error);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "parse_args should not throw if input and numer of positional argument"
    "values are correct and all required optional arguments have values"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);
    sut.add_optional_argument(required_arg_name.primary, required_arg_name.secondary.value()).required();

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
        std::strcpy(argv[argc - 2], prepare_arg_flag_primary(arg_i).c_str());
        std::strcpy(argv[argc - 1], prepare_arg_value(arg_i).c_str());
    }

    CAPTURE(argc);
    CAPTURE(argv);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "parse_args should not throw if there is an argument which has bypass_required "
    "option enabled and is used"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto bypass_required_arg_name = prepare_arg_name(non_default_num_args);
    sut.add_optional_argument<bool>(
           bypass_required_arg_name.primary, bypass_required_arg_name.secondary.value()
    )
        .default_value(false)
        .implicit_value(true)
        .bypass_required();


    const int argc = 2;

    char** argv = new char*[argc];
    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    std::string arg_flag;

    SUBCASE("primary flag") {
        arg_flag = prepare_arg_flag_primary(non_default_num_args);
    }
    SUBCASE("secondary flag") {
        arg_flag = prepare_arg_flag_secondary(non_default_num_args);
    }

    CAPTURE(arg_flag);

    argv[1] = new char[arg_flag.length() - 1];
    std::strcpy(argv[1], arg_flag.c_str());

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));
    REQUIRE(sut.value<bool>(bypass_required_arg_name.primary));

    free_argv(argc, argv);
}


// has_value

TEST_CASE_FIXTURE(argument_parser_test_fixture, "has_value should return false if there is no argument with given name present") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));
    REQUIRE_FALSE(sut.has_value(invalid_arg_name));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "has_value should return false when an argument has no values") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    sut.add_optional_argument(required_arg_name.primary, required_arg_name.secondary.value()).required();

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
            REQUIRE(sut.has_value(arg_name.primary));
            REQUIRE(sut.has_value(arg_name.secondary.value()));
        }
    }
    SUBCASE("only the necessary arguments have values") {
        std::size_t num_args_passed_as_input = non_default_args_split + 1;
        argc = get_argc(num_args_passed_as_input, non_default_args_split);
        argv = prepare_argv(num_args_passed_as_input, non_default_args_split);

        const auto arg_i = non_default_num_args;
        std::strcpy(argv[argc - 2], prepare_arg_flag_primary(arg_i).c_str());
        std::strcpy(argv[argc - 1], prepare_arg_value(arg_i).c_str());

        REQUIRE_NOTHROW(sut.parse_args(argc, argv));

        for (std::size_t i = 0; i < non_default_args_split; i++) {
            const auto arg_name = prepare_arg_name(i);
            REQUIRE(sut.has_value(arg_name.primary));
            REQUIRE(sut.has_value(arg_name.secondary.value()));
        }
        for (std::size_t i = non_default_args_split; i < non_default_num_args; i++) {
            const auto arg_name = prepare_arg_name(i);
            REQUIRE_FALSE(sut.has_value(arg_name.primary));
            REQUIRE_FALSE(sut.has_value(arg_name.secondary.value()));
        }
        for (std::size_t i = non_default_num_args; i < num_args; i++) {
            const auto arg_name = prepare_arg_name(i);
            REQUIRE(sut.has_value(arg_name.primary));
            REQUIRE(sut.has_value(arg_name.secondary.value()));
        }
    }

    CAPTURE(argc);
    CAPTURE(argv);

    free_argv(argc, argv);
}


// value

TEST_CASE_FIXTURE(argument_parser_test_fixture, "value() should throw if there is no argument with given name present") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    REQUIRE_THROWS_AS(sut.value(invalid_arg_name), ap::error::argument_not_found_error);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "value() should throw before calling parse_args") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_THROWS_AS(sut.value(arg_name.primary), ap::error::invalid_value_type_error);
        REQUIRE_THROWS_AS(sut.value(arg_name.secondary.value()), ap::error::invalid_value_type_error);
    }
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "value() should throw if the given argument doesn't have a value") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    sut.add_optional_argument(required_arg_name.primary, required_arg_name.secondary.value()).required();

    const auto num_args = non_default_num_args + 1;

    std::size_t num_args_passed_as_input = non_default_args_split + 1;
    const auto argc = get_argc(num_args_passed_as_input, non_default_args_split);
    auto argv = prepare_argv(num_args_passed_as_input, non_default_args_split);

    const auto arg_i = non_default_num_args;
    std::strcpy(argv[argc - 2], prepare_arg_flag_primary(arg_i).c_str());
    std::strcpy(argv[argc - 1], prepare_arg_value(arg_i).c_str());

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    for (std::size_t i = 0; i < non_default_args_split; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_NOTHROW(sut.value(arg_name.primary));
        REQUIRE_NOTHROW(sut.value(arg_name.secondary.value()));
    }
    for (std::size_t i = non_default_args_split; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_THROWS_AS(sut.value(arg_name.primary), ap::error::invalid_value_type_error);
        REQUIRE_THROWS_AS(sut.value(arg_name.secondary.value()), ap::error::invalid_value_type_error);
    }
    for (std::size_t i = non_default_num_args; i < num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_NOTHROW(sut.value(arg_name.primary));
        REQUIRE_NOTHROW(sut.value(arg_name.secondary.value()));
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "value() should throw if an argument has a value but the given type is invalid") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    using invalid_value_type = int;

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);

        REQUIRE(sut.has_value(arg_name.primary));
        REQUIRE_THROWS_AS(sut.value<invalid_value_type>(arg_name.primary), ap::error::invalid_value_type_error);
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "value() should return a correct value when there is an argument"
    "with the given name and a parsed value present"
) {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto required_arg_name = prepare_arg_name(non_default_num_args);

    sut.add_optional_argument(required_arg_name.primary, required_arg_name.secondary.value()).required();

    const auto num_args = non_default_num_args + 1;

    const auto argc = get_argc(num_args, non_default_args_split);
    auto argv = prepare_argv(num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        const auto arg_value = prepare_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary));
        REQUIRE_EQ(sut.value(arg_name.primary), arg_value);
        REQUIRE_EQ(sut.value(arg_name.secondary.value()), arg_value);
    }

    free_argv(argc, argv);
}


// count

TEST_CASE_FIXTURE(argument_parser_test_fixture, "count should return 0 before calling parse_args") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    for (std::size_t i = 0; i < non_default_num_args; i++) {
        const auto arg_name = prepare_arg_name(i);
        REQUIRE_EQ(sut.count(arg_name.primary), 0u);
        REQUIRE_EQ(sut.count(arg_name.secondary.value()), 0u);
    }
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "count should return 0 if there is no argument with given name present") {
    add_arguments(sut, non_default_num_args, non_default_args_split);

    const auto argc = get_argc(non_default_num_args, non_default_args_split);
    auto argv = prepare_argv(non_default_num_args, non_default_args_split);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));
    REQUIRE_EQ(sut.count(invalid_arg_name), 0u);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "count should return the number of argument's flag usage") {
    // prepare sut
    sut.add_positional_argument(positional_primary_name, positional_secondary_name);
    sut.add_optional_argument(optional_primary_name, optional_secondary_name).nargs(ap::nargs::any());

    // expected values
    const std::size_t positional_count = 1u;
    const std::size_t optional_count = 4u;

    // prepare argc & argv
    const int argc = 1 + positional_count + 2 * optional_count + 1;
    char** argv = new char*[argc];

    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    const std::string positional_arg_value = "positonal_arg_value";
    argv[1] = new char[positional_arg_value.length() + 1];
    std::strcpy(argv[1], positional_arg_value.c_str());

    const std::string optional_arg_flag = "--" + optional_primary_name;
    const std::string optional_arg_value = optional_primary_name + "_value";
    for (std::size_t i = 2; i < argc; i += 2) {
        if (i == argc - 1) {
            argv[i] = new char[optional_arg_value.length() + 1];
            std::strcpy(argv[i], optional_arg_value.c_str());
            continue;
        }

        argv[i] = new char[optional_arg_flag.length() + 1];
        std::strcpy(argv[i], optional_arg_flag.c_str());

        argv[i + 1] = new char[optional_arg_value.length() + 1];
        std::strcpy(argv[i + 1], optional_arg_value.c_str());
    }

    // parse args
    sut.parse_args(argc, argv);

    // test count
    REQUIRE_EQ(sut.count(positional_primary_name), positional_count);
    REQUIRE_EQ(sut.count(optional_primary_name), optional_count);

    // free argv
    free_argv(argc, argv);
}


// values

TEST_CASE_FIXTURE(argument_parser_test_fixture, "values() should throw when calling with a positional argument's name") {
    sut.add_positional_argument(positional_primary_name, positional_secondary_name);

    REQUIRE_THROWS_AS(sut.values(positional_primary_name), std::logic_error);
    REQUIRE_THROWS_AS(sut.values(positional_secondary_name), std::logic_error);
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "values() should return an empty vector if an argument has no values") {
    sut.add_optional_argument(optional_primary_name, optional_secondary_name);

    SUBCASE("calling with argument's primary name") {
        const auto& values = sut.values(optional_primary_name);
        REQUIRE(values.empty());
    }

    SUBCASE("calling with argument's secondary name") {
        const auto& values = sut.values(optional_secondary_name);
        REQUIRE(values.empty());
    }
}

TEST_CASE_FIXTURE(argument_parser_test_fixture, "values() should throw when an argument has values but the given type is invalid") {
    sut.add_optional_argument(optional_primary_name, optional_secondary_name).nargs(at_least(1));

    // prepare argc & argv
    const int argc = 5;
    char** argv = new char*[argc];

    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    const std::string flag = "--" + optional_primary_name;
    argv[1] = new char[flag.length() + 1];
    std::strcpy(argv[1], flag.c_str());
    for (int i = 2; i < argc; i++) {
        const auto value = prepare_arg_value(i - 1);
        argv[i] = new char[value.length() + 1];
        std::strcpy(argv[i], value.c_str());
    }

    // parse args
    sut.parse_args(argc, argv);

    REQUIRE_THROWS_AS(sut.values<invalid_argument_value_type>(optional_primary_name), ap::error::invalid_value_type_error);
    REQUIRE_THROWS_AS(
        sut.values<invalid_argument_value_type>(optional_secondary_name), ap::error::invalid_value_type_error
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "values() should return a vector containing a predefined value if no values "
    "for an argument have been parsed"
) {
    const std::string default_value = "default_value";
    const std::string implicit_value = "implicit_value";

    sut.add_optional_argument(optional_primary_name, optional_secondary_name)
        .default_value(default_value)
        .implicit_value(implicit_value);

    // prepare argc & argv
    int argc;
    char** argv;
    std::string expected_value;

    SUBCASE("default_value") {
        argc = 1;
        argv = new char*[argc];
        expected_value = default_value;
    }

    SUBCASE("implicit_value") {
        argc = 2;
        argv = new char*[argc];

        const auto optional_arg_flag = "--" + optional_primary_name;
        argv[1] = new char[optional_arg_flag.length() + 1];
        std::strcpy(argv[1], optional_arg_flag.c_str());
        expected_value = implicit_value;
    }

    CAPTURE(argc);
    CAPTURE(argv);
    CAPTURE(expected_value);

    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    // parse args
    sut.parse_args(argc, argv);

    const auto& stored_values = sut.values(optional_primary_name);

    REQUIRE_EQ(stored_values.size(), 1);
    REQUIRE_EQ(stored_values.front(), expected_value);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "values() should return a correct vector of values when there is an argument with "
    "a given name and parsed values present"
) {
    sut.add_optional_argument(optional_primary_name, optional_secondary_name).nargs(at_least(1));

    // prepare argc & argv
    const int argc = 5;
    char** argv = new char*[argc];

    argv[0] = new char[8];
    std::strcpy(argv[0], "program");

    const std::string flag = "--" + optional_primary_name;
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

    const auto& stored_values = sut.values(optional_primary_name);

    REQUIRE_EQ(stored_values.size(), values.size());
    for (std::size_t i = 0; i < stored_values.size(); i++)
        REQUIRE_EQ(stored_values[i], values[i]);

    free_argv(argc, argv);
}

TEST_SUITE_END(); // test_argument_parser_parse_args
