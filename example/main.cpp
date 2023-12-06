#include <ap/argument_parser.hpp>

#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser.program_name("power").program_description(
        "calculates the value of expression: base ^ exponent"
    );

    parser.add_positional_argument<double>("base");
    parser.add_optional_argument<double>("exponent", "e").default_value(static_cast<double>(1));

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl << parser << std::endl;
        std::exit(1);
    }

    const auto base = parser.value<double>("base");
    const auto exp = parser.value<double>("exponent");

    std::cout << base << " ^ " << exp << " = " << std::pow(base, exp) << std::endl;

    return 0;
}
