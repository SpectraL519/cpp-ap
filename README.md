# CPP-AP-2

Command-line argument parser for C++20

[![g++](https://github.com/SpectraL519/cpp-ap/actions/workflows/gpp.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/g++)
[![clang++](https://github.com/SpectraL519/cpp-ap/actions/workflows/clang.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/clang++)
[![format](https://github.com/SpectraL519/cpp-ap/actions/workflows/format.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/format)
[![demo](https://github.com/SpectraL519/cpp-ap/actions/workflows/demo.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/demo)

<br />

## Overview

`CPP-AP` is a lightweight and feature-rich command-line argument parsing library for the C++20 and newer standards, designed as an alternative to `boost::program_options`. It offers a modern and intuitive interface allowing for straightforward argument configuration and parsing.

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

## Table of Contents

- [Key Changes in v2](#key-changes-in-v2)
- [Tutorial](#tutorial)
  - [Including CPP-AP into a project](#including-cpp-ap-into-a-project)
    - [CMake integration](#cmake-integration)
    - [Downloading the library](#downloading-the-library)
  - [The parser class](#the-parser-class)
  - [Adding arguments](#adding-arguments)
  - [Argument parameters](#argument-parameters)
  - [Default arguments](#default-arguments)
  - [Parsing arguments](#parsing-arguments)
  - [Retrieving argument values](#retrieving-argument-values)
- [Examples](#examples)
- [Dev notes](#dev-notes)
  - [Building and testing](#building-and-testing)
  - [Formatting](#formatting)
- [Documentation](#documentation)
- [Compiler support](#compiler-compatibilty)
- [License](#license)

<br />
<br />

## Key Changes in v2

### The Structure

The *v2* version of the library no longer uses a single-header approach, however it is still a header-only library. With this change it is now required to download the entire library - either with [CMake](#cmake-integration) or from the [releases page](https://github.com/SpectraL519/cpp-ap/releases).

### Range-based Approach

Aligned functions to use a generic range-based approach, which allows for more flexibility in using the library.

| **Function** | **Description** |
| :- | :- |
| <pre>argument_parser::parse_args(<br/>    const AR& argv<br/>)</pre> | <div align="center">**New**</div>Parses the command-line arguments passed through a range of string-convertible objects.<br/>**NOTE:** The `parse_args(argc, argv)` function is still available. |
| <pre>argument::positional::choices(<br/>    std::initializer_list&lt;value_type&gt; choices<br/>)</pre> | <div align="center">**Aligned**</div>Sets the *choices* parameter of a positional argument.<br/>**NOTE:** Previously the function used `std::vector<value_type>`. |
| <pre>argument::positional::choices(<br/>    const CR& choices<br/>)</pre> | <div align="center">**New**</div>Sets the *choices* parameter of a positional argument using a range of `value_type`-convertible objects. |
| <pre>argument::optional::choices(<br/>    std::initializer_list&lt;value_type&gt; choices<br/>)</pre> | <div align="center">**Aligned**</div>Sets the *choices* parameter of an optional argument.<br/>**NOTE:** Previously the function used `std::vector<value_type>`. |
| <pre>argument::optional::choices(<br/>    const CR& choices<br/>)</pre> | <div align="center">**New**</div>Sets the *choices* parameter of an optional argument using a range of `value_type`-convertible objects. |

### New Utility Functions

Added utility functions to the `argument_parser` class that encapsulate common behavior, reducing the need to write boilerplate code and simplifying the argument-parsing logic.

| **Function** | **Description** |
| :- | :- |
| <pre>void try_parse_args(int argc, char* argv[])<br/>void try_parse_args(const AR& argv)</pre> | Parses the command-line arguments and, in case of an error, prints the error message and exits with a failure status. |
| <pre>void handle_help_action()</pre> | Checks the value of the `help` boolean flag argument and exits with success status if the value is `true`. |
| <pre>T value_or<T, U>(<br/>    std::string_view arg_name, U&& default_value<br/>)</pre> | Returns the argument's value if possible and `default_value` otherwise. |

A more detailed explanation of these functions is available in the [Tutorial](#tutorial) section.

<br />
<br />

## Tutorial

### Including CPP-AP into a project

#### CMake integration

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

# Link against the cpp-ap (v2) library
target_link_libraries(my_project PRIVATE cpp-ap-2)
```

#### Downloading the library

If you do not use CMake you can dowload the desired [library release](https://github.com/SpectraL519/cpp-ap/releases), extract it in a desired directory and simply add `<cpp-ap-dir>/include` to the include directories of your project.

<br />

### The parser class

To use the argument parser in your code you need to use the `ap::argument_parser` class.

The parameters you can specify for a parser's instance are:

- Program name
- Program description
- [Arguments](#adding-arguments)

```c++
ap::argument_parser parser;
parser.program_name("Name of the program")
      .program_description("Description of the program");
```

<br />

### Adding arguments

The parser supports both positional and optional arguments. Both argument types are identified by their names represented as strings. Arguments can be defined with only a primary name or with a primary and a secondary (short) name.

> [!NOTE]
> The basic rules of parsing positional and optional arguments are described in the [Parsing arguments](#parsing-arguments) section.

To add an argument to the parameter's configurations use the following syntax:

```c++
parser.add_<positional/optional>_argument<value_type>("argument");
```

or

```c++
parser.add_<positional/optional>_argument<value_type>("argument", "a");
```

> [!NOTE]
> The library supports any argument value types which meet the following requirements:
>
> - The `std::ostream& operator<<` is overloaded for the value type
> - The value type has a copy constructor and an assignment operator

> [!IMPORTANT]
> If the `value_type` is not provided, `std::string` will be used.

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

<br />

### Argument parameters

#### Common parameters

Parameters which can be specified for both positional and optional arguments include:

- `help` - the argument's description which will be printed when printing the parser class instance.

    ```c++
    parser.add_positional_argument<std::size_t>("number", "n")
          .help("a positive integer value");
    ```

- `choices` - a list of valid argument values.
    The `choices` parameter takes as an argument an instance of `std::initializer_list` or any `std::ranges::range` type such that its value type is convertible to the argument's `value_type`.

    ```c++
    parser.add_optional_argument<char>("method", "m").choices({'a', 'b', 'c'});
    ```

> [!IMPORTANT]
> The `choices` function can be used only if the argument's `value_type` is equality comparable (defines the `==` operator).

- `action` - a function performed after reading an argument's value.
  Actions are represented as functions, which take the argument's value as an argument. There are two types of actions:
  - `modify` actions | `void(value_type&)` - applied to the initialized value of an argument.

    ```c++
    parser.add_optional_argument<std::string>("name", "n")
          .action<ap::action_type::modify>([](std::string& name) { name[0] = std::toupper(name[0]); });
    ```

  - `transform` actions | `value_type(const value_type&)` - applied to the parsed value. The returned value will be used to initialize the argument's value.

    ```c++
    std::string to_lower(std::string s) {
        for (auto& c : s)
            c = static_cast<char>(std::tolower(c));
        return s;
    }

    parser.add_optional_argument<std::string>("key", "k")
          .action<ap::action_type::transform>(to_lower);
    ```

  Actions can also be used to perform some value checking logic instead of actualy modifying or transforming a value, as e.g. the predefined `check_file_exists` which verifies whether a file with a given name exists:

  ```c++
  parser.add_optional_argument("input", "i")
        .action<ap::action_type::modify>(ap::action::check_file_exists());
  ```

  > **NOTE:** The default action is an empty `modify` action.

#### Optional argument specific parameters

- `required` - if this option is set for an argument and it's value is not passed in the command-line, an exception will be thrown.

  ```c++
  parser.add_optional_argument("output", "o").required();
  ```

- `bypass_required` - if this option is set for an argument, parsing it's value will overrite the `required` option for other optional arguments and all positional arguments.

- `nargs` - sets the allowed number of values to be parsed for an argument. This can be set as a:

  - Concrete number:

    ```c++
    parser.add_optional_argument("input", "i").nargs(1);
    ```

  - Bound range:

    ```c++
    parser.add_optional_argument("input", "i").nargs(1, 3);
    ```

  - Partially bound range:

    ```c++
    parser.add_optional_argument("input", "i").nargs(ap::nargs::at_least(1));  // n >= 1
    parser.add_optional_argument("input", "i").nargs(ap::nargs::more_than(1)); // n > 1
    parser.add_optional_argument("input", "i").nargs(ap::nargs::less_than(5)); // n < 5
    parser.add_optional_argument("input", "i").nargs(ap::nargs::up_to(5));     // n <= 5
    ```

  - Unbound range:

    ```c++
    parser.add_optional_argument("input", "i").nargs(ap::nargs::any());
    ```

> [!NOTE]
> The default nargs value is `1`.

- `default_value` - the default value for an argument which will be used if no values for this argument are parsed

  ```c++
  parser.add_opitonal_argument("output", "o").default_value("out.txt");
  ```

- `implicit_value` - a value which will be set for an argument if only it's flag is parsed from the command-line but no value follows

  ```c++
  // program.cpp
  parser.add_optional_argument("save", "s")
        .implicit_value("out.txt")
        .help("save the program's output to a file");
  ```

  In this example if you run the program with only a `-s` or `--save` flag and no value, the value will be set to `out.txt`.

<br />

### Default arguments

The `CPP-AP` library defines several default arguments, which can be added to the parser's configuration as follows.

```c++
parser.default_positional_arguments({...});
// here `...` represents a collection of ap::argument::default_positional values

parser.default_positional_arguments({...});
// here `...` represents a collection of ap::argument::default_optional values
```

> [!NOTE]
> These functions work with `std::initializer_list` and all other `std::ranges::range` types with the correct value type - `ap::argument::default_{positional/optional}`

The available default arguments are:

- `default_positional::input`:

  ```c++
  // equivalent to:
  parser.add_positional_argument<std::string>("input")
        .action<ap::action_type::modify>(ap::action::check_file_exists())
        .help("Input file path");
  ```

- `default_positional::output`:

  ```c++
  // equivalent to:
  parser.add_positional_argument("output").help("Output file path");
  ```

- `default_optional::help`:

  ```c++
  // equivalent to:
  parser.add_flag("help", "h").bypass_required().help("Display help message");
  ```

> [!NOTE]
> As of now the *on flag/use action* functionality is not implemented in the library - this will be added in a future release.
> To properly use the help argument in the current release add the, use the `parser.handle_help_action()` method after parsing the arguments. This would be equivalent to:
>
> ```c++
> if (parser.value<bool>("help")) {
>     std::cout << parser << std::endl;
>     std::exit(EXIT_SUCCESS);
> }
> ```

- `default_optional::input` and `default_optional::multi_input`:

  ```c++
  // input - equivalent to:
  parser.add_optional_argument("input", "i")
        .required()
        .nargs(1)
        .action<ap::action_type::modify>(ap::action::check_file_exists())
        .help("Input file path");

  // multi_input - equivalent to:
  parser.add_optional_argument("input", "i")
        .required()
        .nargs(ap::nargs::at_least(1))
        .action<ap::action_type::modify>(ap::action::check_file_exists())
        .help("Input files paths");
  ```

- `default_optional::output` and `default_optional::multi_output`:

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

<br />

### Parsing arguments

To parse the command-line arguments use the `void argument_parser::parse_args(const AR& argv)` method, where `AR` must be a type that satisfies `std::ranges::range` and its value type is convertible to `std::string`.

The `argument_parser` class also defines the `void parse_args(int argc, char* argv[])` overload, which works directly with the `argc` and `argv` arguments of the `main` function.

> [!IMPORTANT]
> The `parse_args(argc, argv)` method ignores the first argument (the program name) and is equivalent to calling `parse_args(std::span(argv + 1, argc - 1))`.

> [!TIP]
> The `argument_parser` class defines `try_parse_args` methods, which are equivalent to:
>
> ```c++
> try {
>     parser.parse_args(...);
> }
> catch (const ap::argument_parser_exception& err) {
>     std::cerr << "[ERROR] : " << err.what() << std::endl << parser << std::endl;
>     std::exit(EXIT_FAILURE);
> }
> ```

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

    parser.default_optional_arguments({ap::argument::default_optional::help});

    // parse command-line arguments
    parser.try_parse_args(argc, argv);

    // handle the `help` argument
    parser.handle_help_action();

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

- Positional arguments are parsed first, in the order they were defined in and without a flag.

  In the example above the first command-line argument must be the value for the `positional` argument:

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

- Optional arguments are parsed only with a flag:

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

### Retrieving argument values

You can retrieve the argument's value with:

```cpp
(const) auto value = parser.value<value_type>("argument_name"); // (1)
(const) auto value = parser.value_or<value_type>("argument_name", default_value); // (2)
```

1. This will return the value parsed for the given argument.

    For optional arguments this will return the argument's predefined value if no value has been parsed. Additionaly, if more than one value has been parsed for an optional argument, this function will return the first parsed value.

2. When a value has been parsed for the argument, the behaviour is the same as in case **(1)**. Otherwise, this will return `value_type{std::forward<U>(default_value)}` (where `U` is the deducted type of `default_value`), if:

    - There is no value parsed for a positional argument
    - There is no parsed values and no predefined values for an optional arrument

<br />

Additionally for optional arguments, you can use:

```cpp
(const) std::vector<value_type> values = parser.values<value_type>("argument_name");
```

which returns a `vector` containing all values parsed for the given argument.

<br />
<br />

## Examples

The library usage examples / demo projects can be found in the [cpp-ap-demo](https://github.com/SpectraL519/cpp-ap-demo) repository.

<br />
<br />

## Dev notes

### Building and testing

> [!NOTE]
> The project uses [doctest](https://github.com/doctest/doctest) framework for unit testing, however it is already installed in the [tests/external](/tests/external/) directory, so there is no need to install it sepparately.

#### Build the testing executable

```shell
cmake -B build -DBUILD_TESTS=ON
cd build
make # -j <n>
```

This will build the test executable `run` in the `<project-root>/build/tests` directory.

#### Run the tests

```shell
cd build
./tests/run # -ts=<test-suite-name>
```

> [!NOTE]
> Test suites in the project have the same names as the files they're in except for the `test_extarnal_libs_config.cpp` file which defines the `test_doctest_config` test suite.

<br />

### Formatting

> [!NOTE]
> The project uses `clang-format-18` which is natively available on the `Ubuntu-24.04 LTS` system.
>
> If the `llvm-18` toolchain isn't available on your system, the installation instructions can be found on the [llvm apt](https://apt.llvm.org/) page. After installing the toolchain run `sudo apt install clang-format-18`.

You can format the code manually using `clang-format-18` or you can use the prepared python script:

```shell
python scripts/format.py
```

To inspect the all script's options and usage run:

```shell
python scripts/format.py --help
```

> [!NOTE]
> The script requires `python >= 3.9`

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

## Compiler compatibilty

| Compiler | Min version |
| :-: | :-: |
| GNU G++ | 13 |
| Clang | 17 |

> [!NOTE]
> Although currently the project has been properly verified using only the G++ and Clang compilers it should work fine with other compilers with C++20 support like MSVC.

<br />
<br />

## License

The `CPP-AP` project uses the [MIT License](https://mit-license.org/) which can be found in the [LICENSE](/LICENSE) file
