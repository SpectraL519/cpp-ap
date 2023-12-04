#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

namespace ap_testing {

struct argument_parser_test_fixture {
    argument_parser_test_fixture() = default;

    using cmd_argument_list = ap::argument_parser::cmd_argument_list;
    using argument_list_type = ap::argument_parser::argument_list_type;
    using argument_opt_type = ap::argument_parser::argument_opt_type;


    // test utility functions
    std::string prepare_arg_flag(std::size_t i) const {
        return "--test_arg_" + std::to_string(i);
    }

    std::string prepare_arg_flag_short(std::size_t i) const {
        return "-ta_" + std::to_string(i);
    }

    std::string prepare_arg_value(std::size_t i) const {
        return "test_value_" + std::to_string(i);
    }

    std::size_t get_args_length(std::size_t num_args, std::size_t args_split) const {
        return args_split + 2 * (num_args - args_split);
    }

    int get_argc(std::size_t num_args, std::size_t args_split) const {
        return static_cast<int>(get_args_length(num_args, args_split) + 1);
    }

    char** prepare_argv(std::size_t num_args, std::size_t args_split) const {
        char** argv = new char*[get_argc(num_args, args_split)];

        argv[0] = new char[8];
        std::strcpy(argv[0], "program");

        for (std::size_t i = 0; i < args_split; i++) { // positional args
            std::string arg_v = prepare_arg_value(i);

            const auto arg_i = i + 1;
            argv[arg_i] = new char[arg_v.length() + 1];
            std::strcpy(argv[arg_i], arg_v.c_str());
        }

        for (std::size_t i = args_split; i < num_args; i++) { // optional args
            std::string arg = prepare_arg_flag(i);
            std::string arg_v = prepare_arg_value(i);

            const std::size_t arg_i = 2 * i - args_split + 1;
            const std::size_t arg_v_i = arg_i + 1;

            argv[arg_i] = new char[arg.length() + 1];
            argv[arg_v_i] = new char[arg_v.length() + 1];

            std::strcpy(argv[arg_i], arg.c_str());
            std::strcpy(argv[arg_v_i], arg_v.c_str());
        }

        return argv;
    }

    void free_argv(std::size_t argc, char** argv) const {
        for (std::size_t i = 0; i < argc; i++)
            delete[] argv[i];
        delete[] argv;
    }

    ap::detail::argument_name prepare_arg_name(std::size_t i) const {
        return ap::detail::argument_name(
            "test_arg_" + std::to_string(i), "ta_" + std::to_string(i)
        );
    }

    void add_arguments(ap::argument_parser& parser, std::size_t num_args, std::size_t args_split) const {
        for (std::size_t i = 0; i < args_split; i++) { // positional args
            const auto arg_name = prepare_arg_name(i);
            parser.add_positional_argument(arg_name.name, arg_name.short_name.value());
        }

        for (std::size_t i = args_split; i < num_args; i++) { // optional args
            const auto arg_name = prepare_arg_name(i);
            parser.add_optional_argument(arg_name.name, arg_name.short_name.value());
        }
    }

    cmd_argument_list
        prepare_cmd_arg_list(std::size_t num_args, std::size_t args_split) const {
        cmd_argument_list cmd_args;
        cmd_args.reserve(get_args_length(num_args, args_split));

        for (std::size_t i = 0; i < args_split; i++) { // positional args
            cmd_args.emplace_back(prepare_arg_value(i));
        }
        for (std::size_t i = args_split; i < num_args; i++) { // optional args
            cmd_args.emplace_back(prepare_arg_name(i).name);
            cmd_args.emplace_back(prepare_arg_value(i));
        }

        return cmd_args;
    }


    // argument_parser private function accessors
    const std::optional<std::string_view>& sut_program_name() const {
        return sut._program_name;
    }

    const std::optional<std::string_view>& sut_program_description() const {
        return sut._program_description;
    }

    cmd_argument_list sut_process_input(int argc, char* argv[]) const {
        return sut._process_input(argc, argv);
    }

    void sut_parse_args_impl(const cmd_argument_list& cmd_args) {
        sut._parse_args_impl(cmd_args);
    }

    argument_opt_type sut_get_argument(std::string_view arg_name) const {
        return sut._get_argument(arg_name);
    }

    ap::argument_parser sut;
};

} // namespace ap_testing
