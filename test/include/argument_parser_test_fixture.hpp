#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

namespace ap_testing {

struct argument_parser_test_fixture {
    argument_parser_test_fixture() = default;

    using cmd_argument_list = ap::argument_parser::cmd_argument_list;
    using argument_list_type = ap::argument_parser::argument_list_type;

    // argument_parser private function accessors
    const std::optional<std::string_view>& get_program_name() const {
        return sut._program_name;
    }

    const std::optional<std::string_view>& get_program_description() const {
        return sut._program_description;
    }

    cmd_argument_list _process_input(int argc, char* argv[]) const {
        return sut._process_input(argc, argv);
    }

    void _parse_args_impl(const cmd_argument_list& cmd_args) {
        sut._parse_args_impl(cmd_args);
    }

    ap::argument_parser sut;


    // test utility functions
    std::string prepare_arg_flag(int i) {
        return "--test_arg_" + std::to_string(i);
    }

    std::string prepare_arg_flag_short(int i) {
        return "-ta_" + std::to_string(i);
    }

    ap::detail::argument_name prepare_arg_name(int i) {
        return ap::detail::argument_name(
            "test_arg_" + std::to_string(i),
            "ta_" + std::to_string(i)
        );
    }

    std::string prepare_arg_value(int i) {
        return "argv_" + std::to_string(i);
    }

    char** prepare_argv(int argc, int argc_split) {
        char** argv = new char*[argc];

        argv[0] = new char[8];
        std::strcpy(argv[0], "program");

        for (int i = 1; i < argc_split; i++) {
            std::string arg = prepare_arg_flag_short(i - 1);
            argv[i] = new char[arg.length() + 1];
            std::strcpy(argv[i], arg.c_str());
        }

        for (int i = argc_split; i < argc; i++) {
            std::string arg = prepare_arg_flag(i - 1);
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

    void add_arguments(ap::argument_parser& parser, int argc, int argc_split) {
        for (int i = 1; i < argc_split; i++) {
            const auto arg_name = prepare_arg_name(i);
            parser.add_positional_argument(arg_name.name, arg_name.short_name.value());
        }

        for (int i = argc_split; i < argc; i++) {
            const auto arg_name = prepare_arg_name(i);
            parser.add_optional_argument(arg_name.name, arg_name.short_name.value());
        }
    }

    cmd_argument_list prepare_cmd_arg_list(int argc) {
        cmd_argument_list cmd_args;
        cmd_args.reserve(2 * argc);

        for (int i = 1; i < argc; i++) {
            cmd_args.emplace_back(prepare_arg_name(i).name);
            cmd_args.emplace_back(prepare_arg_value(i));
        }

        return cmd_args;
    }
};

} // namespace ap_testing
