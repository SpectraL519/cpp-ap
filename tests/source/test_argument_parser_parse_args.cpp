#define AP_TESTING

#include "argument_parser_test_fixture.hpp"
#include "doctest.h"

using namespace ap_testing;
using namespace ap::argument;
using namespace ap::nargs;

TEST_SUITE_BEGIN("test_argument_parser_parse_args");

struct test_argument_parser_parse_args : public argument_parser_test_fixture {
    const std::string_view test_program_name = "test program name";

    const std::size_t no_args = 0ull;
    const std::size_t n_positional_args = 6ull;
    const std::size_t n_optional_args = 4ull;
    const std::size_t n_args_total = n_positional_args + n_optional_args;

    const std::string invalid_arg_name = "invalid_arg";

    const std::string positional_primary_name = "positional_arg";
    const std::string positional_secondary_name = "pa";
    const std::string optional_primary_name = "optional_arg";
    const std::string optional_secondary_name = "oa";

    const std::string empty_str{};
};

// _tokenize

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "_tokenize should return an empty vector for no command-line arguments"
) {
    const auto argc = get_argc(no_args, no_args);
    auto argv = init_argv(no_args, no_args);

    const auto args = tokenize(argc, argv);

    CHECK(args.empty());

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args, "_tokenize should return a vector of correct arguments"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    const auto arg_tokens = tokenize(argc, argv);

    REQUIRE_EQ(arg_tokens.size(), get_args_length(n_positional_args, n_optional_args));

    for (std::size_t i = 0; i < n_positional_args; ++i) {
        REQUIRE_EQ(arg_tokens.at(i).type, argument_token::t_value);
        CHECK_EQ(arg_tokens.at(i).value, init_arg_value(i));
    }

    std::size_t opt_arg_idx = n_positional_args;
    for (std::size_t i = n_positional_args; i < arg_tokens.size(); i += 2ull) {
        REQUIRE_EQ(arg_tokens.at(i).type, argument_token::t_flag);
        CHECK(init_arg_name(opt_arg_idx).match(arg_tokens.at(i).value));

        REQUIRE_EQ(arg_tokens.at(i + 1ull).type, argument_token::t_value);
        CHECK_EQ(arg_tokens.at(i + 1ull).value, init_arg_value(opt_arg_idx));

        ++opt_arg_idx;
    }

    free_argv(argc, argv);
}

// _parse_args_impl

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "_parse_args_impl should throw when there is a non-positional value specified "
    "without an argument flag present before the value"
) {
    add_arguments(n_positional_args, n_optional_args);

    auto arg_tokens = init_arg_tokens(n_positional_args, n_optional_args);
    arg_tokens.erase(std::next(arg_tokens.begin(), static_cast<std::ptrdiff_t>(n_positional_args)));

    CHECK_THROWS_AS(parse_args_impl(arg_tokens), ap::error::argument_deduction_failure);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "_parse_args_impl should not throw when the arguments are correct"
) {
    add_arguments(n_positional_args, n_optional_args);
    const auto arg_tokens = init_arg_tokens(n_positional_args, n_optional_args);
    CHECK_NOTHROW(parse_args_impl(arg_tokens));
}

// _get_argument

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "_get_argument should return nullopt if there is no argument with given name present"
) {
    add_arguments(n_positional_args, n_optional_args);
    CHECK_FALSE(get_argument(invalid_arg_name));
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "_get_argument should return valid argument if there is an argument with the given name"
) {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_positional_args; ++i) {
        const auto arg_name = init_arg_name(i);
        CHECK(get_argument(arg_name.primary));
        CHECK(get_argument(arg_name.secondary.value()));
    }
}

// parse_args

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when there is less input values than positional arguments"
) {
    add_arguments(n_positional_args, no_args);

    auto argc = get_argc(n_positional_args, no_args);
    auto argv_vec = init_argv_vec(n_positional_args, no_args);

    // remove the last positional value
    --argc;
    argv_vec.pop_back();

    auto argv = to_char_2d_array(argv_vec);

    CHECK_THROWS_AS(sut.parse_args(argc, argv), ap::error::required_argument_not_parsed);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when there is not enough positional values"
) {
    add_arguments(n_positional_args, n_optional_args);

    auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv_vec = init_argv_vec(n_positional_args, n_optional_args);

    // remove the last positional value
    --argc;
    argv_vec.erase(
        std::next(argv_vec.begin(), static_cast<std::ptrdiff_t>(n_positional_args - 1ull))
    );

    auto argv = to_char_2d_array(argv_vec);

    CHECK_THROWS_AS(sut.parse_args(argc, argv), ap::error::required_argument_not_parsed);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when there is no value specified for a required optional argument"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto required_arg_name = init_arg_name(n_args_total);
    sut.add_optional_argument(required_arg_name.primary, required_arg_name.secondary.value())
        .required();

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    CHECK_THROWS_AS(sut.parse_args(argc, argv), ap::error::required_argument_not_parsed);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when an optional argument's nvalues is not in a specified range"
) {
    add_arguments(n_positional_args, n_optional_args);

    auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    const auto range_arg_name = init_arg_name(n_args_total);
    sut.add_optional_argument(range_arg_name.primary, range_arg_name.secondary.value())
        .nargs(at_least(1ull));

    CHECK_THROWS_AS(sut.parse_args(argc, argv), ap::error::invalid_nvalues);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should not throw if the number of positional values are correct and all required "
    "optional arguments have values"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto required_arg_name = init_arg_name(n_args_total);
    sut.add_optional_argument(required_arg_name.primary, required_arg_name.secondary.value())
        .required();

    int argc;
    char** argv;

    SUBCASE("all arguments have values") {
        const auto n_optional_args_curr = n_optional_args + 1ull;
        argc = get_argc(n_positional_args, n_optional_args_curr);
        argv = init_argv(n_positional_args, n_optional_args_curr);
    }
    SUBCASE("only the necessary arguments have values") {
        const auto n_optional_args_curr = 1ull;
        argc = get_argc(n_positional_args, n_optional_args_curr);
        argv = init_argv(n_positional_args, n_optional_args_curr);

        std::strcpy(argv[argc - 2], init_arg_flag_primary(n_args_total).c_str());
        std::strcpy(argv[argc - 1], init_arg_value(n_args_total).c_str());
    }

    CAPTURE(argc);
    CAPTURE(argv);

    CHECK_NOTHROW(sut.parse_args(argc, argv));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should not throw if there is an argument which has the bypass_required option "
    "enabled and is used"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto bypass_required_arg_name = init_arg_name(n_args_total);
    sut.add_optional_argument<bool>(
           bypass_required_arg_name.primary, bypass_required_arg_name.secondary.value()
    )
        .default_value(false)
        .implicit_value(true)
        .bypass_required();

    std::string arg_flag;

    SUBCASE("primary flag") {
        arg_flag = init_arg_flag_primary(n_args_total);
    }
    SUBCASE("secondary flag") {
        arg_flag = init_arg_flag_secondary(n_args_total);
    }

    CAPTURE(arg_flag);

    std::vector<std::string> argv_vec{"program", arg_flag};
    const int argc = static_cast<int>(argv_vec.size());
    const auto argv = to_char_2d_array(argv_vec);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));
    CHECK(sut.value<bool>(bypass_required_arg_name.primary));

    free_argv(argc, argv);
}

// has_value

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "has_value should return false if there is no argument with given name present"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));
    CHECK_FALSE(sut.has_value(invalid_arg_name));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args, "has_value should return false when an argument has no values"
) {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        CHECK_FALSE(sut.has_value(arg_name.primary));
        CHECK_FALSE(sut.has_value(arg_name.secondary.value()));
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "has_value should return true if a value has been parsed for the argument"
) {
    add_arguments(n_positional_args, n_optional_args);

    const int argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        CHECK(sut.has_value(arg_name.primary));
        CHECK(sut.has_value(arg_name.secondary.value()));
    }

    free_argv(argc, argv);
}

// value

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value() should throw if there is no argument with given name present"
) {
    add_arguments(n_positional_args, n_optional_args);
    CHECK_THROWS_AS(sut.value(invalid_arg_name), ap::error::argument_not_found);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value() should throw if no value has been parsed for an argument"
) {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        CHECK_THROWS_AS(sut.value(arg_name.primary), std::logic_error);
        CHECK_THROWS_AS(sut.value(arg_name.secondary.value()), std::logic_error);
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value() should throw if an argument has a value but the given type is invalid"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    using invalid_value_type = int;

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);

        REQUIRE(sut.has_value(arg_name.primary));
        CHECK_THROWS_AS(
            sut.value<invalid_value_type>(arg_name.primary), ap::error::invalid_value_type
        );
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value() should return a correct value when the argument name is valid and its value has been "
    "parsed"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary));
        CHECK_EQ(sut.value(arg_name.primary), arg_value);
        CHECK_EQ(sut.value(arg_name.secondary.value()), arg_value);
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value() should return a correct value for an optional argument when the name is valid and the "
    "argument has a predefined value"
) {
    for (std::size_t i = 0ull; i < n_optional_args; ++i) {
        const auto arg_name = init_arg_name(i);
        sut.add_optional_argument(arg_name.primary, arg_name.secondary.value())
            .default_value(init_arg_value(i));
    }

    for (std::size_t i = 0ull; i < n_optional_args; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary));
        CHECK_EQ(sut.value(arg_name.primary), arg_value);
        CHECK_EQ(sut.value(arg_name.secondary.value()), arg_value);
    }
}

// value_or

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should throw if there is no argument with given name present"
) {
    add_arguments(n_positional_args, n_optional_args);
    CHECK_THROWS_AS(sut.value_or(invalid_arg_name, empty_str), ap::error::argument_not_found);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should throw if an argument has a value but the given type is invalid"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    using invalid_value_type = int;

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);

        REQUIRE(sut.has_value(arg_name.primary));
        CHECK_THROWS_AS(
            sut.value_or<invalid_value_type>(arg_name.primary, invalid_value_type{}),
            ap::error::invalid_value_type
        );
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should return a correct value when the argument name is valid and its value has "
    "been parsed"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary));
        CHECK_EQ(sut.value_or(arg_name.primary, empty_str), arg_value);
        CHECK_EQ(sut.value_or(arg_name.secondary.value(), empty_str), arg_value);
    }

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should return a correct value for an optional argument when the name is valid and "
    "the argument has a predefined value"
) {
    for (std::size_t i = 0ull; i < n_optional_args; ++i) {
        const auto arg_name = init_arg_name(i);
        sut.add_optional_argument(arg_name.primary, arg_name.secondary.value())
            .default_value(init_arg_value(i));
    }

    for (std::size_t i = 0ull; i < n_optional_args; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary));
        CHECK_EQ(sut.value_or(arg_name.primary, empty_str), arg_value);
        CHECK_EQ(sut.value_or(arg_name.secondary.value(), empty_str), arg_value);
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should return an instance of the specified type initialized from the given default "
    "value if no value has been parsed for an argument and it doesn't have a predefined value "
    "(optional)"
) {
    using value_type = int;
    using default_value_type = short;

    add_arguments<value_type>(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const default_value_type default_value = 2 * static_cast<default_value_type>(i);

        CHECK_EQ(sut.value_or<value_type>(arg_name.primary, default_value), default_value);
        CHECK_EQ(
            sut.value_or<value_type>(arg_name.secondary.value(), default_value), default_value
        );
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should return the given default value if no value has been parsed for an argument "
    "and it doesn't have a predefined value (optional)"
) {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto default_value = init_arg_value(i);

        CHECK_EQ(sut.value_or(arg_name.primary, default_value), default_value);
        CHECK_EQ(sut.value_or(arg_name.secondary.value(), default_value), default_value);
    }
}

// count

TEST_CASE_FIXTURE(test_argument_parser_parse_args, "count should return 0 by default") {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        CHECK_EQ(sut.count(arg_name.primary), 0ull);
        CHECK_EQ(sut.count(arg_name.secondary.value()), 0ull);
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "count should return 0 if there is no argument with given name present"
) {
    add_arguments(n_positional_args, n_optional_args);
    CHECK_EQ(sut.count(invalid_arg_name), 0ull);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args, "count should return the number of argument's flag usage"
) {
    // prepare sut
    sut.add_positional_argument(positional_primary_name, positional_secondary_name);
    sut.add_optional_argument(optional_primary_name, optional_secondary_name)
        .nargs(ap::nargs::any());

    // expected values
    const std::size_t positional_count = 1ull;
    const std::size_t optional_count = 4ull;

    // prepare argc and argv
    std::vector<std::string> argv_vec{"program", "positional_arg_value"};

    const std::string optional_arg_flag = "--" + optional_primary_name;
    const std::string optional_arg_value = optional_primary_name + "_value";
    for (std::size_t i = 0ull; i < optional_count; ++i) {
        argv_vec.push_back(optional_arg_flag);
        if (i % 2ull == 0)
            argv_vec.push_back(optional_arg_value);
    }

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    sut.parse_args(argc, argv);

    // test count
    CHECK_EQ(sut.count(positional_primary_name), positional_count);
    CHECK_EQ(sut.count(optional_primary_name), optional_count);

    // free argv
    free_argv(argc, argv);
}

// values

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() should throw when calling with a positional argument's name"
) {
    sut.add_positional_argument(positional_primary_name, positional_secondary_name);

    CHECK_THROWS_AS(sut.values(positional_primary_name), std::logic_error);
    CHECK_THROWS_AS(sut.values(positional_secondary_name), std::logic_error);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() should return an empty vector if an argument has no values"
) {
    sut.add_optional_argument(optional_primary_name, optional_secondary_name);

    SUBCASE("calling with argument's primary name") {
        const auto& values = sut.values(optional_primary_name);
        CHECK(values.empty());
    }

    SUBCASE("calling with argument's secondary name") {
        const auto& values = sut.values(optional_secondary_name);
        CHECK(values.empty());
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() should throw when an argument has values but the given type is invalid"
) {
    sut.add_optional_argument(optional_primary_name, optional_secondary_name).nargs(at_least(1));

    // prepare argc & argv
    const std::string optional_arg_flag = "--" + optional_primary_name;
    const std::string optional_arg_value = optional_primary_name + "_value";
    std::vector<std::string> argv_vec{
        "program", optional_arg_flag, optional_arg_value, optional_arg_value
    };

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    sut.parse_args(argc, argv);

    REQUIRE_THROWS_AS(
        sut.values<invalid_argument_value_type>(optional_primary_name),
        ap::error::invalid_value_type
    );
    REQUIRE_THROWS_AS(
        sut.values<invalid_argument_value_type>(optional_secondary_name),
        ap::error::invalid_value_type
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() should return a vector containing a predefined value if an optional argument if no "
    "values for an argument have been parsed"
) {
    const std::string default_value = "default_value";
    const std::string implicit_value = "implicit_value";

    sut.add_optional_argument(optional_primary_name, optional_secondary_name)
        .default_value(default_value)
        .implicit_value(implicit_value);

    // prepare argc & argv
    std::vector<std::string> argv_vec{"program"};
    std::string expected_value;

    SUBCASE("default_value") {
        expected_value = default_value;
    }

    SUBCASE("implicit_value") {
        expected_value = implicit_value;

        const auto optional_arg_flag = "--" + optional_primary_name;
        argv_vec.push_back(optional_arg_flag);
    }

    CAPTURE(expected_value);

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    sut.parse_args(argc, argv);

    const auto& stored_values = sut.values(optional_primary_name);

    REQUIRE_EQ(stored_values.size(), 1);
    CHECK_EQ(stored_values.front(), expected_value);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() should return a correct vector of values when there is an argument with "
    "a given name and has parsed values"
) {
    sut.add_optional_argument(optional_primary_name, optional_secondary_name).nargs(at_least(1));

    // prepare argc & argv
    const std::string optional_arg_flag = "--" + optional_primary_name;
    const std::string optional_arg_value = optional_primary_name + "_value";

    std::vector<std::string> optional_arg_values{
        optional_arg_value + "_1", optional_arg_value + "_2", optional_arg_value + "_3"
    };

    std::vector<std::string> argv_vec{"program", optional_arg_flag};
    for (const auto& value : optional_arg_values)
        argv_vec.push_back(value);

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    sut.parse_args(argc, argv);

    const auto& stored_values = sut.values(optional_primary_name);

    REQUIRE_EQ(stored_values.size(), optional_arg_values.size());
    for (std::size_t i = 0ull; i < stored_values.size(); ++i)
        CHECK_EQ(stored_values[i], optional_arg_values[i]);

    free_argv(argc, argv);
}

TEST_SUITE_END(); // test_argument_parser_parse_args
