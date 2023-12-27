#include <ap/argument_parser.hpp>

#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser
        .program_name("multiply")
        .program_description("calculates the value of expression: multiplicant * multiplier")
        .default_optional_arguments({ap::default_argument::optional::help});

    parser.add_positional_argument<double>("multiplicant");
    parser
        .add_optional_argument<double>("multiplier", "m")
        .default_value(0.)
        .implicit_value(1.)
        .nargs(ap::nargs::up_to(5));

    try {
        parser.parse_args(argc, argv);
    }
    catch (const std::exception& err) {
        std::cerr << "[ERROR] : " << err.what() << std::endl << parser << std::endl;
        std::exit(1);
    }

    if (parser.value<bool>("help")) {
        std::cout << parser << std::endl;
        std::exit(0);
    }

    const auto multiplicant = parser.value<double>("multiplicant");
    const auto vmultiplier = parser.values<double>("multiplier");

    for (const auto multiplier : vmultiplier)
        std::cout << multiplicant << " * " << multiplier << " = " << multiplicant * multiplier << std::endl;

    return 0;
}
