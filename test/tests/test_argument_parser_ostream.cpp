#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>

#include <sstream>

using namespace ap;


TEST_CASE("operator<< should push correct data to the output stream") {
    argument_parser parser;

    std::stringstream ss;
    std::stringstream expected_ss;

    SUBCASE("nothing by default") {}

    SUBCASE("only program name if nothing else set") {
        std::string name = "test program name";
        parser.program_name(name);
        expected_ss << name << std::endl;
    }

    SUBCASE("only program description if nothing else set") {
        std::string description = "test program description";
        parser.program_description(description);
        expected_ss << description << std::endl;
    }

    SUBCASE("program name and description if both specified") {
        std::string name = "test program name";
        std::string description = "test program description";

        parser
            .program_name(name)
            .program_description(description);

        expected_ss << name << std::endl << description << std::endl;
    }

    CAPTURE(expected_ss);

    ss << parser;

    REQUIRE_EQ(ss.str(), expected_ss.str());
}
