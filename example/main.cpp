#include <ap/argument_parser.hpp>

#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser
        .program_name("power")
        .program_description("calculates the value of expression: base ^ exponent")
        .default_optional_arguments({ap::default_argument::optional::help});

    parser.add_positional_argument<double>("base");
    parser
        .add_optional_argument<double>("exponent", "e")
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

    const auto base = parser.value<double>("base");
    const auto vexp = parser.values<double>("exponent");

    for (const auto exp : vexp)
        std::cout << base << " ^ " << exp << " = " << std::pow(base, exp) << std::endl;

    return 0;
}
