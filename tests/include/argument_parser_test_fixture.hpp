#pragma once

#include <ap/argument_parser.hpp>

#include <cstring>
#include <format>

using ap::optional_argument;
using ap::positional_argument;
using ap::detail::argument_name;
using ap::detail::argument_token;
using ap::detail::c_argument_value_type;

namespace ap_testing {

struct argument_parser_test_fixture {
    argument_parser_test_fixture() = default;
    virtual ~argument_parser_test_fixture() = default;

    using arg_ptr_t = ap::argument_parser::arg_ptr_t;
    using arg_token_list_t = ap::argument_parser::arg_token_list_t;

    using argument_value_type = std::string;
    using invalid_argument_value_type = int;

    // test utility functions
    [[nodiscard]] std::string init_arg_flag_primary(std::size_t i) const {
        return std::format("--test-arg-{}", i);
    }

    [[nodiscard]] std::string init_arg_flag_secondary(std::size_t i) const {
        return std::format("-ta-{}", i);
    }

    [[nodiscard]] argument_value_type init_arg_value(std::size_t i) const {
        return std::format("test-value-{}", i);
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
        argv_vec.reserve(static_cast<std::size_t>(get_argc(n_positional_args, n_optional_args)));

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

    void free_argv(int argc, char** argv) const {
        for (int i = 0; i < argc; ++i)
            delete[] argv[i];
        delete[] argv;
    }

    [[nodiscard]] std::string init_arg_name_primary(const std::size_t i) const {
        return std::format("test-arg-{}", i);
    }

    [[nodiscard]] std::string init_arg_name_secondary(const std::size_t i) const {
        return std::format("ta-{}", i);
    }

    [[nodiscard]] argument_name init_arg_name(
        const std::size_t i, std::optional<char> flag_char = std::nullopt
    ) const {
        return argument_name(
            init_arg_name_primary(i), init_arg_name_secondary(i), std::move(flag_char)
        );
    }

    template <
        c_argument_value_type T = std::string,
        typename F = std::function<void(positional_argument<T>&)>>
    void add_positional_args(const std::size_t n, F&& setup_arg = [](positional_argument<T>&) {}) {
        for (std::size_t i = 0ull; i < n; ++i)
            setup_arg(this->sut.add_positional_argument<T>(
                init_arg_name_primary(i), init_arg_name_secondary(i)
            ));
    }

    template <
        c_argument_value_type T = std::string,
        typename F = std::function<void(optional_argument<T>&)>>
    void add_optional_args(
        const std::size_t n,
        const std::size_t begin_idx,
        F&& setup_arg = [](optional_argument<T>&) {}
    ) {
        for (std::size_t i = 0ull; i < n; ++i)
            setup_arg(this->sut.add_optional_argument<T>(
                init_arg_name_primary(begin_idx + i), init_arg_name_secondary(begin_idx + i)
            ));
    }

    template <c_argument_value_type T = std::string>
    void add_arguments(const std::size_t n_positional_args, std::size_t n_optional_args) {
        add_positional_args(n_positional_args);
        add_optional_args(n_optional_args, n_positional_args);
    }

    [[nodiscard]] arg_token_list_t init_arg_tokens(
        std::size_t n_positional_args, std::size_t n_optional_args
    ) {
        arg_token_list_t arg_tokens;
        arg_tokens.reserve(get_args_length(n_positional_args, n_optional_args));

        for (std::size_t i = 0ull; i < n_positional_args; ++i)
            arg_tokens.push_back(argument_token{argument_token::t_value, init_arg_value(i)});

        for (std::size_t i = 0ull; i < n_optional_args; ++i) {
            const auto arg_idx = n_positional_args + i;
            const auto arg_name = init_arg_name_primary(arg_idx);

            argument_token flag_tok{argument_token::t_flag_primary, arg_name};
            const auto opt_arg_it = this->sut._find_opt_arg(flag_tok);
            if (opt_arg_it != this->sut._optional_args.end())
                flag_tok.arg = *opt_arg_it;

            arg_tokens.push_back(std::move(flag_tok));
            arg_tokens.push_back(argument_token{argument_token::t_value, init_arg_value(arg_idx)});
        }

        return arg_tokens;
    }

    // argument_parser private member accessors
    [[nodiscard]] const std::optional<std::string>& get_program_name() const {
        return this->sut._program_name;
    }

    [[nodiscard]] const std::optional<std::string>& get_program_description() const {
        return this->sut._program_description;
    }

    [[nodiscard]] const std::optional<std::string>& get_program_version() const {
        return this->sut._program_version;
    }

    // private function callers
    [[nodiscard]] arg_token_list_t tokenize(int argc, char* argv[]) {
        return this->sut._tokenize(std::span(argv + 1, static_cast<std::size_t>(argc - 1)));
    }

    void parse_args_impl(const arg_token_list_t& arg_tokens) {
        this->sut._parse_args_impl(arg_tokens, this->unknown_args);
    }

    [[nodiscard]] arg_ptr_t get_argument(std::string_view arg_name) const {
        return this->sut._get_argument(arg_name);
    }

    ap::argument_parser sut;
    std::vector<std::string> unknown_args;
};

} // namespace ap_testing
