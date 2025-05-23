#include <ap/argument_parser.hpp>

#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {
    // create the parser class instance
    ap::argument_parser parser;
    parser.program_name("arg_desc_test")
          .program_description("A simple program to test the parser's and argumments' description.")
          .default_optional_arguments({ap::argument::default_optional::help});

    // add arguments
    parser.add_positional_argument("simple-pos")
          .choices({"abc", "def"})
          .help("A simple positional argument.");
    parser.add_optional_argument<int>("simpl-opt", "s")
          .nargs(ap::nargs::any())
          .help("A simple optional argument.");

    std::cout << parser;

    return 0;
}

/*
g++ -o arg_desc_test arg_desc_test.cpp -I include/ -std=c++20
*/
