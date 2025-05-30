# Tutorial

- [Setting Up CPP-AP](#setting-up-cpp-ap)
  - [CMake Integration](#cmake-integration)
  - [Downloading the Library](#downloading-the-library)
- [The Parser Class](#the-parser-class)
- [Adding Arguments](#adding-arguments)
- [Argument Parameters](#argument-parameters)
- [Predefined Parameter Values](#predefined-parameter-values)
- [Default Arguments](#default-arguments)
- [Parsing Arguments](#parsing-arguments)
- [Retrieving Argument Values](#retrieving-argument-values)
- [Examples](#examples)

<br/>

> [!IMPORTANT]
>
> This tutorial covers the most common use cases and features of the library.
> For more in-depth information and advanced usage, please refer to the full documentation.
> Instructions for building the documentation are available in the [Dev Notes](/docs/dev_notes.md#documentation) page.

<br/>

## Setting Up CPP-AP

### CMake Integration

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

### Downloading the Library

If you do not use CMake you can dowload the desired [library release](https://github.com/SpectraL519/cpp-ap/releases), extract it in a desired directory and simply add `<cpp-ap-dir>/include` to the include directories of your project.

<br/>
<br/>
<br/>

## The Parser Class

To use the argument parser in your code you need to use the `ap::argument_parser` class.

The parameters you can specify for a parser's instance are:

- Program name and description - used in the parser's configuration output (`std::cout << parser`).
- Verbosity mode - `false` by default; if set to `true` the parser's configuration output will include more detailed info about arguments' parameters in addition to their names and help messages.
- [Arguments](#adding-arguments) - specify the values/options accepted by the program.

```cpp
ap::argument_parser parser;
parser.program_name("Name of the program")
      .program_description("Description of the program")
      .verbose();
```

<br/>
<br/>
<br/>

## Adding Arguments

The parser supports both positional and optional arguments. Both argument types are identified by their names represented as strings. Arguments can be defined with only a primary name or with a primary and a secondary (short) name.

> [!NOTE]
>
> The basic rules of parsing positional and optional arguments are described in the [Parsing arguments](#parsing-arguments) section.

To add an argument to the parameter's configurations use the following syntax:

```cpp
parser.add_<positional/optional>_argument<value_type>("argument");
```

or

```cpp
parser.add_<positional/optional>_argument<value_type>("argument", "a");
```

> [!NOTE]
>
> The library supports any argument value types which meet the following requirements:
>
> - The `std::ostream& operator<<` is overloaded for the value type
> - The value type has a copy constructor and an assignment operator

> [!IMPORTANT]
>
> If the `value_type` is not provided, `std::string` will be used.

You can also add boolean flags:

```cpp
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

```cpp
parser.add_flag<false>("disable_another_option", "dao").help("disables option: another option");
/* equivalent to:
parser.add_optional_argument<bool>("disable_another_option", "dao")
      .default_value(true)
      .implicit_value(false)
      .nargs(0)
      .help("disables option: another option");
*/
```

<br/>
<br/>
<br/>

## Argument Parameters

### Common Parameters

Parameters which can be specified for both positional and optional arguments include:

#### `help` - The argument's description which will be printed when printing the parser class instance.

    ```cpp
    parser.add_positional_argument<std::size_t>("number", "n")
          .help("a positive integer value");
    ```

#### `choices` - A list of valid argument values.

The `choices` parameter takes as an argument an instance of `std::initializer_list` or any `std::ranges::range` type such that its value type is convertible to the argument's `value_type`.

```cpp
parser.add_optional_argument<char>("method", "m").choices({'a', 'b', 'c'});
```

> [!IMPORTANT]
>
> The `choices` function can be used only if the argument's `value_type` is equality comparable (defines the `==` operator).

#### Value actions - Function performed after parsing an argument's value.
  Actions are represented as functions, which take the argument's value as an argument. The available action types are:

  - `observe` actions | `void(const value_type&)` - applied to the parsed value. No value is returned - this action type is used to perform some logic on the parsed value without modifying it.

    ```cpp
    void is_valid_user_tag(const std::string& tag) {
        if (tag.empty() or tag.front() != '@')
            throw std::runtime_error(std::format("Invalid user tag: `{}` — must start with '@'", tag));
    }

    parser.add_optional_argument<std::string>("user", "u")
          .action<ap::action_type::observe>(is_valid_user_tag);
    ```

  - `transform` actions | `value_type(const value_type&)` - applied to the parsed value. The returned value will be used to initialize the argument's value.

    ```cpp
    std::string to_lower(std::string s) {
        for (auto& c : s)
            c = static_cast<char>(std::tolower(c));
        return s;
    }

    parser.add_optional_argument<std::string>("key", "k")
          .action<ap::action_type::transform>(to_lower);
    ```

  - `modify` actions | `void(value_type&)` - applied to the initialized value of an argument.

    ```cpp
    void capitalize(std::string& s) {
        s.at(0) = std::toupper(s.at(0));
    }

    parser.add_optional_argument<std::string>("name", "n")
          .action<ap::action_type::modify>(capitalize);
    ```

> [!TIP]
>
> A single argument can have multiple value actions. Instead of writing complex logic in one action, consider composing several simple, focused actions for better readability and reuse.

<br/>

### Parameters Specific for Optional Arguments

Apart from the common parameters listed above, for optional arguments you can also specify the following parameters:

#### `required` - If this option is set for an argument and it's value is not passed in the command-line, an exception will be thrown.

  ```cpp
  parser.add_optional_argument("output", "o").required();
  ```

#### `bypass_required` - If this option is set for an argument, parsing it's value will overrite the `required` option for other optional arguments and all positional arguments.

#### `nargs` - Sets the allowed number of values to be parsed for an argument. This can be set as a:

  - Concrete number:

    ```cpp
    parser.add_optional_argument("input", "i").nargs(1);
    ```

  - Bound range:

    ```cpp
    parser.add_optional_argument("input", "i").nargs(1, 3);
    ```

  - Partially bound range:

    ```cpp
    parser.add_optional_argument("input", "i").nargs(ap::nargs::at_least(1));  // n >= 1
    parser.add_optional_argument("input", "i").nargs(ap::nargs::more_than(1)); // n > 1
    parser.add_optional_argument("input", "i").nargs(ap::nargs::less_than(5)); // n < 5
    parser.add_optional_argument("input", "i").nargs(ap::nargs::up_to(5));     // n <= 5
    ```

  - Unbound range:

    ```cpp
    parser.add_optional_argument("input", "i").nargs(ap::nargs::any());
    ```

> [!IMPORTANT]
>
> The default `nargs` parameter value is `ap::nargs::any()`.

#### `default_value` - The default value for an argument which will be used if no values for this argument are parsed

  ```cpp
  parser.add_opitonal_argument("output", "o").default_value("out.txt");
  ```

#### `implicit_value` - A value which will be set for an argument if only it's flag is parsed from the command-line but no values follow.

  ```cpp
  // program.cpp
  parser.add_optional_argument("save", "s")
        .implicit_value("out.txt")
        .help("save the program's output to a file");
  ```

  In this example if you run the program with only a `-s` or `--save` flag and no value, the value will be set to `out.txt`.

#### On-flag actions - For optional arguments, apart from value actions, you can specify on-flag actions which are executed immediately after parsing an argument's flag.

  ```cpp
  void print_debug_info() noexcept {
  #ifdef NDEBUG
      std::cout << "Running in release mode.\n";
  #else
      std::cout << "Running in debug mode.\n";
  #endif
      std::exit(EXIT_SUCCESS);
  };

  parser.add_optional_argument("--debug-info")
        .action<ap::action_type::on_flag>(print_debug_info);
  ```

  Here the `print_debug_info` function will be called right after parsing the `--debug-info` flag and the program will exit, even if there are more arguments after this flag.

<br/>
<br/>
<br/>

## Predefined Parameter Values

### Actions

---

#### `print_config` | on-flag

Prints the configuration of the parser to the output stream and optionally exits with the given code.

```cpp
typename ap::action_type::on_flag::type print_config(
    const ap::argument_parser& parser,
    const std::optional<int> exit_code = std::nullopt,
    std::ostream& os = std::cout
) noexcept;
```

---

#### `check_file_exists` | observe (value type: `std::string`)

Throws if the provided file path does not exist.

```cpp
detail::callable_type<ap::action_type::observe, std::string> check_file_exists() noexcept;
```

---

#### `gt` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

Validates that the value is strictly greater than `lower_bound`.

```cpp
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> gt(const T lower_bound) noexcept;
```

---

#### `geq` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

Validates that the value is greater than or equal to `lower_bound`.

```cpp
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> geq(const T lower_bound) noexcept;
```

---

#### `lt` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

Validates that the value is strictly less than `upper_bound`.

```cpp
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> lt(const T upper_bound) noexcept
```

---

#### `leq` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

Validates that the value is less than or equal to `upper_bound`.

```cpp
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> leq(const T upper_bound) noexcept
```

---

#### `within` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

Checks if the value is within the given interval. Bound inclusivity is customizable using template parameters.

```cpp
template <ap::detail::c_arithmetic T, bool LeftInclusive = true, bool RightInclusive = true>
detail::callable_type<ap::action_type::observe, T> within(
    const T lower_bound, const T upper_bound
) noexcept
```

<br/>
<br/>
<br/>

## Default Arguments

The `CPP-AP` library defines several default arguments, which can be added to the parser's configuration as follows.

```cpp
parser.default_positional_arguments({...});
// here `...` represents a collection of ap::argument::default_positional values

parser.default_positional_arguments({...});
// here `...` represents a collection of ap::argument::default_optional values
```

> [!NOTE]
>
> These functions work with `std::initializer_list` and all other `std::ranges::range` types with the correct value type - `ap::argument::default_{positional/optional}`

The available default arguments are:

- `default_positional::input`:

  ```cpp
  // equivalent to:
  parser.add_positional_argument<std::string>("input")
        .action<ap::action_type::modify>(ap::action::check_file_exists())
        .help("Input file path");
  ```

- `default_positional::output`:

  ```cpp
  // equivalent to:
  parser.add_positional_argument("output").help("Output file path");
  ```

- `default_optional::help`:

  ```cpp
  // equivalent to:
  parser.add_flag("help", "h")
        .action<action_type::on_flag>(action::print_config(arg_parser, EXIT_SUCCESS))
        .help("Display the help message");
  ```

- `default_optional::input` and `default_optional::multi_input`:

  ```cpp
  // input - equivalent to:
  parser.add_optional_argument("input", "i")
        .required()
        .nargs(1)
        .action<ap::action_type::observe>(ap::action::check_file_exists())
        .help("Input file path");

  // multi_input - equivalent to:
  parser.add_optional_argument("input", "i")
        .required()
        .nargs(ap::nargs::at_least(1))
        .action<ap::action_type::observe>(ap::action::check_file_exists())
        .help("Input files paths");
  ```

- `default_optional::output` and `default_optional::multi_output`:

  ```cpp
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

<br/>
<br/>
<br/>

## Parsing Arguments

To parse the command-line arguments use the `void argument_parser::parse_args(const AR& argv)` method, where `AR` must be a type that satisfies `std::ranges::range` and its value type is convertible to `std::string`.

The `argument_parser` class also defines the `void parse_args(int argc, char* argv[])` overload, which works directly with the `argc` and `argv` arguments of the `main` function.

> [!IMPORTANT]
>
> The `parse_args(argc, argv)` method ignores the first argument (the program name) and is equivalent to calling `parse_args(std::span(argv + 1, argc - 1))`.

> [!TIP]
>
> The `parse_args` function may throw an `ap::argument_parser_exception` (specifically the `ap::parsing_failure` derived exception) if the provided command-line arguments do not match the expected configuration. To simplify error handling, the `argument_parser` class provides `try_parse_args` methods, which automatically catch these exceptions, print the error message, and exit with a failure status.
>
> Internally, This is equivalent to:
>
> ```cpp
> try {
>     parser.parse_args(...);
> }
> catch (const ap::argument_parser_exception& err) {
>     std::cerr << "[ERROR] : " << err.what() << std::endl << parser << std::endl;
>     std::exit(EXIT_FAILURE);
> }
> ```

```cpp
// power.cpp
#include <ap/argument_parser.hpp>

#include <cmath>
#include <iostream>

int main(int argc, char* argv[]) {
    // create the parser class instance
    ap::argument_parser parser;
    parser.program_name("power calculator")
          .program_description("Calculates the value of an expression: base ^ exponent");

    // add arguments
    parser.add_positional_argument<double>("base").help("the exponentation base value");
    parser.add_optional_argument<int>("exponent", "e")
          .nargs(ap::nargs::any())
          .help("the exponent value");

    parser.default_optional_arguments({ap::argument::default_optional::help});

    // parse command-line arguments
    parser.try_parse_args(argc, argv);

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
// g++ -o power power.cpp -I <cpp-ap-include-dir> -std=c++20
```

### Argument Parsing Rules:

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
  # Program: power calculator
  #
  #   Calculates the value of an expression: base ^ exponent
  #
  # Positional arguments:
  #
  #   base : the exponentation base value
  #
  # Optional arguments:
  #
  #   --exponent, -e : the exponent value
  #   --help, -h     : Display the help message
  ```

> [!IMPORTANT]
>
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
  # [ERROR] : Failed to deduce the argument for values [1, 2, 3]
  # Program: power calculator
  #
  #   Calculates the value of an expression: base ^ exponent
  #
  # Positional arguments:
  #
  #   base : the exponentation base value
  #
  # Optional arguments:
  #
  #   --exponent, -e : the exponent value
  #   --help, -h     : Display the help message
  ```

> [!IMPORTANT]
>
> The parser behaviour depends on the argument definitions. The argument parameters are described int the [Argument parameters](#argument-parameters) section.

<br/>
<br/>
<br/>

## Retrieving Argument Values

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

<br/>

Additionally for optional arguments, you can use:

```cpp
(const) std::vector<value_type> values = parser.values<value_type>("argument_name");
```

which returns a `vector` containing all values parsed for the given argument.

<br/>
<br/>
<br/>

## Examples

The library usage examples / demo projects can be found in the [cpp-ap-demo](https://github.com/SpectraL519/cpp-ap-demo) repository.
