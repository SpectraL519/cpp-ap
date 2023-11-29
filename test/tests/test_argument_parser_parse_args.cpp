#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>
#include <argument_parser_test_fixture.hpp>

#include <iostream>

using namespace ap::detail;


namespace {

constexpr std::string_view test_program_name = "test program name";

} // namespace


namespace ap_testing {


TEST_SUITE_BEGIN("test_argument_parser_parse_args");

TEST_CASE_FIXTURE(
    argument_parser_test_fixture,
    "_process_input should return an empty vector for no command-line arguments"
) {
    set_program_name(test_program_name);
    const auto& program_name = get_program_name();

    REQUIRE(program_name);
    REQUIRE_EQ(program_name.value(), test_program_name);
}

TEST_SUITE_END();

} // namespace ap_testing
