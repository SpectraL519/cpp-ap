# CPP-AP
Command-line argument parser for C++20

<br />

## Project description

The `CPP-AP` poject has been developed for the *Team Programming* course at the *Wrocław University of Science and Technology*.

Faculty: *W04N - Faculty of Information and Communication Technology*

Field of study: *Algorithmic Computer Science*

<br />

The goal of the project was to create a light,  intuitive and simple to use command-line argument parser library for the `C++20` and newer standards.

The `CPP-AP` library does not require installing any additional tools or heavy libraries, like with `boost::program_options`. Much like with the `Doctest` framework - the only thing you need to do is copy the `argument_parser.hpp` file into the include directory of your project and you're set to go.

<br />
<br />

## Table of contents

* [Tutorial](#tutorial)
    * [The parser class](#the-parser-class)
    * [Adding arguments](#adding-arguments)
    * [Argument parameters](#argument-parameters)
    * [Default arguments](#default-arguments)
    * [Parsing arguments](#parsing-arguments)
* [Examples](#examples)
* [Dev notes](#dev-notes)
    * [Requirements](#requirements)
    * [Building and testing](#building-and-testing)
* [Documentation](#documentation)
* [Compiler support](#compiler-support)
* [Licence](#licence)

<br />
<br />

## Tutorial

To use the `CPP-AP` library in your project, copy the [argument_parser.hpp](include/ap/argument_parser.hpp) file into your include directory, e.g. `include/ap`. No other setup is necessary - you only need to include this header in your code:

```c++
#include <ap/argument_parser.hpp>
```

If you wish to use the library across multiple projects without copying the header into each one, you can copy it into a common directory and add the `-I <argument-parser-dir>` option when compiling your project.

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

**NOTE:** The basic rules of parsing positional and optional arguments are described in the [Parsing arguments](#parsing-arguments) section.

To add an argument to the parameter's configurations use the following syntax:

```c++
parser.add_<positional/optional>_argument<value_type>("argument_name");
```

or

```c++
parser.add_<positional/optional>_argument<value_type>("argument_name", "a");
```

**NOTE:** The library supports any argument value types which meet the following requirements:
* The `std::ostream& operator<<` must be overloaded for a value type
* The type must have a copy constructor and an assignment operator

**NOTE:** If the `value_type` is not provided, `std::string` will be used.

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

    **NOTE:** To use the `choices` the `value_type` must overload the equaility comparison operator: `==`;

    ```c++
    parser.add_optional_argument<char>("method", "m").choices({'a', 'b', 'c'});
    ```

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

    Actions can also be used to perform some value checking logic, e.g. the predefined `check_file_exists_action` which checks if a file with a given name exists:
    ```c++
    parser.add_optional_argument("input", "i")
          .action<ap::void_action>(ap::action::check_file_exists_action);
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

    **NOTE:** The default nargs value is `1`.

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

// add optional arguments - pass a std::vector of default optional arguments
parser.default_positional_arguments({...});
```

The supported default arguments are:
* `positional::input`:
    ```c++
    // equivalent to:
    parser.add_positional_argument<std::string>("input")
          .action<ap::void_action>(ap::action::check_file_exists_action)
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

    **Note:** As of now the *on flag action* functionality is not implemented in the library - this will be added in a future release.
    To properly use the help argument in the current release add the following right beneath the `parser.parse_args(argc, argv)` try-catch block:
    ```c++
    if (parser.has_value("help")) {
        std::cout << parser << std::endl;
        std::exit(EXIT_SUCCESS);
    }
    ```

* `optional::input` and `optional::multi_input`:
    ```c++
    // input - equivalent to:
    parser.add_optional_argument("input", "i")
          .required()
          .nargs(1)
          .action<ap::void_action>(ap::action::check_file_exists_action)
          .help("Input file path");

    // multi_input - equivalen to:
    parser.add_optional_argument("input", "i")
          .required()
          .nargs(ap::nargs::at_least(1))
          .action<ap::void_action>(ap::action::check_file_exists_action)
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

**NOTE:** The `argument_parser::default_<positional/optional>_arguments` functions will be modified to use a variadic argument list instead of a `std::vector` in a future release.

### Parsing arguments

To parse the command-line arguments use the `argument_parser::parse_args` method:

```c++
// power.cpp
#include <ap/argument_parser.hpp>

#include <iostream>
#include <cmath>

int main(int argc, char* argv[]) {
    // create the parser class instance
    ap::argument_parser parser;
    parser.program_name("power calculator")
          .program_description("calculates the value of an expression: base & exponent");

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
    if (parser.has_value("help")) {
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

    **NOTE:** For each positional argument there must be **exactly one value**.
    ```shell
    ./test_program
    # out:
    # [ERROR] : No values parsed for a required argument [base]
    # power calculator
    # calculates the value of an expression: base & exponent
    #         [base] : the exponentation base value
    #         [exponent,e] : the exponent value
    #         [help,h] : Display help message
    ```

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

**NOTE:** The parser behaviour depends on the argument definitions. The argument parameters are described int the [Argument parameters](#argument-parameters) section.

<br />
<br />

## Examples

If you wish to test the parser functionality with some real examples then follow these steps:

Open your terminal in the project's example directory:
```shell
cd <project-root>/example
```

The examples' source files are in the `<project-root>/example/src` directory.

> **Note:** Each source file is a sepparate example.

To build the examples use the `make all` command in the example base directory. The generated executable files will appear in the `<project-root>/example/exe` directory and will have the same names as their corresponding source files.

<br />
<br />

## Dev notes

#### Requirements:
   * Supported compiler (check compiler support [here](#compiler-support))
   * clang-format-17 ([ubuntu download tutorial](https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/amp/?fbclid=IwAR1ZfJpoiitjwn8aMlKVWpFdkYmUqtaQwraJBju09v1gtc0jQANTgVeCuMY))

<br />

#### Building and testing:

1. Build the testing executable:

    * With GNU Make
        ```shell
        > cd <project-root>/test
        > make all
        ```

    * With CMake
        ```shell
        > cd <project-root>/test/
        > mkdir build
        > cmake ..
        > make
        ```

2. Run tests

    ```shell
    cd <project-root>/test
    ```

    Run all tests:
    ```shell
    > ./test
    ```

    Run a single test suite:
    ```shell
    > ./test -ts="<test-suite-name>"
    ```

    > **Note**: Test suites in the project have the same name as the files they're in.

3. Tips and tricks:

    * Using a non-default compiler with GNU Make:

        If you wish to use a non-default compiler when building the project with GNU Make, run the Makefile with a `CXX=<compiler>` option, e.g.:
        ```shell
        > make all CXX=clang++
        ```
        > **Note:** The project uses the `g++` compiler by default.

    * Using a non-default compiler with CMake:

        If you wish to use a non-default compiler when building the project with CMake, run the cmake command with a `-DCMAKE_CXX_COMPILER=<compiler>` option, e.g:
        ``` shell
        > cmake -DCMAKE_CXX_COMPILER=clang++ ..
        ```
        > **Note:** The project uses the `g++` compiler by default.

        You can also change used flags in current compiler in the following way:
        ```
        > cmake -DCMAKE_CXX_FLAGS="<flag1> <flag2> ..." ..
        ```
        Or both things at once:
        ```
        > cmake -DCMAKE_CXX_COMPILER=<compiler> -DCMAKE_CXX_FLAGS="<flag1> <flag2> ..." ..
        ```

    * Changing the CMake generator:

        If you wish for CMake to generate a different type of project, use the `-G` option, e.g. (building a Make project on Windows instead of a VS project):
        ```
        > cmake -G "Unix Makefiles" ..
        ```

4. Possible errors:

    * Windows Make - running scripts error:

        If you see an error:
        ```shell
        ...\profile.ps1 cannot be loaded because running scripts is disabled on this system. ...
        ```
        when building the project on Windows using GNU Make, run the following command:
        ```shell
        > PowerShell -ExecutionPolicy Bypass
        ```
        This modifies the execution policy **for the current powershell session**.

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

As of now the project supports the **GNU G++** compilers with `C++20` support (g++ >= 11.3.0) on Linux and Windows.

The project does compile with **clang** compilers, however it is not officialy tested yet.

<br />
<br />

## Licence

The `CPP-AP` project uses the [MIT Licence](https://opensource.org/license/mit/)
