#include <ap/argument_parser.hpp>

#include <bitset>
#include <iostream>


int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser.program_name("convert numbers")
          .program_description("shows the correct way of using dthe choices parameter")
          .default_optional_arguments({ap::default_argument::optional::help});

    parser.add_optional_argument<std::size_t>("number", "n")
          .nargs(ap::nargs::any())
          .help("positive integer value");
    parser.add_optional_argument("base", "b")
          .required()
          .default_value("dec")
          .choices({"bin", "dec", "hex"})
          .help("output number format base");

    try {
        parser.parse_args(argc, argv);
    }
    catch (const ap::argument_parser_error& err) {
        std::cerr << "[ERROR] : " << err.what() << std::endl << parser << std::endl;
        std::exit(EXIT_FAILURE);
    }

    if (parser.value<bool>("help")) {
        std::cout << parser << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    const auto numbers = parser.values<std::size_t>("number");
    const auto base = parser.value("base");

    if (base == "bin") {
        constexpr std::size_t nbits = sizeof(std::size_t) * 8;
        for (const auto n : numbers)
            std::cout << std::bitset<nbits>(n) << std::endl;
    }
    else if (base == "dec") {
        for (const auto n : numbers)
            std::cout << n << std::endl;
    }
    else {
        std::cout << std::hex;
        for (const auto n : numbers)
            std::cout << n << std::endl;
    }

    return 0;
}
