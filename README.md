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
    * [Parsing arguments](#parsing-arguments)
* [Examples](#examples)
* [Dev notes](#dev-notes)
    * [Requirements](#requirements)
    * [Building and testing](#building-and-testing)
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

**NOTE:** The rules of parsing positional and optional arguments are described in the [Parsing arguments](#parsing-arguments) section.

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
    parser.add_optional_argument<char>("method", "m")
          .choices({'a', 'b', 'c'});
    ```

* `action`: TODO

**Optional argument specific parameters**

* `required`: TODO
* `bypass_required`: TODO
* `nargs`: TODO
* `default_value`: TODO
* `implicit_value`: TODO

### Parsing arguments

To parse the command-line arguments use the `argument_parser::parse_args` method:

```c++
#include <ap/argument_parser.hpp>

int main(int argc, char** argv) {
    ap::argument_parser parser;
    parser.program_name("test program");

    parser.add_positional_argument<int>("positional_argument", "p");
    parser.add_optional_argument("optional_argument", "o");

    try {
        parser.parse_args(argc, argv);
    }
    catch (const ap::argument_parser_error& err) {
        std::cerr << "[ERROR] : " << err.what() << std::endl << parser << std::endl;
        std::exit(EXIT_FAILURE);
    }

    std::cout << "value parsed for the positional argument: " << parser.value<int>("positional_argument") << std::endl;
    std::cout << "value parsed for the optional argument: " << parser.value("optional_argument") << std::endl;

    return 0;
}
```

<br />
<br />

## Examples

TODO

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

## Compiler support

As of now the project supports the **GNU G++** compilers with `C++20` support (g++ >= 11.3.0) on Linux and Windows.

The project does compile with **clang** compilers, however it is not officialy tested yet.

<br />
<br />

## Licence

The `CPP-AP` project uses the [MIT Licence](https://opensource.org/license/mit/)
