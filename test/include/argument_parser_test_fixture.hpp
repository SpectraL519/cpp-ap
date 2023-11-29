#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

namespace ap_testing {

struct argument_parser_test_fixture {
    argument_parser_test_fixture() = default;

    using input_args_list = ap::argument_parser::input_args_list;
    using argument_list_type = ap::argument_parser::argument_list_type;

    const std::optional<std::string_view>& get_program_name() const {
        return sut._program_name;
    }

    const std::optional<std::string_view>& get_program_description() const {
        return sut._program_description;
    }

    input_args_list _process_input(int argc, char* argv[]) const {
        return sut._process_input(argc, argv);
    }

    ap::argument_parser sut;
};

} // namespace ap_testing
