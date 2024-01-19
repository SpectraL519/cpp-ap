#include <ap/argument_parser.hpp>

#include <iostream>
#include <fstream>
#include <sstream>


int main(int argc, char* argv[]) {
    ap::argument_parser parser;
    parser.program_name("merge files")
          .program_description("shows the correct way of using default arguments")
          .default_optional_arguments({ap::default_argument::optional::help,
                                       ap::default_argument::optional::multi_input,
                                       ap::default_argument::optional::output});

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

    const auto input_file_name_list = parser.values("input");
    const auto output_file_name = parser.value("output");

    std::ofstream output_file(output_file_name);
    if (!output_file.is_open())
        throw std::runtime_error("Cannot open file: " + output_file_name);

    for (const auto& input_file_name : input_file_name_list) {
        std::ifstream input_file(input_file_name);
        std::stringstream buff;
        buff << input_file.rdbuf();
        output_file << buff.str();
    }

    return 0;
}
