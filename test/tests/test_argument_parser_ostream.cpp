#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>

#include <sstream>

using namespace ap;


TEST_SUITE("test_argument_parser_ostream") {

TEST_CASE("operator<< should push correct data to the output stream") {
    argument_parser parser;

    std::stringstream ss;
    std::stringstream expected_ss;

    const std::string test_name = "test program name";
    const std::string test_description = "test program description";

    SUBCASE("nothing by default") {}

    SUBCASE("only program name if nothing else set") {
        parser.program_name(test_name);
        expected_ss << test_name << std::endl;
    }

    SUBCASE("only program description if nothing else set") {
        parser.program_description(test_description);
        expected_ss << test_description << std::endl;
    }

    SUBCASE("program name and description if both specified") {
        parser.program_name(test_name).program_description(test_description);
        expected_ss << test_name << std::endl << test_description << std::endl;
    }

    ss << parser;

    REQUIRE_EQ(ss.str(), expected_ss.str());
}

}
