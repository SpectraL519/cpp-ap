#include <ap/argument_parser.hpp>

#include <iostream>
#include <cmath>

int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser
        .program_name("power")
        .program_description("calculates the value of expression: base ^ exponent");

    parser.add_positional_argument<double>("base");
    parser.add_positional_argument<double>("exponent");

    parser.parse_args(argc, argv);

    const auto base = parser.value<double>("base");
    const auto exp = parser.value<double>("exponent");

    std::cout << base << " ^ " << exp << " = " << std::pow(base, exp) << std::endl;

    return 0;
}
