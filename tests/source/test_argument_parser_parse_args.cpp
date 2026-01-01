#include "argument_parser_test_fixture.hpp"
#include "doctest.h"
#include "utility.hpp"

using namespace argon_testing;
using namespace argon::nargs;
using argon::invalid_configuration;
using argon::parsing_failure;
using argon::unknown_policy;

TEST_SUITE_BEGIN("test_argument_parser_parse_args");

struct test_argument_parser_parse_args : public argument_parser_test_fixture {
    const std::string_view test_program_name = "test program name";

    const std::size_t no_args = 0ull;
    const std::size_t n_positional_args = 3ull;
    const std::size_t n_optional_args = n_positional_args;
    const std::size_t n_args_total = n_positional_args + n_optional_args;
    const std::size_t last_pos_arg_idx = n_positional_args - 1ull;
    const std::size_t first_opt_arg_idx = n_positional_args;

    const std::string invalid_arg_name = "invalid_arg";

    const std::string positional_name = "positional_arg";
    const std::string optional_primary_name = "optional_arg";
    const std::string optional_secondary_name = "oa";

    const std::optional<std::string> optional_primary_name_opt = optional_primary_name;
    const std::optional<std::string> optional_secondary_name_opt = optional_secondary_name;

    const char flag_char = '-';

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
        REQUIRE_EQ(arg_tokens.at(i).type, argument_token::t_flag_primary);
        const auto stripped_flag = strip_flag_prefix(arg_tokens.at(i));
        CHECK(init_arg_name(opt_arg_idx).match(stripped_flag));

        REQUIRE_EQ(arg_tokens.at(i + 1ull).type, argument_token::t_value);
        CHECK_EQ(arg_tokens.at(i + 1ull).value, init_arg_value(opt_arg_idx));

        ++opt_arg_idx;
    }

    free_argv(argc, argv);
}

// _parse_args_impl

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "_parse_args_impl should treat non-positional values specified without an argument flag "
    "present before as unknown arguments"
) {
    add_arguments(n_positional_args, n_optional_args);

    auto argv_vec = init_argv_vec(n_positional_args, n_optional_args, false);
    argv_vec.erase(std::next(argv_vec.begin(), static_cast<std::ptrdiff_t>(first_opt_arg_idx)));

    CHECK_NOTHROW(parse_args_impl(argv_vec.begin(), argv_vec.end()));
    CHECK_NE(
        std::ranges::find(state.unknown_args, init_arg_value(first_opt_arg_idx)),
        state.unknown_args.end()
    );
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "_parse_args_impl should not detect any unknown arguments for correct command-line argument "
    "list"
) {
    add_arguments(n_positional_args, n_optional_args);
    auto argv_vec = init_argv_vec(n_positional_args, n_optional_args, false);

    CHECK_NOTHROW(parse_args_impl(argv_vec.begin(), argv_vec.end()));
    CHECK(state.unknown_args.empty());
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

    for (std::size_t i = 0ull; i < n_positional_args; ++i)
        CHECK(get_argument(init_arg_name_primary(i)));

    for (std::size_t i = first_opt_arg_idx; i < n_args_total; ++i) {
        CHECK(get_argument(init_arg_name_primary(i)));
        CHECK(get_argument(init_arg_name_secondary(i)));
    }
}

// parse_args

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when there is not enough positional values"
) {
    add_arguments(n_positional_args, n_optional_args);

    auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv_vec = init_argv_vec(n_positional_args, n_optional_args);

    // remove the last positional value
    --argc;
    argv_vec.erase(std::next(argv_vec.begin(), static_cast<std::ptrdiff_t>(last_pos_arg_idx)));

    auto argv = to_char_2d_array(argv_vec);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        required_argument_not_parsed_msg({init_arg_name_primary(last_pos_arg_idx)}).c_str(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw if a required positional argument is defined after a non-required "
    "posisitonal argument"
) {
    const auto non_required_arg_name = init_arg_name_primary(0ull);
    sut.add_positional_argument(non_required_arg_name).required(false);

    const auto required_arg_name = init_arg_name_primary(1ull);
    sut.add_positional_argument(required_arg_name);

    const auto argc = get_argc(no_args, no_args);
    auto argv = init_argv(no_args, no_args);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        std::format(
            "Required positional argument [{}] cannot be defined after a non-required positional "
            "argument [{}].",
            required_arg_name,
            non_required_arg_name
        )
            .c_str(),
        invalid_configuration
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args, "parse_args should throw if unknown arguments are detected"
) {
    add_optional_args(n_optional_args, n_positional_args, [](auto& opt_arg) {
        opt_arg.nargs(0ull);
    });

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    std::vector<std::string> unknown_args;
    unknown_args.reserve(n_args_total);
    for (std::size_t i = 0ull; i < n_args_total; ++i)
        unknown_args.emplace_back(init_arg_value(i));

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        std::format(
            "Failed to deduce the argument for values [{}]", argon::util::join(unknown_args)
        )
            .c_str(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when there is no value specified for a required optional argument"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto required_arg_name = init_arg_name(n_args_total, flag_char);
    sut.add_optional_argument(
           required_arg_name.primary.value(), required_arg_name.secondary.value()
    )
        .required();

    const auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        required_argument_not_parsed_msg(required_arg_name).c_str(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when an optional argument's nvalues is not in a specified range"
) {
    add_arguments(n_positional_args, n_optional_args);

    auto argc = get_argc(n_positional_args, n_optional_args);
    auto argv = init_argv(n_positional_args, n_optional_args);

    const auto range_arg_name = init_arg_name(n_args_total, flag_char);
    sut.add_optional_argument(range_arg_name.primary.value(), range_arg_name.secondary.value())
        .nargs(at_least(1ull));

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        parsing_failure::invalid_nvalues(range_arg_name, std::weak_ordering::less).what(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when an unrecognized argument flag is used"
) {
    add_arguments(no_args, no_args);

    constexpr std::size_t n_opt_clargs = 1ull;
    constexpr std::size_t opt_arg_idx = 0ull;

    auto argc = get_argc(no_args, n_opt_clargs);
    auto argv = init_argv(no_args, n_opt_clargs);

    const auto unknown_arg_flag = init_arg_flag_primary(opt_arg_idx);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        parsing_failure::unknown_argument(unknown_arg_flag).what(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when an optional argument's flag uses the correct name but an "
    "incorrect flag prefix"
) {
    constexpr std::size_t n_opt_args = 1ull;
    constexpr std::size_t opt_arg_idx = 0ull;
    add_arguments(no_args, n_opt_args);

    auto argc = get_argc(no_args, n_opt_args);
    auto argv_vec = init_argv_vec(no_args, n_opt_args);

    const auto opt_arg_name = init_arg_name(opt_arg_idx);

    std::string invalid_flag;

    SUBCASE("primary name with a secondary flag prefix") {
        invalid_flag = "-" + opt_arg_name.primary.value();
    }
    SUBCASE("secondary name with a primary flag prefix") {
        invalid_flag = "--" + opt_arg_name.secondary.value();
    }

    CAPTURE(invalid_flag);

    auto arg_flag_it = std::ranges::find(argv_vec, init_arg_flag_primary(opt_arg_idx));
    if (arg_flag_it == argv_vec.end())
        FAIL("could not find the optional argument flag");

    *arg_flag_it = invalid_flag;
    auto argv = to_char_2d_array(argv_vec);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        parsing_failure::unknown_argument(invalid_flag).what(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should not throw if the number of positional values are correct and all required "
    "optional arguments have values"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto required_arg_name = init_arg_name(n_args_total);
    sut.add_optional_argument(
           required_arg_name.primary.value(), required_arg_name.secondary.value()
    )
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

        auto argv_vec = init_argv_vec(n_positional_args, n_optional_args_curr);
        argv_vec[static_cast<std::size_t>(argc - 2)] = init_arg_flag_primary(n_args_total).c_str();
        argv_vec[static_cast<std::size_t>(argc - 1)] = init_arg_value(n_args_total).c_str();

        argv = to_char_2d_array(argv_vec);
    }

    CAPTURE(argc);
    CAPTURE(argv);

    CHECK_NOTHROW(sut.parse_args(argc, argv));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should not throw if there is a positional argument which has the "
    "suppress_arg_checks "
    "option enabled and is used"
) {
    const std::size_t n_positional_args = 1ull;
    const auto bypass_required_arg_name = init_arg_name(n_positional_args - 1ull).primary.value();
    sut.add_positional_argument(bypass_required_arg_name).required(false).suppress_arg_checks();
    const std::string bypass_required_arg_value = "bypass_required_arg_value";

    for (std::size_t i = 0ull; i < n_optional_args; ++i)
        sut.add_optional_argument(init_arg_name(n_positional_args + i).primary.value()).required();

    std::vector<std::string> argv_vec{"program", bypass_required_arg_value};
    const int argc = static_cast<int>(argv_vec.size());
    const auto argv = to_char_2d_array(argv_vec);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));
    CHECK_EQ(sut.value(bypass_required_arg_name), bypass_required_arg_value);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should not throw if there is an optional argument which has the "
    "suppress_arg_checks "
    "option enabled and is used"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto bypass_required_arg_name = init_arg_name(n_args_total);
    sut.add_optional_argument<bool>(
           bypass_required_arg_name.primary.value(), bypass_required_arg_name.secondary.value()
    )
        .default_values(false)
        .implicit_values(true)
        .suppress_arg_checks();

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
    CHECK(sut.value<bool>(bypass_required_arg_name.primary.value()));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args, "parse_args should consume free values at various positions"
) {
    add_positional_args(n_positional_args);
    add_optional_args(n_optional_args, n_positional_args, [](auto& arg) { arg.nargs(1ull); });

    std::vector<std::string> argv_vec{"program"};

    SUBCASE("at the beginning") {
        for (std::size_t i = 0ull; i < n_positional_args; ++i)
            argv_vec.emplace_back(init_arg_value(i));

        for (std::size_t i = n_positional_args; i < n_args_total; ++i) {
            argv_vec.emplace_back(init_arg_flag_primary(i));
            argv_vec.emplace_back(init_arg_value(i));
        }
    }

    SUBCASE("mixed with optional args (front)") {
        for (std::size_t i = 0ull; i < n_positional_args; ++i) {
            argv_vec.emplace_back(init_arg_value(i));
            argv_vec.emplace_back(init_arg_flag_primary(i + n_positional_args));
            argv_vec.emplace_back(init_arg_value(i + n_positional_args));
        }
    }

    SUBCASE("mixed with optional args (back)") {
        for (std::size_t i = 0ull; i < n_positional_args; ++i) {
            argv_vec.emplace_back(init_arg_flag_primary(i + n_positional_args));
            argv_vec.emplace_back(init_arg_value(i + n_positional_args));
            argv_vec.emplace_back(init_arg_value(i));
        }
    }

    SUBCASE("at the end") {
        for (std::size_t i = n_positional_args; i < n_args_total; ++i) {
            argv_vec.emplace_back(init_arg_flag_primary(i));
            argv_vec.emplace_back(init_arg_value(i));
        }

        for (std::size_t i = 0ull; i < n_positional_args; ++i)
            argv_vec.emplace_back(init_arg_value(i));
    }

    CAPTURE(argv_vec);

    const auto argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    sut.parse_args(argc, argv);

    for (std::size_t i = 0ull; i < n_positional_args; ++i) {
        const auto arg_name = init_arg_name_primary(i);
        REQUIRE(sut.has_value(arg_name));
        CHECK_EQ(sut.value(arg_name), init_arg_value(i));
    }

    free_argv(argc, argv);
}

// parse_known_args

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_known_args should return an empty vector for a correct command-line argument list"
) {
    add_arguments(n_positional_args, n_optional_args);

    const auto required_arg_name = init_arg_name(n_args_total);
    sut.add_optional_argument(
           required_arg_name.primary.value(), required_arg_name.secondary.value()
    )
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

        auto argv_vec = init_argv_vec(n_positional_args, n_optional_args_curr);
        argv_vec[static_cast<std::size_t>(argc - 2)] = init_arg_flag_primary(n_args_total).c_str();
        argv_vec[static_cast<std::size_t>(argc - 1)] = init_arg_value(n_args_total).c_str();

        argv = to_char_2d_array(argv_vec);
    }

    CAPTURE(argc);
    CAPTURE(argv);

    std::vector<std::string> unknown_args;
    CHECK_NOTHROW(unknown_args = sut.parse_known_args(argc, argv));
    CHECK(unknown_args.empty());

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_known_args should return a list of unknown command-line arguments"
) {
    add_optional_args(n_optional_args, n_positional_args, [](auto& opt_arg) {
        opt_arg.nargs(0ull);
    });

    const auto argc = get_argc(n_positional_args, n_optional_args + 1ull);
    const auto argv_vec = init_argv_vec(n_positional_args, n_optional_args + 1ull);
    auto argv = to_char_2d_array(argv_vec);

    std::vector<std::string> expected_unknown_args;
    expected_unknown_args.reserve(n_args_total + 2ull);
    for (std::size_t i = 0ull; i < n_args_total; ++i)
        expected_unknown_args.emplace_back(init_arg_value(i));

    // unrecognized argument flags should also be treated as unknown arguments
    const std::size_t unknown_opt_arg_idx = n_args_total;
    expected_unknown_args.emplace_back(init_arg_flag_primary(unknown_opt_arg_idx));
    expected_unknown_args.emplace_back(init_arg_value(unknown_opt_arg_idx));

    std::vector<std::string> unknown_args;
    CHECK_NOTHROW(unknown_args = sut.parse_known_args(argc, argv));
    CHECK_EQ(unknown_args, expected_unknown_args);

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
        CHECK_FALSE(sut.has_value(arg_name.primary.value()));
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

    for (std::size_t i = 0ull; i < n_positional_args; ++i)
        CHECK(sut.has_value(init_arg_name_primary(i)));

    for (std::size_t i = first_opt_arg_idx; i < n_args_total; ++i) {
        CHECK(sut.has_value(init_arg_name_primary(i)));
        CHECK(sut.has_value(init_arg_name_secondary(i)));
    }

    free_argv(argc, argv);
}

// count

TEST_CASE_FIXTURE(test_argument_parser_parse_args, "count should return 0 by default") {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        CHECK_EQ(sut.count(arg_name.primary.value()), 0ull);
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
    sut.add_positional_argument(positional_name);
    sut.add_optional_argument(optional_primary_name, optional_secondary_name)
        .nargs(argon::nargs::any());

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
    CHECK_EQ(sut.count(positional_name), positional_count);
    CHECK_EQ(sut.count(optional_primary_name), optional_count);

    // free argv
    free_argv(argc, argv);
}

// value

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value() should throw if there is no argument with given name present"
) {
    add_arguments(n_positional_args, n_optional_args);
    CHECK_THROWS_AS(discard_result(sut.value(invalid_arg_name)), argon::lookup_failure);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value() should throw if no value has been parsed for an argument"
) {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_positional_args; ++i)
        CHECK_THROWS_AS(discard_result(sut.value(init_arg_name_primary(i))), std::logic_error);

    for (std::size_t i = first_opt_arg_idx; i < n_args_total; ++i) {
        CHECK_THROWS_AS(discard_result(sut.value(init_arg_name_primary(i))), std::logic_error);
        CHECK_THROWS_AS(discard_result(sut.value(init_arg_name_secondary(i))), std::logic_error);
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

        REQUIRE(sut.has_value(arg_name.primary.value()));
        CHECK_THROWS_AS(
            discard_result(sut.value<invalid_value_type>(arg_name.primary.value())),
            argon::type_error
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

    for (std::size_t i = 0ull; i < n_positional_args; ++i) {
        const auto arg_name = init_arg_name_primary(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name));
        CHECK_EQ(sut.value(arg_name), arg_value);
    }

    for (std::size_t i = first_opt_arg_idx; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary.value()));
        CHECK_EQ(sut.value(arg_name.primary.value()), arg_value);
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
        sut.add_optional_argument(arg_name.primary.value(), arg_name.secondary.value())
            .default_values(init_arg_value(i));
    }

    for (std::size_t i = 0ull; i < n_optional_args; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary.value()));
        CHECK_EQ(sut.value(arg_name.primary.value()), arg_value);
        CHECK_EQ(sut.value(arg_name.secondary.value()), arg_value);
    }
}

// value_or

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should throw if there is no argument with given name present"
) {
    add_arguments(n_positional_args, n_optional_args);
    CHECK_THROWS_AS(
        discard_result(sut.value_or(invalid_arg_name, empty_str)), argon::lookup_failure
    );
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

        REQUIRE(sut.has_value(arg_name.primary.value()));
        CHECK_THROWS_AS(
            discard_result(
                sut.value_or<invalid_value_type>(arg_name.primary.value(), invalid_value_type{})
            ),
            argon::type_error
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

    for (std::size_t i = 0ull; i < n_positional_args; ++i) {
        const auto arg_name = init_arg_name_primary(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name));
        CHECK_EQ(sut.value_or(arg_name, empty_str), arg_value);
    }

    for (std::size_t i = first_opt_arg_idx; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary.value()));
        CHECK_EQ(sut.value_or(arg_name.primary.value(), empty_str), arg_value);
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
        sut.add_optional_argument(arg_name.primary.value(), arg_name.secondary.value())
            .default_values(init_arg_value(i));
    }

    for (std::size_t i = 0ull; i < n_optional_args; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto arg_value = init_arg_value(i);

        REQUIRE(sut.has_value(arg_name.primary.value()));
        CHECK_EQ(sut.value_or(arg_name.primary.value(), empty_str), arg_value);
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

    for (std::size_t i = 0ull; i < n_positional_args; ++i) {
        const default_value_type fallback_value = 2 * static_cast<default_value_type>(i);
        CHECK_EQ(
            sut.value_or<value_type>(init_arg_name_primary(i), fallback_value), fallback_value
        );
    }

    for (std::size_t i = first_opt_arg_idx; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const default_value_type fallback_value = 2 * static_cast<default_value_type>(i);

        CHECK_EQ(
            sut.value_or<value_type>(arg_name.primary.value(), fallback_value), fallback_value
        );
        CHECK_EQ(
            sut.value_or<value_type>(arg_name.secondary.value(), fallback_value), fallback_value
        );
    }
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "value_or() should return the given default value if no value has been parsed for an argument "
    "and it doesn't have a predefined value (optional)"
) {
    add_arguments(n_positional_args, n_optional_args);

    for (std::size_t i = 0ull; i < n_positional_args; ++i) {
        const auto fallback_value = init_arg_value(i);
        CHECK_EQ(sut.value_or(init_arg_name_primary(i), fallback_value), fallback_value);
    }

    for (std::size_t i = first_opt_arg_idx; i < n_args_total; ++i) {
        const auto arg_name = init_arg_name(i);
        const auto fallback_value = init_arg_value(i);

        CHECK_EQ(sut.value_or(arg_name.primary.value(), fallback_value), fallback_value);
        CHECK_EQ(sut.value_or(arg_name.secondary.value(), fallback_value), fallback_value);
    }
}

// values: positional arguments

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [positional arguments] should return an empty vector if an argument has no values"
) {
    sut.add_positional_argument(positional_name);
    CHECK(sut.values(positional_name).empty());
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [positional arguments] should throw when an argument has values but the given type "
    "is invalid"
) {
    sut.add_positional_argument(positional_name);

    // prepare argc & argv
    const int argc = get_argc(1ull, no_args);
    auto argv = init_argv(1ull, no_args);

    // parse args
    sut.parse_args(argc, argv);

    CHECK_THROWS_AS(
        discard_result(sut.values<invalid_argument_value_type>(positional_name)), argon::type_error
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [positional arguments] should return a vector containing the predefined values of an "
    "argument if no values for an argument have been parsed"
) {
    const std::vector<std::string> default_values{"default_value_1", "default_value_2"};

    sut.add_positional_argument(positional_name).default_values(default_values);

    // prepare argc & argv
    const int argc = get_argc(no_args, no_args);
    auto argv = init_argv(no_args, no_args);

    // parse args
    sut.parse_args(argc, argv);

    const auto& stored_values = sut.values(positional_name);

    REQUIRE_EQ(stored_values.size(), default_values.size());
    CHECK_EQ(stored_values, default_values);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [positional arguments] should return a correct vector of values when there is an "
    "argument with a given name and has parsed values"
) {
    sut.add_positional_argument(positional_name).nargs(any());

    // prepare argc & argv
    const std::size_t n_positional_values = 3ull;
    std::vector<std::string> positional_arg_values;
    for (std::size_t i = 0ull; i < n_positional_values; ++i)
        positional_arg_values.emplace_back(std::format("positional_value_{}", i + 1ull));

    std::vector<std::string> argv_vec{"program"};
    argv_vec.insert(argv_vec.end(), positional_arg_values.begin(), positional_arg_values.end());

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    sut.parse_args(argc, argv);

    const auto& stored_values = sut.values(positional_name);

    REQUIRE_EQ(stored_values.size(), positional_arg_values.size());
    CHECK_EQ(stored_values, positional_arg_values);

    free_argv(argc, argv);
}

// values: optional arguments

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [optional arguments] should return an empty vector if an argument has no values"
) {
    sut.add_optional_argument(optional_primary_name, optional_secondary_name);

    CHECK(sut.values(optional_primary_name).empty());
    CHECK(sut.values(optional_secondary_name).empty());
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [optional arguments] should throw when an argument has values but the given type is "
    "invalid"
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

    CHECK_THROWS_AS(
        discard_result(sut.values<invalid_argument_value_type>(optional_primary_name)),
        argon::type_error
    );
    CHECK_THROWS_AS(
        discard_result(sut.values<invalid_argument_value_type>(optional_secondary_name)),
        argon::type_error
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [optional arguments] should return a vector containing the predefined values of an "
    "argument if no values for an argument have been parsed"
) {
    const std::vector<std::string> default_values{"default_value_1", "default_value_2"};
    const std::vector<std::string> implicit_values{"implicit_value_1", "implicit_value_2"};

    sut.add_optional_argument(optional_primary_name, optional_secondary_name)
        .default_values(default_values)
        .implicit_values(implicit_values);

    // prepare argc & argv
    std::vector<std::string> argv_vec{"program"};
    std::vector<std::string> expected_values;

    SUBCASE("default_values") {
        expected_values = default_values;
    }

    SUBCASE("implicit_values") {
        expected_values = implicit_values;

        const auto optional_arg_flag = "--" + optional_primary_name;
        argv_vec.push_back(optional_arg_flag);
    }

    CAPTURE(argv_vec);
    CAPTURE(expected_values);

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    sut.parse_args(argc, argv);

    const auto& stored_values = sut.values(optional_primary_name);

    REQUIRE_EQ(stored_values.size(), expected_values.size());
    CHECK_EQ(stored_values, expected_values);

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "values() [optional arguments] should return a correct vector of values when there is an "
    "argument with a given name and has parsed values"
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
    CHECK_EQ(stored_values, optional_arg_values);

    free_argv(argc, argv);
}

// compound flags

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "argument_parser should throw if an invalid compound flag is used"
) {
    // add arguments
    sut.add_optional_argument("first-arg", "f");
    sut.add_optional_argument("second-arg", "s");
    sut.add_optional_argument("third-arg", "t");

    const std::string invalid_flag = "-abc";
    std::vector<std::string> argv_vec{"program", invalid_flag};

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        parsing_failure::unknown_argument(invalid_flag).what(),
        parsing_failure
    );

    // validate argument usage counts
    CHECK_EQ(sut.count("first-arg"), 0ull);
    CHECK_EQ(sut.count("second-arg"), 0ull);
    CHECK_EQ(sut.count("third-arg"), 0ull);

    // cleanup
    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args, "argument_parser should properly handle valid compound flags"
) {
    // add arguments
    sut.add_optional_argument("first-arg", "f");
    sut.add_optional_argument("second-arg", "s");
    sut.add_optional_argument("third-arg", "t");

    std::size_t first_arg_count, second_arg_count, third_arg_count;
    std::string compound_flag;

    // prepare argc & argv
    SUBCASE("one usage of each argument") {
        compound_flag = "-fst";
        first_arg_count = second_arg_count = third_arg_count = 1ull;
    }
    SUBCASE("complex usage") {
        compound_flag = "-ffstfst";
        first_arg_count = 3ull;
        second_arg_count = third_arg_count = 2ull;
    }

    CAPTURE(first_arg_count);
    CAPTURE(second_arg_count);
    CAPTURE(third_arg_count);
    CAPTURE(compound_flag);

    std::vector<std::string> argv_vec{"program", compound_flag};

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // parse args
    sut.parse_args(argc, argv);

    // validate argument usage counts
    CHECK_EQ(sut.count("first-arg"), first_arg_count);
    CHECK_EQ(sut.count("second-arg"), second_arg_count);
    CHECK_EQ(sut.count("third-arg"), third_arg_count);

    // cleanup
    free_argv(argc, argv);
}

// greedy arguments

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "greedy arguments should consume all command-line values until their upper nargs bound is "
    "reached regardless of the tokens' types"
) {
    const std::string greedy_arg_name = "greedy";
    const std::string actual_value = "value";

    const std::size_t n_greedily_consumed_values = 3ull;
    const std::size_t greedy_nargs_bound = 1ull + n_greedily_consumed_values;
    const std::size_t opt_begin_idx = 0ull;

    std::vector<std::string> argv_vec{"program"};

    SUBCASE("greedy positional argument") {
        sut.add_positional_argument(greedy_arg_name).greedy().nargs(greedy_nargs_bound);
        argv_vec.emplace_back(actual_value);
    }
    SUBCASE("greedy optional arg") {
        sut.add_optional_argument(greedy_arg_name).greedy().nargs(greedy_nargs_bound);
        argv_vec.emplace_back(std::format("--{}", greedy_arg_name));
        argv_vec.emplace_back(actual_value);
    }

    CAPTURE(sut);
    CAPTURE(argv_vec);

    add_optional_args(n_greedily_consumed_values, opt_begin_idx);

    // add the greedily consumed flags
    std::vector<std::string> expected_greedy_arg_values{actual_value};
    for (std::size_t i = opt_begin_idx; i < n_greedily_consumed_values; ++i) {
        expected_greedy_arg_values.emplace_back(init_arg_flag_primary(i));
        argv_vec.emplace_back(init_arg_flag_primary(i));
    }

    // add the normally parsed flags
    const std::size_t expected_opt_count = 1ull;
    for (std::size_t i = opt_begin_idx; i < n_greedily_consumed_values; ++i)
        argv_vec.emplace_back(init_arg_flag_primary(i));

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    CHECK_EQ(sut.values(greedy_arg_name), expected_greedy_arg_values);
    for (std::size_t i = opt_begin_idx; i < n_greedily_consumed_values; ++i)
        CHECK_EQ(sut.count(init_arg_name_primary(i)), expected_opt_count);

    free_argv(argc, argv);
}

// unknown_arguments_policy

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when an unrecognized argument flag is used with the default unknown "
    "arguments handling policy (fail)"
) {
    add_arguments(no_args, no_args);

    constexpr std::size_t n_opt_clargs = 1ull;
    constexpr std::size_t opt_arg_idx = 0ull;

    auto argc = get_argc(no_args, n_opt_clargs);
    auto argv = init_argv(no_args, n_opt_clargs);

    const auto unknown_arg_name = init_arg_flag_primary(opt_arg_idx);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        parsing_failure::unknown_argument(unknown_arg_name).what(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when an unrecognized argument flag is used with the default unknown "
    "arguments handling policy (fail)"
) {
    const auto unknown_arg_flag = "--unknown";
    const std::vector<std::string> argv_vec{"program", unknown_arg_flag};

    const auto argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        parsing_failure::unknown_argument(unknown_arg_flag).what(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should print a warning to std::cerr when an unrecognized argument flag is used "
    "with the warn unknown arguments handling policy"
) {
    sut.unknown_arguments_policy(unknown_policy::warn);

    const auto unknown_arg_flag = "--unknown";
    const std::vector<std::string> argv_vec{"program", unknown_arg_flag};

    const auto argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // redirect std::cerr
    std::stringstream tmp_buffer;
    auto* cerr_buffer = std::cerr.rdbuf(tmp_buffer.rdbuf());

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    CHECK_EQ(
        tmp_buffer.str(),
        std::format("[argon::warning] Unknown argument '{}' will be ignored.\n", unknown_arg_flag)
    );

    free_argv(argc, argv);

    // reset std::cerr
    std::cerr.rdbuf(cerr_buffer);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should do nothing when an unrecognized argument flag is used with the ignore "
    "unknown arguments handling policy"
) {
    sut.unknown_arguments_policy(unknown_policy::ignore);

    const auto unknown_arg_flag = "--unknown";
    const std::vector<std::string> argv_vec{"program", unknown_arg_flag};

    const auto argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    // redirect std::cerr
    std::stringstream tmp_buffer;
    auto* cerr_buffer = std::cerr.rdbuf(tmp_buffer.rdbuf());

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));

    CHECK_EQ(tmp_buffer.str(), "");

    free_argv(argc, argv);

    // reset std::cerr
    std::cerr.rdbuf(cerr_buffer);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should treat an unrecognized argument flag as a value with the as_values unknown "
    "arguments handling policy"
) {
    sut.unknown_arguments_policy(unknown_policy::as_values);
    sut.add_positional_argument("known");

    const auto unknown_arg_flag = "--unknown";
    const std::vector<std::string> argv_vec{"program", unknown_arg_flag};

    const auto argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    CHECK_NOTHROW(sut.parse_args(argc, argv));
    CHECK_EQ(sut.value("known"), unknown_arg_flag);

    free_argv(argc, argv);
}

// argument groups

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw if no arguments from a required group are used"
) {
    const std::string req_gr_name = "Required Group";
    auto& req_gr = sut.add_group(req_gr_name).required();

    for (std::size_t i = 0ull; i < n_optional_args; ++i)
        sut.add_optional_argument(req_gr, init_arg_name_primary(i));

    const int argc = get_argc(no_args, no_args);
    auto argv = init_argv(no_args, no_args);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        std::format("At least one argument from the required group '{}' must be used", req_gr_name)
            .c_str(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should throw when multiple arguments from a mutually exclusive group are used"
) {
    const std::string me_gr_name = "Mutually Exclusive Group";
    auto& me_gr = sut.add_group(me_gr_name).mutually_exclusive();

    for (std::size_t i = 0ull; i < n_optional_args; ++i)
        sut.add_optional_argument(me_gr, init_arg_name_primary(i));

    const int argc = get_argc(no_args, n_optional_args);
    auto argv = init_argv(no_args, n_optional_args);

    CHECK_THROWS_WITH_AS(
        sut.parse_args(argc, argv),
        std::format(
            "At most one argument from the mutually exclusive group '{}' can be used", me_gr_name
        )
            .c_str(),
        parsing_failure
    );

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should not throw when the group requirements are satisfied"
) {
    const std::string req_me_gr_name = "Required & Mutually Exclusive Group";
    auto& req_me_gr = sut.add_group(req_me_gr_name).mutually_exclusive();

    for (std::size_t i = 0ull; i < n_optional_args; ++i)
        sut.add_optional_argument(req_me_gr, init_arg_name_primary(i));

    // required group: >= 1 argument must be used
    // mutually exclusive group: <= 1 argument can be used
    // together: exactly one argument can be used
    std::size_t used_arg_idx;
    SUBCASE("used arg = 0") {
        used_arg_idx = 0;
    }
    SUBCASE("used arg = 1") {
        used_arg_idx = 1;
    }
    SUBCASE("used arg = 2") {
        used_arg_idx = 2;
    }
    CAPTURE(used_arg_idx);

    std::vector<std::string> argv_vec = {
        "program", init_arg_flag_primary(used_arg_idx), init_arg_value(used_arg_idx)
    };

    const int argc = static_cast<int>(argv_vec.size());
    auto argv = to_char_2d_array(argv_vec);

    REQUIRE_NOTHROW(sut.parse_args(argc, argv));
    CHECK(sut.count(init_arg_name_primary(used_arg_idx)));

    free_argv(argc, argv);
}

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "parse_args should not throw when the group requirements are not satisfied but a group check "
    "suppressing argument is used"
) {
    const std::string suppressing_arg_name = "suppress";
    sut.add_flag(suppressing_arg_name).suppress_group_checks();

    const std::string req_me_gr_name = "Required & Mutually Exclusive Group";
    auto& req_me_gr = sut.add_group(req_me_gr_name).mutually_exclusive();

    for (std::size_t i = 0ull; i < n_optional_args; ++i)
        sut.add_optional_argument(req_me_gr, init_arg_name_primary(i));

    std::vector<std::string> argv_vec{std::format("--{}", suppressing_arg_name)};

    REQUIRE_NOTHROW(sut.parse_args(argv_vec));
    CHECK(sut.value<bool>(suppressing_arg_name));
    for (std::size_t i = 0ull; i < n_optional_args; ++i)
        CHECK_FALSE(sut.is_used(init_arg_name_primary(i)));
}

// subparsers

TEST_CASE_FIXTURE(
    test_argument_parser_parse_args,
    "an argument parser should delegate parsing remaining arguments if the first command-line "
    "argument matches a subparser's name"
) {
    const std::string subparser_name = "subprogram";
    auto& subparser = sut.add_subparser(subparser_name);

    const std::string pos_arg_name = "positional";
    const std::string pos_arg_val = "positional-value";
    const std::string opt_arg_name = "optional";
    const std::string opt_arg_val = "optional-value";

    subparser.add_positional_argument(pos_arg_name);
    subparser.add_optional_argument(opt_arg_name);

    const std::vector<std::string> argv_vec{
        subparser_name, pos_arg_val, std::format("--{}", opt_arg_name), opt_arg_val
    };

    sut.parse_args(argv_vec);

    REQUIRE(sut.invoked());
    CHECK_FALSE(sut.finalized());

    REQUIRE(subparser.invoked());
    CHECK(subparser.finalized());
    CHECK_EQ(&subparser, &sut.resolved_parser());
    CHECK_EQ(subparser.value(pos_arg_name), pos_arg_val);
    CHECK_EQ(subparser.value(opt_arg_name), opt_arg_val);
}

TEST_SUITE_END(); // test_argument_parser_parse_args;
