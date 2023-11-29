#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

namespace ap_testing {

struct argument_parser_test_fixture {
    argument_parser_test_fixture() = default;

    ap::argument_parser& set_program_name(const std::string_view program_name) {
        return sut.program_name(program_name);
    }

    const std::optional<std::string_view>& get_program_name() const {
        return sut._program_name;
    }

    ap::argument_parser sut;
};

} // namespace ap_testing
