#include <ap/argument_parser.hpp>

#include <cstdint>
#include <iostream>

namespace {

enum class verbosity_level : uint16_t { low, mid, high };
constexpr verbosity_level max_verbosity_level = verbosity_level::high;

std::istream& operator>>(std::istream& input, verbosity_level& verbosity) {
    uint16_t value;
    input >> value;

    if (value > static_cast<uint16_t>(max_verbosity_level)) {
        std::cerr << "[ERROR] : Invalid verbosity_level value - " << value << std::endl;
        std::exit(EXIT_FAILURE);
    }

    verbosity = static_cast<verbosity_level>(value);

    return input;
}


void print_msg(const verbosity_level verbosity) {
    switch (verbosity) {
    case verbosity_level::low:
        break;

    case verbosity_level::mid:
        std::cout << "msg" << std::endl;
        break;

    case verbosity_level::high:
        std::cout << "this is a really verbose message" << std::endl;
        break;
    }
}

} // namespace

int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser.program_name("verbosity level")
        .program_description("shows the correct way of using enums as a parser argument type")
        .default_optional_arguments({ ap::default_argument::optional::help });

    parser.add_optional_argument<verbosity_level>("verbosity_level", "v")
        .default_value(verbosity_level::low)
        .implicit_value(verbosity_level::mid)
        .nargs(1);

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

    print_msg(parser.value<verbosity_level>("verbosity_level"));

    return 0;
}
