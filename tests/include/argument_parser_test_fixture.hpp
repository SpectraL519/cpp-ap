#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::argument::detail::argument_name;

namespace ap_testing {

struct argument_parser_test_fixture {
    argument_parser_test_fixture() = default;
    ~argument_parser_test_fixture() = default;

    using arg_token = ap::argument_parser::arg_token;
    using arg_token_list = ap::argument_parser::arg_token_list;
    using argument_list_type = ap::argument_parser::argument_list_type;
    using argument_opt_type = ap::argument_parser::argument_opt_type;

    using argument_value_type = std::string;
    using invalid_argument_value_type = int;

    // test utility functions
    [[nodiscard]] std::string prepare_arg_flag_primary(std::size_t i) const {
        return "--test_arg_" + std::to_string(i);
    }

    [[nodiscard]] std::string prepare_arg_flag_secondary(std::size_t i) const {
        return "-ta_" + std::to_string(i);
    }

    [[nodiscard]] argument_value_type prepare_arg_value(std::size_t i) const {
        return "test_value_" + std::to_string(i);
    }

    [[nodiscard]] std::size_t get_args_length(std::size_t num_args, std::size_t args_split) const {
        return args_split + 2 * (num_args - args_split);
    }

    [[nodiscard]] int get_argc(std::size_t num_args, std::size_t args_split) const {
        return static_cast<int>(get_args_length(num_args, args_split) + 1);
    }

    [[nodiscard]] char** to_char_2d_array(const std::vector<std::string>& argv_vec) const {
        char** argv = new char*[argv_vec.size()];

        for (size_t i = 0; i < argv_vec.size(); ++i) {
            argv[i] = new char[argv_vec[i].size() + 1];
            std::strcpy(argv[i], argv_vec[i].c_str());
        }

        return argv;
    }

    [[nodiscard]] std::vector<std::string> prepare_argv_vec(
        std::size_t num_args, std::size_t args_split
    ) const {
        std::vector<std::string> argv_vec;
        argv_vec.reserve(get_argc(num_args, args_split));

        argv_vec.emplace_back("program");

        for (std::size_t i = 0; i < args_split; ++i) // positional args
            argv_vec.emplace_back(prepare_arg_value(i));

        for (std::size_t i = args_split; i < num_args; ++i) { // optional args
            argv_vec.emplace_back(prepare_arg_flag_primary(i));
            argv_vec.emplace_back(prepare_arg_value(i));
        }

        return argv_vec;
    }

    [[nodiscard]] char** prepare_argv(std::size_t num_args, std::size_t args_split) const {
        return to_char_2d_array(prepare_argv_vec(num_args, args_split));
    }

    void free_argv(std::size_t argc, char** argv) const {
        for (std::size_t i = 0; i < argc; ++i)
            delete[] argv[i];
        delete[] argv;
    }

    [[nodiscard]] argument_name prepare_arg_name(std::size_t i) const {
        return argument_name("test_arg_" + std::to_string(i), "ta_" + std::to_string(i));
    }

    void add_arguments(ap::argument_parser& parser, std::size_t num_args, std::size_t args_split)
        const {
        for (std::size_t i = 0; i < args_split; ++i) { // positional args
            const auto arg_name = prepare_arg_name(i);
            parser.add_positional_argument(arg_name.primary, arg_name.secondary.value());
        }

        for (std::size_t i = args_split; i < num_args; ++i) { // optional args
            const auto arg_name = prepare_arg_name(i);
            parser.add_optional_argument(arg_name.primary, arg_name.secondary.value());
        }
    }

    [[nodiscard]] arg_token_list prepare_arg_token_list(
        std::size_t num_args, std::size_t args_split
    ) const {
        arg_token_list arg_tokens;
        arg_tokens.reserve(get_args_length(num_args, args_split));

        for (std::size_t i = 0; i < args_split; ++i) { // positional args
            arg_tokens.push_back(arg_token{arg_token::token_type::value, prepare_arg_value(i)});
        }
        for (std::size_t i = args_split; i < num_args; ++i) { // optional args
            arg_tokens.push_back(arg_token{arg_token::token_type::flag, prepare_arg_name(i).primary}
            );
            arg_tokens.push_back(arg_token{arg_token::token_type::value, prepare_arg_value(i)});
        }

        return arg_tokens;
    }

    // argument_parser private function accessors
    [[nodiscard]] const std::optional<std::string>& sut_get_program_name() const {
        return sut._program_name;
    }

    [[nodiscard]] const std::optional<std::string>& sut_get_program_description() const {
        return sut._program_description;
    }

    [[nodiscard]] arg_token_list sut_tokenize(int argc, char* argv[]) const {
        return sut._tokenize(argc, argv);
    }

    void sut_parse_args_impl(const arg_token_list& arg_tokens) {
        sut._parse_args_impl(arg_tokens);
    }

    [[nodiscard]] argument_opt_type sut_get_argument(std::string_view arg_name) const {
        return sut._get_argument(arg_name);
    }

    ap::argument_parser sut;
};

} // namespace ap_testing
