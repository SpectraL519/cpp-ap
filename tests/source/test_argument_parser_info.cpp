#include "argument_parser_test_fixture.hpp"
#include "doctest.h"

using namespace ap_testing;

struct test_argument_parser_info : public argument_parser_test_fixture {
    const std::string test_name = "test program name";
    const std::string test_description = "test program description";
    const ap::version test_version = {1u, 2u, 3u};
    const std::string test_str_version = "alpha";
};

TEST_CASE_FIXTURE(
    test_argument_parser_info, "parser's program name member should be nullopt by default"
) {
    const auto stored_program_name = get_program_name();
    CHECK_FALSE(stored_program_name);
}

TEST_CASE_FIXTURE(test_argument_parser_info, "name() should set the program name member") {
    sut.program_name(test_name);

    const auto stored_program_name = get_program_name();

    REQUIRE(stored_program_name);
    CHECK_EQ(stored_program_name.value(), test_name);
}

TEST_CASE_FIXTURE(
    test_argument_parser_info, "parser's program version member should be nullopt by default"
) {
    const auto stored_program_version = get_program_version();
    CHECK_FALSE(stored_program_version);
}

TEST_CASE_FIXTURE(test_argument_parser_info, "version() should set the program version member") {
    sut.program_version(test_version);
    auto stored_program_version = get_program_version();
    REQUIRE(stored_program_version);
    CHECK_EQ(stored_program_version.value(), test_version.str());

    sut.program_version(test_str_version);
    stored_program_version = get_program_version();
    REQUIRE(stored_program_version);
    CHECK_EQ(stored_program_version.value(), test_str_version);
}

TEST_CASE_FIXTURE(
    test_argument_parser_info, "parser's program description member should be nullopt by default"
) {
    const auto stored_program_description = get_program_description();
    CHECK_FALSE(stored_program_description);
}

TEST_CASE_FIXTURE(test_argument_parser_info, "name() should set the program name member") {
    sut.program_description(test_description);

    const auto stored_program_description = get_program_description();

    REQUIRE(stored_program_description);
    CHECK_EQ(stored_program_description.value(), test_description);
}
