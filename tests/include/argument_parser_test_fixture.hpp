#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::detail::argument_name;
using ap::detail::argument_token;

namespace ap_testing {

struct argument_parser_test_fixture {
    argument_parser_test_fixture() = default;
    virtual ~argument_parser_test_fixture() = default;

    using arg_token_list_t = ap::argument_parser::arg_token_list_t;
    using arg_opt_t = ap::argument_parser::arg_opt_t;

    using argument_value_type = std::string;
    using invalid_argument_value_type = int;

    // test utility functions
    [[nodiscard]] std::string init_arg_flag_primary(std::size_t i) const {
        return "--test_arg_" + std::to_string(i);
    }

    [[nodiscard]] std::string init_arg_flag_secondary(std::size_t i) const {
        return "-ta_" + std::to_string(i);
    }

    [[nodiscard]] argument_value_type init_arg_value(std::size_t i) const {
        return "test_value_" + std::to_string(i);
    }

    [[nodiscard]] std::size_t get_args_length(
        std::size_t n_positional_args, std::size_t n_optional_args
    ) const {
        return n_positional_args + (2ull * n_optional_args);
    }

    [[nodiscard]] int get_argc(std::size_t n_positional_args, std::size_t n_optional_args) const {
        return static_cast<int>(get_args_length(n_positional_args, n_optional_args) + 1);
    }

    [[nodiscard]] char** to_char_2d_array(const std::vector<std::string>& argv_vec) const {
        char** argv = new char*[argv_vec.size()];

        for (size_t i = 0; i < argv_vec.size(); ++i) {
            argv[i] = new char[argv_vec[i].size() + 1];
            std::strcpy(argv[i], argv_vec[i].c_str());
        }

        return argv;
    }

    [[nodiscard]] std::vector<std::string> init_argv_vec(
        std::size_t n_positional_args, std::size_t n_optional_args
    ) const {
        std::vector<std::string> argv_vec;
        argv_vec.reserve(get_argc(n_positional_args, n_optional_args));

        argv_vec.emplace_back("program");

        for (std::size_t i = 0ull; i < n_positional_args; ++i)
            argv_vec.emplace_back(init_arg_value(i));

        for (std::size_t i = 0ull; i < n_optional_args; ++i) {
            const auto arg_idx = n_positional_args + i;
            argv_vec.emplace_back(init_arg_flag_primary(arg_idx));
            argv_vec.emplace_back(init_arg_value(arg_idx));
        }

        return argv_vec;
    }

    [[nodiscard]] char** init_argv(std::size_t n_positional_args, std::size_t n_optional_args)
        const {
        return to_char_2d_array(init_argv_vec(n_positional_args, n_optional_args));
    }

    void free_argv(std::size_t argc, char** argv) const {
        for (std::size_t i = 0ull; i < argc; ++i)
            delete[] argv[i];
        delete[] argv;
    }

    [[nodiscard]] argument_name init_arg_name(std::size_t i) const {
        return argument_name("test_arg_" + std::to_string(i), "ta_" + std::to_string(i));
    }

    void add_arguments(
        ap::argument_parser& parser, std::size_t n_positional_args, std::size_t n_optional_args
    ) const {
        for (std::size_t i = 0ull; i < n_positional_args; ++i) {
            const auto arg_name = init_arg_name(i);
            parser.add_positional_argument(arg_name.primary, arg_name.secondary.value());
        }

        for (std::size_t i = 0ull; i < n_optional_args; ++i) {
            const auto arg_idx = n_positional_args + i;
            const auto arg_name = init_arg_name(arg_idx);
            parser.add_optional_argument(arg_name.primary, arg_name.secondary.value());
        }
    }

    [[nodiscard]] arg_token_list_t init_arg_tokens(
        std::size_t n_positional_args, std::size_t n_optional_args
    ) const {
        arg_token_list_t arg_tokens;
        arg_tokens.reserve(get_args_length(n_positional_args, n_optional_args));

        for (std::size_t i = 0ull; i < n_positional_args; ++i)
            arg_tokens.push_back(argument_token{argument_token::t_value, init_arg_value(i)});

        for (std::size_t i = 0ull; i < n_optional_args; ++i) {
            const auto arg_idx = n_positional_args + i;
            arg_tokens.push_back(
                argument_token{argument_token::t_flag, init_arg_name(arg_idx).primary}
            );
            arg_tokens.push_back(argument_token{argument_token::t_value, init_arg_value(arg_idx)});
        }

        return arg_tokens;
    }

    // argument_parser private function accessors
    [[nodiscard]] const std::optional<std::string>& get_program_name() const {
        return sut._program_name;
    }

    [[nodiscard]] const std::optional<std::string>& get_program_description() const {
        return sut._program_description;
    }

    [[nodiscard]] arg_token_list_t tokenize(int argc, char* argv[]) const {
        return sut._tokenize(std::span(argv + 1, argc - 1));
    }

    void parse_args_impl(const arg_token_list_t& arg_tokens) {
        sut._parse_args_impl(arg_tokens);
    }

    [[nodiscard]] arg_opt_t get_argument(std::string_view arg_name) const {
        return sut._get_argument(arg_name);
    }

    ap::argument_parser sut;
};

} // namespace ap_testing
