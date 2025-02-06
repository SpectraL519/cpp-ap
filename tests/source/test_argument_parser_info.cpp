#define AP_TESTING

#include "argument_parser_test_fixture.hpp"
#include "doctest.h"

using namespace ap_testing;

TEST_SUITE_BEGIN("test_argument_parser_info");

struct test_argument_parser_info : public argument_parser_test_fixture {
    const std::string test_name = "test program name";
    const std::string test_description = "test program description";
};

TEST_CASE_FIXTURE(
    test_argument_parser_info, "parser's program name member should be nullopt by default"
) {
    const auto stored_program_name = sut_get_program_name();
    CHECK_FALSE(stored_program_name);
}

TEST_CASE_FIXTURE(test_argument_parser_info, "name() should set the program name member") {
    sut.program_name(test_name);

    const auto stored_program_name = sut_get_program_name();

    REQUIRE(stored_program_name);
    CHECK_EQ(stored_program_name.value(), test_name);
}

TEST_CASE_FIXTURE(
    test_argument_parser_info, "parser's program description member should be nullopt by default"
) {
    const auto stored_program_description = sut_get_program_description();
    CHECK_FALSE(stored_program_description);
}

TEST_CASE_FIXTURE(test_argument_parser_info, "name() should set the program name member") {
    sut.program_description(test_description);

    const auto stored_program_description = sut_get_program_description();

    REQUIRE(stored_program_description);
    CHECK_EQ(stored_program_description.value(), test_description);
}

TEST_SUITE_END();
