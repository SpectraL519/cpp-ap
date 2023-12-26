#include <ap/argument_parser.hpp>

#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser
        .program_name("power")
        .program_description("calculates the value of expression: base ^ exponent");

    parser
        .add_optional_argument<bool>("help", "h")
        .default_value(false)
        .implicit_value(true)
        .nargs(0)
        .help("displays help message")
        .bypass_required();

    parser.add_positional_argument<double>("base");
    parser
        .add_optional_argument<double>("exponent", "e")
        .default_value(static_cast<double>(1));

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
    const auto exp = parser.value<double>("exponent");

    std::cout << base << " ^ " << exp << " = " << std::pow(base, exp) << std::endl;

    return 0;
}
