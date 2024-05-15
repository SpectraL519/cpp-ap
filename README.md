# CPP-AP
Command-line argument parser for C++20

[![g++](https://github.com/SpectraL519/cpp-ap/actions/workflows/gpp.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/g++)
[![clang++](https://github.com/SpectraL519/cpp-ap/actions/workflows/clang.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/clang++)
[![test](https://github.com/SpectraL519/cpp-ap/actions/workflows/test.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/test)
[![format](https://github.com/SpectraL519/cpp-ap/actions/workflows/format.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/format)

<br />

## Overview

The goal of the project was to create a light,  intuitive and simple to use command-line argument parser library for the `C++20` and newer standards.

The `CPP-AP` library does not require installing any additional tools or heavy libraries, like with `boost::program_options`. Much like with the `Doctest` framework - the only thing you need to do is copy the `argument_parser.hpp` file into the include directory of your project and you're set to go.

> [!NOTE]
> [v1.0](https://github.com/SpectraL519/cpp-ap/commit/9a9e5360766b732f322ae2efe3cf5ec5f9268eef) of the library has been developed for the *Team Programming* course at the *Wrocław University of Science and Technology*.
>
> Faculty: *W04N - Faculty of Information and Communication Technology*
>
> Field of study: *Algorithmic Computer Science*
>
> The project has received the 1st place at the 2024 CreatiWITy competition organized by the faculty. The article in Polish can be found on the [faculty website](https://wit.pwr.edu.pl/aktualnosci/oto-laureaci-konkursu-creatiwity-273.html). Please note that this is not a technical article :)

<br />
<br />

## Table of contents

* [Tutorial](#tutorial)
    * [Including CPP-AP into a project](#including-cpp-ap-into-a-project)
        * [CMake integration](#cmake-integration)
        * [Downloading the library](#downloading-the-library)
        * [Downloading the single header](#downloading-the-single-header)
    * [The parser class](#the-parser-class)
    * [Adding arguments](#adding-arguments)
    * [Argument parameters](#argument-parameters)
    * [Default arguments](#default-arguments)
    * [Parsing arguments](#parsing-arguments)
* [Examples](#examples)
* [Dev notes](#dev-notes)
    * [Building and testing](#building-and-testing)
    * [Formatting](#formatting)
* [Documentation](#documentation)
* [Compiler support](#compiler-support)
* [Licence](#licence)
* [Change log](change_log.md)

<br />
<br />

## Tutorial

### Including CPP-AP into a project

There are 3 main ways to include the CPP-AP library into a C++ project:

#### CMake integration:

For CMake projects you can simply fetch the library in your `CMakeLists.txt` file:

```cmake
cmake_minimum_required(VERSION 3.12)

project(my_project LANGUAGES CXX)

# Include FetchContent module
include(FetchContent)

# Fetch CPP-AP library
FetchContent_Declare(
    cpp-ap
    GIT_REPOSITORY https://github.com/SpectraL519/cpp-ap.git
    GIT_TAG master # here you can specify the desired tag or branch
)

FetchContent_MakeAvailable(cpp-ap)

# Define the executable for the project
add_executable(my_project main.cpp)

set_target_properties(my_project PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED YES
)

# Link against the cpp-ap library
target_link_libraries(my_project PRIVATE cpp-ap)
```

#### Downloading the library

If you do not use CMake you can dowload the desired [library release](https://github.com/SpectraL519/cpp-ap/releases), extract it in a desired directory and simply add the `<cpp-ap-dir>/include` to the include paths of your project.

#### Downloading the single header

The core of the library is a [single header file](https://github.com/SpectraL519/cpp-ap/blob/master/include/ap/argument_parser.hpp) so to be able to use the library you can simply download the `argument_parser.hpp` header and paste it into the include directory of your project.

> [!IMPORTANT]
> To actually use the library in your project simply include the single header in you `main.cpp` file:
> ```c++
> #include <ap/argument_parser.hpp>
> ```

<br />

### The parser class

To use the argument parser in your code you need to use the `ap::argument_parser` class.

The parameters you can specify for a parser's instance are:
* Program name
* Program description
* [Arguments](#adding-arguments)

```c++
ap::argument_parser parser;
parser.program_name("Name of the program")
      .program_description("Description of the program");
```

### Adding arguments

The parser supports both positional and optional arguments. Both argument types are identified by their names represented as strings. Arguments can be defined with only a primary name or with a primary and a secondary (short) name.

> [!NOTE]
>  The basic rules of parsing positional and optional arguments are described in the [Parsing arguments](#parsing-arguments) section.

To add an argument to the parameter's configurations use the following syntax:

```c++
parser.add_<positional/optional>_argument<value_type>("argument");
```

or

```c++
parser.add_<positional/optional>_argument<value_type>("argument", "a");
```

> [!NOTE]
>  The library supports any argument value types which meet the following requirements:
* The `std::ostream& operator<<` is overloaded for the value type
* The value type has a copy constructor and an assignment operator

> [!IMPORTANT]
>  If the `value_type` is not provided, `std::string` will be used.

You can also add boolean flags:

```c++
parser.add_flag("enable_some_option", "eso").help("enables option: some option");
/* equivalent to:
parser.add_optional_argument<bool>("enable_some_option", "eso")
      .default_value(false)
      .implicit_value(true)
      .nargs(0)
      .help("enables option: some option");
*/
```

Boolean flags store `true` by default but you can specify whether the flag should store `true` or `false` when used:
```c++
parser.add_flag<false>("disable_another_option", "dao").help("disables option: another option");
/* equivalent to:
parser.add_optional_argument<bool>("disable_another_option", "dao")
      .default_value(true)
      .implicit_value(false)
      .nargs(0)
      .help("disables option: another option");
*/
```

### Argument parameters

**Common parameters**

Parameters which can be specified for both positional and optional arguments include:

* `help` - the argument's description which will be printed when printing the parser class instance.

    ```c++
    parser.add_positional_argument<std::size_t>("number", "n")
          .help("a positive integer value");
    ```

* `choices` - a list of valid argument values.
    The `choices` parameter takes a `const std::vector<value_type>&` as an argument.

    ```c++
    parser.add_optional_argument<char>("method", "m").choices({'a', 'b', 'c'});
    ```

> [!IMPORTANT]
> To use the `choices` the `value_type` must overload the equaility comparison operator: `==`;

* `action` - a function performed after reading an argument's value.

    Actions are represented as functions, which take the argument's value as an argument. There are two types of actions:
    * Void actions - `void(value_type&)`
    * Valued actions - `value_type(const value_type&)`

    The default action is an empty void function.

    Actions can be used to modify a value parsed from the command-line:
    ```c++
    parser.add_optional_argument<double>("denominator", "d")
          .action<ap::void_action>([](double& value) { value = 1. / value; });
    ```
    or en equivalent valued action:
    ```c++
    parser.add_optional_argument<double>("denominator", "d")
          .action<ap::valued_action>([](const double& value) { return 1. / value; });
    ```

    Actions can also be used to perform some value checking logic, e.g. the predefined `check_file_exists` which checks if a file with a given name exists:
    ```c++
    parser.add_optional_argument("input", "i")
          .action<ap::void_action>(ap::action::check_file_exists());
    ```

**Optional argument specific parameters**

* `required` - if this option is set for an argument, failure of parsing it's value will result in an error.
    ```c++
    parser.add_optional_argument("output", "o").required();
    ```

* `bypass_required` - if this option is set for an argument, parsing it's value will overrite the `required` option for other optional arguments and all positional arguments.

* `nargs` - sets the allowed number of values to be parsed for an argument. This can be set as a:
    * Concrete number:
        ```c++
        parser.add_optional_argument("input", "i").nargs(1);
        ```
    * Bound range:
        ```c++
        parser.add_optional_argument("input", "i").nargs(1, 3);
        ```
    * Partially bound range:
        ```c++
        parser.add_optional_argument("input", "i").nargs(ap::nargs::at_least(1));  // n >= 1
        parser.add_optional_argument("input", "i").nargs(ap::nargs::more_than(1)); // n > 1
        parser.add_optional_argument("input", "i").nargs(ap::nargs::less_than(5)); // n < 5
        parser.add_optional_argument("input", "i").nargs(ap::nargs::up_to(5));     // n <= 5
        ```
    * Unbound range:
        ```c++
        parser.add_optional_argument("input", "i").nargs(ap::nargs::any());
        ```

> [!NOTE]
> The default nargs value is `1`.

* `default_value` - the default value for an argument which will be used if no values for this argument are parsed
    ```c++
    parser.add_opitonal_argument("output", "o").default_value("out.txt");
    ```

* `implicit_value` - a value which will be set for an argument if only it's flag is parsed from the command-line but no value follows
    ```c++
    // program.cpp
    parser.add_optional_argument("save", "s")
          .implicit_value("out.txt")
          .help("save the program's output to a file");
    ```
    In this example if you run the program with only a `-s` or `--save` flag and no value, the value will be set to `out.txt`.

### Default arguments

The `CPP-AP` library has a few default arguments defined. To add a default argument to the parser use the following:

```c++
// add positional arguments - pass a std::vector of default positional arguments
parser.default_positional_arguments({...});
// here ... represents a list of ap::default_argument::positional values (or alternatively ap::default_posarg)

// add optional arguments - pass a std::vector of default optional arguments
parser.default_positional_arguments({...});
// here ... represents a list of ap::default_argument::optional values (or alternatively ap::default_optarg)
```

The supported default arguments are:
* `positional::input`:
    ```c++
    // equivalent to:
    parser.add_positional_argument<std::string>("input")
          .action<ap::void_action>(ap::action::check_file_exists())
          .help("Input file path");
    ```

* `positional::output`:
    ```c++
    // equivalent to:
    parser.add_positional_argument("output").help("Output file path");
    ```

* `optional::help`:
    ```c++
    // equivalent to:
    parser.add_flag("help", "h").bypass_required().help("Display help message");
    ```

> [!NOTE]
> As of now the *on flag action* functionality is not implemented in the library - this will be added in a future release.
> To properly use the help argument in the current release add the following right beneath the `parser.parse_args(argc, argv)` try-catch block:
> ```c++
> if (parser.value<bool>("help")) {
>     std::cout << parser << std::endl;
>     std::exit(EXIT_SUCCESS);
> }
> ```

* `optional::input` and `optional::multi_input`:
    ```c++
    // input - equivalent to:
    parser.add_optional_argument("input", "i")
          .required()
          .nargs(1)
          .action<ap::void_action>(ap::action::check_file_exists())
          .help("Input file path");

    // multi_input - equivalent to:
    parser.add_optional_argument("input", "i")
          .required()
          .nargs(ap::nargs::at_least(1))
          .action<ap::void_action>(ap::action::check_file_exists())
          .help("Input files paths");
    ```

* `optional::output` and `optional::multi_output`:
    ```c++
    // output - equivalent to:
    parser.add_optional_argument("output", "o")
          .required()
          .nargs(1)
          .help("Output file path");

    // multi_otput - equivalent to:
    parser.add_optional_argument("output", "o")
          .required()
          .nargs(ap::nargs::at_least(1))
          .help("Output files paths");
    ```

> [!NOTE]
> The `argument_parser::default_<positional/optional>_arguments` functions will be modified to use a variadic argument list instead of a `std::vector` in a future release.

### Parsing arguments

To parse the command-line arguments use the `argument_parser::parse_args` method:

```c++
// power.cpp
#include <ap/argument_parser.hpp>

#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {
    // create the parser class instance
    ap::argument_parser parser;
    parser.program_name("power calculator")
          .program_description("calculates the value of an expression: base ^ exponent");

    // add arguments
    parser.add_positional_argument<double>("base").help("the exponentation base value");
    parser.add_optional_argument<int>("exponent", "e")
          .nargs(ap::nargs::any())
          .help("the exponent value");

    parser.default_optional_arguments({ap::default_argument::optional::help});

    // parse command-line arguments
    try {
        parser.parse_args(argc, argv);
    }
    catch (const ap::argument_parser_error& err) {
        std::cerr << "[ERROR] : " << err.what() << std::endl << parser << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // check for the help argument presence
    if (parser.value<bool>("help")) {
        std::cout << parser << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    // check if any values for the `exponent` argument have been parsed
    if (not parser.has_value("exponent")) {
        std::cout << "no exponent values given" << std::endl;
        std::exit(EXIT_SUCCESS);
    }

    const double base = parser.value<double>("base");
    const std::vector<int> exponent_values = parser.values<int>("exponent");

    for (const int exponent : exponent_values) {
        std::cout << base << " ^ " << exponent << " = " << std::pow(base, exponent) << std::endl;
    }

    return 0;
}

// compiled with:
// g++ -o power power.cpp -I <cpp-ap-include-dir>
```

**Argument parsing rules:**

* Positional arguments are parsed first, in the order they were defined in and without a flag.

    In the example above the first command-line argument must be the value for `positional_argument`:
    ```shell
    ./power 2
    # out:
    # no exponent values given
    ```

    ```shell
    ./power
    # out:
    # [ERROR] : No values parsed for a required argument [base]
    # power calculator
    # calculates the value of an expression: base ^ exponent
    #         [base] : the exponentation base value
    #         [exponent,e] : the exponent value
    #         [help,h] : Display help message
    ```

> [!IMPORTANT]
> For each positional argument there must be **exactly one value**.

* Optional arguments are parsed only with a flag:
    ```shell
    ./power 2 --exponent 1 2 3
    # equivalent to: ./power 2 -e 1 2 3
    # out:
    # 2 ^ 1 = 2
    # 2 ^ 2 = 4
    # 2 ^ 3 = 8
    ```

    You can use the flag for each command-line value:
    ```shell
    ./power 2 -e 1 -e 2 -e 3
    ```

    Not using a flag will result in an error:
    ```shell
    ./power 2 1 2 3
    # out:
    # [ERROR] : Failed to deduce the argument for the given value `1`
    # power calculator
    # calculates the value of an expression: base & exponent
    #         [base] : the exponentation base value
    #         [exponent,e] : the exponent value
    #         [help,h] : Display help message
    ```

> [!IMPORTANT]
> The parser behaviour depends on the argument definitions. The argument parameters are described int the [Argument parameters](#argument-parameters) section.

<br />
<br />

## Examples

If you wish to test the parser functionality with some real examples then follow these steps:

Open your terminal in the project's example directory:
```shell
cd <project-root>/example
```

The examples' source files are in the `<project-root>/example/source` directory.

> [!NOTE]
> Each source file is a sepparate example.

Building the examples:

```shell
cmake -B build
cd build
make
```

or

```shell
mkdir build && cd build
cmake ..
make
```

The compiled binaries will appear in the `<project-root>/example/build/bin` directory.

<br />
<br />

## Dev notes

### Building and testing:

First build the testing executable:

```shell
cmake -B build
cd build && make
```

or alternatively:

```shell
mkdir build && cd build
cmake ..
make
```

This will build the test executable `run_tests` in the `<project-root>/build/test` directory.

> [!TIP]
> Building on Windows -  use the `-G "Unix Makefiles"` option when running CMake to build a GNU Make project instead of a default Visual Studio project.

Run the tests:

* All tests:

    ```shell
    ./run_tests
    ```

* A single test suite:

    ```shell
    ./run_tests -ts="<test-suite-name>"
    ```

    > [!NOTE]
    > Test suites in the project have the same names as the files they're in.

<br />

### Formatting

> [!IMPORTANT]
> To ensure new line encoding compatibility the project uses unix new line encoding.
>
> This can be set using the `git config --global core.autocrlf true` command.
> More details can be found [here](https://docs.github.com/en/get-started/getting-started-with-git/configuring-git-to-handle-line-endings)

> [!NOTE]
> The project uses `clang-format-17`.
>
> To install this tool on ubuntu run `sudo bash ./scripts/env/install_clang17_toolchain.sh`.
>
> On windows you can download the LLVM package from the official LLVM [GitHub release page](https://github.com/llvm/llvm-project/releases/tag/llvmorg-17.0.1)

To format the code use run the following:

```shell
# Unix platforms
./scripts/format/unix.sh
```

```shell
# Windows: powershell
./scripts/format/windows.ps1
```

To run a forrmat check use the scripts mentioned above with a `--check` flag.

<br />
<br />

## Documentation

The documentation for this project can be generated using Doxygen:

1. Make sure you have `Doxygen` installed on your machine. If not you can download it from [here](https://www.doxygen.nl/download.html).

2. Generate the documentation:

    Open your terminal and use the following instructions:
    ```shell
    cd <project-root>
    doxygen Doxyfile
    ```

<br />
<br />

## Compiler support

As of now the project supports the **GNU G++** and **Clang++** compilers with `C++20` support on Linux and Windows.

> [!NOTE]
> To build the project using clang you will need to install the `clang-17` toolchain using the script or website mentioned in the [Formatting](#formatting) section.

<br />
<br />

## Licence

The `CPP-AP` project uses the [MIT Licence](https://opensource.org/license/mit/)
