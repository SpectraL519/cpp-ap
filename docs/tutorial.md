# Tutorial

- [Setting Up CPP-AP](#setting-up-cpp-ap)
  - [CMake Integration](#cmake-integration)
  - [Bazel Build System](#bazel-build-system)
  - [Downloading the Library](#downloading-the-library)
- [The Parser Class](#the-parser-class)
- [Adding Arguments](#adding-arguments)
  - [Syntax](#syntax)
  - [Names](#names)
  - [Value Types](#value-types)
  - [Boolean Flags](#boolean-flags)
- [Argument Parameters](#argument-parameters)
  - [Common Parameters](#common-parameters)
    - [help](#1-help---the-arguments-description-which-will-be-printed-when-printing-the-parser-class-instance)
    - [hidden](#2-hidden---if-this-option-is-set-for-an-argument-then-it-will-not-be-included-in-the-program-description)
    - [required](#3-required---if-this-option-is-set-for-an-argument-and-its-value-is-not-passed-in-the-command-line-an-exception-will-be-thrown)
    - [bypass required](#4-bypass_required---if-this-option-is-set-for-an-argument-the-required-option-for-other-arguments-will-be-discarded-if-the-bypassing-argument-is-used-in-the-command-line)
    - [nargs](#5-nargs---sets-the-allowed-number-of-values-to-be-parsed-for-an-argument)
    - [greedy](#6-greedy---if-this-option-is-set-the-argument-will-consume-all-command-line-values-until-its-upper-nargs-bound-is-reached)
    - [choices](#7-choices---a-list-of-valid-argument-values)
    - [value actions](#8-value-actions---functions-that-are-called-after-parsing-an-arguments-value)
    - [default values](#9-default_values---a-list-of-values-which-will-be-used-if-no-values-for-an-argument-have-been-parsed)
  - [Parameters Specific for Optional Arguments](#parameters-specific-for-optional-arguments)
    - [on-flag actions](#1-on-flag-actions---functions-that-are-called-immediately-after-parsing-an-arguments-flag)
    - [implicit values](#2-implicit_values---a-list-of-values-which-will-be-set-for-an-argument-if-only-its-flag-but-no-values-are-parsed-from-the-command-line)
- [Predefined Parameter Values](#predefined-parameter-values)
- [Default Arguments](#default-arguments)
- [Argument Groups](#argument-groups)
  - [Creating New Groups](#creating-new-groups)
  - [Adding Arguments to Groups](#adding-arguments-to-groups)
  - [Group Attributes](#group-attributes)
  - [Complete Example](#complete-example)
- [Parsing Arguments](#parsing-arguments)
  - [Basic Argument Parsing Rules](#basic-argument-parsing-rules)
  - [Compound Arguments](#compound-arguments)
  - [Parsing Known Arguments](#parsing-known-arguments)
- [Retrieving Argument Values](#retrieving-argument-values)
- [Subparsers](#subparsers)
  - [Creating Subparsers](#creating-subparsers)
  - [Using Multiple Subparsers](#using-multiple-subparsers)
  - [Parsing Arguments with Subparsers](#parsing-arguments-with-subparsers)
  - [Tracking Parser State](#tracking-parser-state)
- [Examples](#examples)
- [Common Utility](#common-utility)

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
    GIT_TAG master # here you can specify the desired tag or branch name
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

### Bazel Build System

To use the `CPP-AP` in a [Bazel](https://bazel.build/) project add the following in the `MODULE.bazel` (or `WORKSPACE.bazel`) file:

```bazel
git_repository = use_repo_rule("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "cpp-ap",
    remote = "https://github.com/SpectraL519/cpp-ap.git",
    tag = "<version-name>" # here you can declare the desired CPP-AP version
)
```

> [!IMPORTANT]
> CPP-AP versions older than [2.5.0](https://github.com/SpectraL519/cpp-ap/releases/tag/v2.5.0) DO NOT support building with Bazel.

And then add the `"@cpp-ap//:cpp-ap"` dependency for the target you want to use `CPP-AP` for by adding it to the `deps` list. For instance:

```bazel
# BUILD.bazel
cc_binary(
    name = "my_app",
    srcs = ["application.cpp"],
    includes = ["include"],
    deps = ["@cpp-ap//:cpp-ap"],
    cxxopts = ["-std=c++20"],
    visibility = ["//visibility:public"],
)
```

### Downloading the Library

If you do not use CMake you can dowload the desired [library release](https://github.com/SpectraL519/cpp-ap/releases), extract it in a desired directory and simply add `<cpp-ap-dir>/include` to the include directories of your project.

<br/>
<br/>
<br/>

## The Parser Class

To use the argument parser in your code you need to use the `ap::argument_parser` class.

```cpp
ap::argument_parser parser("program");
parser.program_version("alhpa")
      .program_description("Description of the program")
      .verbose();
```

> [!IMPORTANT]
>
> - When creating an argument parser instance, you must provide a program name to the constructor.
>
>   The program name given to the parser cannot be empty and cannot contain whitespace characters.
>
> - Additional parameters you can specify for a parser's instance incldue:
>   - The program's version and description - used in the parser's configuration output (`std::cout << parser`).
>   - Verbosity mode - `false` by default; if set to `true` the parser's configuration output will include more detailed info about arguments' parameters in addition to their names and help messages.
>   - [Arguments](#adding-arguments) - specify the values/options accepted by the program.
>   - [Argument Groups](#argument-groups) - organize related optional arguments into sections and optionally enforce usage rules.
>   - [The unknown argument flags handling policy](#4-unknown-argument-flag-handling).

> [!TIP]
>
> You can specify the program version using a string (like in the example above) or using the `ap::version` structure:
>
> ```cpp
> parser.program_version({0u, 0u, 0u})
> parser.program_version({ .major = 1u, .minor = 1u, .patch = 1u });
> ap::version ver{2u, 2u, 2u};
> parser.program_version(ver);
> ```
>
> **NOTE:** The `ap::version` struct
> - contains the three members - `major`, `minor`, `patch` - all of which are of type `std::uint32_t`,
> - defines a `std::string str() const` method which returns a `v{major}.{minor}.{path}` version string,
> - defines the `std::ostream& operator<<` for stream insertion.

<br/>
<br/>
<br/>

## Adding Arguments

The parser supports **positional** and **optional** arguments.

> [!NOTE]
>
> The general rules for parsing arguments are described in the [Parsing arguments](#parsing-arguments) section.

### Syntax

To add an argument, use:

```cpp
parser.add_positional_argument<value_type>("name");
parser.add_optional_argument<value_type>("name");
```

For **optional arguments**, you may also specify a secondary (short) name:

```cpp
parser.add_optional_argument<value_type>("name", "n")
```

or use only the secondary name:

```cpp
parser.add_optional_argument("n", ap::n_secondary);
```

### Names

- Positional arguments must have exactly one name (no secondary/short names allowed).
- Optional arguments can have:
  - only a primary (long) name,
  - only a secondary (short) name, or
  - both a primary and a secondary name.

### Value Types

> [!IMPORTANT]
> An argument's value type must be `ap::none_type` **or** satisfy all of the following requirements:
>
> - [Constructible from](https://en.cppreference.com/w/cpp/concepts/constructible_from) `const std::string&` or overload `std::istream& operator>>`.
>   - The parser will always try direct initialization from std::string first, and only fall back to the extraction operator if direct initialization fails.
> - Satisfy the [`std::semiregular`](https://en.cppreference.com/w/cpp/concepts/semiregular.html) concept (default-initializable and copyable).

> [!NOTE]
>
> - The default value type of any argument is `std::string`.
> - If the argument's value type is `ap::none_type`, the argument will not accept any values and therefore no value-related parameters can be set for such argument. This includes:
>   - [nargs](#5-nargs---sets-the-allowed-number-of-values-to-be-parsed-for-an-argument-this-can-be-set-as-a)
>   - [greedy](#6-greedy---if-this-option-is-set-the-argument-will-consume-all-command-line-values-until-its-upper-nargs-bound-is-reached)
>   - [choices](#7-choices---a-list-of-valid-argument-values)
>   - [value actions](#8-value-actions---functions-that-are-called-after-parsing-an-arguments-value)
>   - [default values](#9-default_values---a-list-of-values-which-will-be-used-if-no-values-for-an-argument-have-been-parsed)
>   - [implicit values](#2-implicit_values---a-list-of-values-which-will-be-set-for-an-argument-if-only-its-flag-but-no-values-are-parsed-from-the-command-line)

### Boolean Flags

Flags are essentialy optional arguments with a boolean value type.

```cpp
parser.add_flag("enable_some_option", "eso").help("enables option: some option");
/* equivalent to:
parser.add_optional_argument<bool>("enable_some_option", "eso")
      .default_values(false)
      .implicit_values(true)
      .nargs(0)
      .help("enables option: some option");
*/
```

By default, flags store `true` when parsed from the command-line. You can invert this behavior:

```cpp
parser.add_flag<false>("disable_another_option", "dao").help("disables option: another option");
/* equivalent to:
parser.add_optional_argument<bool>("disable_another_option", "dao")
      .default_values(true)
      .implicit_values(false)
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

#### 1. `help` - The argument's description which will be printed when printing the parser class instance.

```cpp
parser.add_positional_argument<std::size_t>("number", "n")
      .help("a positive integer value");
```

<br />

#### 2. `hidden` - If this option is set for an argument, then it will not be included in the program description.

By default all arguments are visible, but this can be modified using the `hidden(bool)` setter as follows:

```cpp
ap::argument_parser("hidden-test")
parser.program_description("A simple test program for argument hiding")
      .default_arguments(ap::default_argument::o_help);

parser.add_optional_argument("hidden")
      .hidden()
      .help("A simple hidden argument");
parser.add_optional_argument("visible")
      .help("A simple visible argument");

parser.try_parse_args(argc, argv);

/*
> ./hidden-test --help
Program: hidden-test

  A simple test program for argument hiding

Optional arguments:

  --help, -h : Display the help message
  --visible  : A simple visible argument
```

<br />

#### 3. `required` - If this option is set for an argument and it's value is not passed in the command-line, an exception will be thrown.

> [!IMPORTANT]
>
> - By default positional arguments are set to be required, while optional arguments have this option disabled by default.
> - The default value of the value parameter of the `required(bool)` function is `true` for both positional and optional arguments.

> [!WARNING]
>
> - If a positional argument is defined as non-required, then no required positional argument can be defined after (only other non-required positional arguments and optional arguments will be allowed).
> - For both positional and optional arguments:
>   - enabling the `required` option disables the `bypass_required` option
>   - disabling the `required` option has no effect on the `bypass_required` option.

```cpp
// example: positional arguments
parser.add_positional_argument("input");
parser.add_positional_argument("output").required(false);

parser.parse_args(argc, argv);

// input is guaranteed to have a value if parsing was successfull
const auto data = read_data(parser.value("input"));

if (parser.has_value("output")) {
    std::ofstream os(parser.value("output"));
    os << data << std::endl;
}
else {
    std::cout << data << std::endl;
}

/*
Command                           Result
-----------------------------------------------------------------------------------------
./program                         Parsing error (no value for the input argument)
./program input.txt               Parsing success; Printing data to stdout
./program input.txt output.txt    Parsing success; Printing data to the `output.txt` file
```

```cpp
// example: optional arguments
parser.add_optional_argument("input", "i").required();
parser.add_optional_argument("output", "o");

parser.parse_args(argc, argv);

// `input` is guaranteed to have a value if parsing was successfull
const auto data = read_data(parser.value("input"));

if (parser.has_value("output")) {
    std::ofstream os(parser.value("output"));
    os << data << std::endl;
}
else {
    std::cout << data << std::endl;
}

/*
Command                                 Result
-----------------------------------------------------------------------------------------------
./program                               Parsing error (no value for the input argument)
./program --input input.txt             Parsing success; Printing data to stdout
./program -i input.txt -o output.txt    Parsing success; Printing data to the `output.txt` file
*/
```

<br />

#### 4. `bypass_required` - If this option is set for an argument, the `required` option for other arguments will be discarded if the bypassing argument is used in the command-line.

> [!NOTE]
>
> - Both all arguments have the `bypass_required` option disabled.
> - The default value of the value parameter of the `argument::bypass_required(bool)` method is `true` for all arguments.

> [!WARNING]
>
> For both positional and optional arguments:
> - enabling the `bypass_required` option disables the `required` option
> - disabling the `bypass_required` option has no effect on the `required` option.

```cpp
// example: optional arguments
parser.add_positional_argument("input");
parser.add_optional_argument("output", "o").required();
parser.add_optional_argument("version", "v").bypass_required();

parser.parse_args(argc, argv);

if (parser.count("version")) {
    std::cout << PROJECT_VERSION << std::endl;
    std::exit(EXIT_SUCCESS);
}

// may result in an `ap::argument_parser_exception`:
// `input` is not guaranteed to have a value at this point
const auto data = read_data(parser.value("input"));

// may result in an `ap::argument_parser_exception`:
// `output` is not guaranteed to have a value at this point
std::ofstream os(parser.value("output"));
os << data << std::endl;
```

<br />

#### 5. `nargs` - Sets the allowed number of values to be parsed for an argument.

The `nargs` parameter can be set as:

- Specific number:

  ```cpp
  parser.add_optional_argument("input", "i").nargs(1);
  ```

- Fully bound range:

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
> The default `nargs` parameter value is:
>
> - `ap::nargs::range(1ull)` for positional arguments
>
> - `ap::nargs::any()` for optional arguments

<br />

#### 6. `greedy` - If this option is set, the argument will consume ALL command-line values until it's upper nargs bound is reached.

> [!NOTE]
>
> - By default the `greedy` option is disabled for all arguments.
> - The default value of the parameter of the `argument::greedy(bool)` method is true for all arguments.

> [!TIP]
>
> - Enabling the `greedy` option for an argument only makes sense for arguments with string-like value types.
> - If no explicit `nargs` bound is set for a greedy argument, once it starts being parsed, it will consume all remaining command-line arguments.

Consider a simple example:

```cpp
ap::argument_parser parser("run-script");
parser.default_arguments(ap::default_argument::o_help);

parser.add_positional_argument("script")
      .help("The name of the script to run");
parser.add_optional_argument("args")
      .greedy()
      .help("Set the execution option");

parser.try_parse_args(argc, argv);

// Application logic here
std::cout << "Executing: " << parser.value("script") << " " << ap::util::join(parser.values("args")) << std::endl;
```

Here the program execution should look something like this:

```txt
> ./run-script remove-comments --args module.py -v --type py
Executing: remove-comments module.py -v --type py
```

Notice that even though the `-v` and `--type` command-line arguments have flag prefixes and are not defined in the program, they are not treated as unknown arguments (and therefore no exception is thrown) because the `--args` argument is marked as `greedy` and it consumes these command-line arguments as its values.

<br />

#### 7. `choices` - A list of valid argument values.

```cpp
parser.add_optional_argument<char>("method", "m").choices('a', 'b', 'c');
// equivalent to: parser.add_optional_argument<char>("method", "m").choices({'a', 'b', 'c'});
// passing a value other than a, b or c for the `method` argument will result in an error
```

> [!IMPORTANT]
>
> - The `choices` function can be used only if the argument's `value_type` is equality comparable (defines the `==` operator)
> - The `choices` function can be called with:
>   - A variadic number of values [convertible to](https://en.cppreference.com/w/cpp/concepts/convertible_to.html) the argument's value type
>   - An arbitrary [`std::ranges::range`](https://en.cppreference.com/w/cpp/ranges/range.html) type with a value type [convertible to](https://en.cppreference.com/w/cpp/concepts/convertible_to.html) the argument's value type

<br />

#### 8. value actions - Functions that are called after parsing an argument's value.
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
> A single argument can have multiple value actions. Instead of writing complex logic in one action, consider composing several simple, focused actions for better readability and reusability.

<br />

#### 9. `default_values` - A list of values which will be used if no values for an argument have been parsed

> [!WARNING]
>
> For both positional and optional arguments, setting the `default_values` parameter disables the `required` option.

```cpp
// example: positional arguments
parser.add_positional_argument("input");
parser.add_positional_argument("output").default_values("output.txt");

parser.parse_args(argc, argv);

// `input` is guaranteed to have a value if parsing was successfull
const auto data = read_data(parser.value("input"));

// `output` is guaranteed to have a value even if one was not specified in the command-line
std::ofstream os(parser.value("output"));
os << data << std::endl;

/*
Command                           Result
-----------------------------------------------------------------------------------------
./program                         Parsing error (no value for the input argument)
./program input.txt               Parsing success; Printing data to `output.txt`
./program input.txt myfile.txt    Parsing success; Printing data to the `myfile.txt` file
```

```cpp
// example: optional arguments
parser.add_optional_argument("input", "i").required();
parser.add_optional_argument("output", "o").default_values("output.txt");

parser.parse_args(argc, argv);

// `input` is guaranteed to have a value if parsing was successfull
const auto data = read_data(parser.value("input"));

// `output` is guaranteed to have a value even if one was not specified in the command-line
std::ofstream os(parser.value("output"));
os << data << std::endl;

/*
Command                                 Result
-----------------------------------------------------------------------------------------------
./program                               Parsing error (no value for the input argument)
./program --input input.txt             Parsing success; Printing data to `output.txt`
./program -i input.txt -o myfile.txt    Parsing success; Printing data to the `myfile.txt` file
```

> [!NOTE]
>
> The `default_values` function can be called with:
> - A variadic number of values [convertible to](https://en.cppreference.com/w/cpp/concepts/convertible_to.html) the argument's value type
> - An arbitrary [`std::ranges::range`](https://en.cppreference.com/w/cpp/ranges/range.html) type with a value type [convertible to](https://en.cppreference.com/w/cpp/concepts/convertible_to.html) the argument's value type

<br/>
<br />

### Parameters Specific for Optional Arguments

Apart from the common parameters listed above, for optional arguments you can also specify the following parameters:

#### 1. on-flag actions - Functions that are called immediately after parsing an argument's flag.

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

<br />

#### 2. `implicit_values` - A list of values which will be set for an argument if only its flag but no values are parsed from the command-line.

```cpp
// example
parser.add_optional_argument("save", "s").implicit_values("output.txt");

parser.parse_args(argc, argv);

const auto data = get_data(); // arbitrary program data

// `output` is not guaranteed to have a value
if (parser.has_value("save")) {
    std::ofstream os(parser.value("save"));
    os << data << std::endl;
}

/*
Command                       Result
--------------------------------------------------------------------
./program                     No data will be saved
./program -s                  The data will be saved to `output.txt`
./program --save myfile.txt   The data will be saved to `myfile.txt`
```

> [!NOTE]
>
> The `implicit_values` function can be called with:
> - A variadic number of values [convertible to](https://en.cppreference.com/w/cpp/concepts/convertible_to.html) the argument's value type
> - An arbitrary [`std::ranges::range`](https://en.cppreference.com/w/cpp/ranges/range.html) type with a value type [convertible to](https://en.cppreference.com/w/cpp/concepts/convertible_to.html) the argument's value type

> [!TIP]
>
> The `implicit_values` parameter is extremely useful when combined with default value (e.g. in case of boolean flags - see [Adding Arguments](#adding-arguments)).

<br/>
<br/>
<br/>

## Predefined Parameter Values

### Actions

- `print_help` | on-flag

  Prints the parser's help message to the output stream and optionally exits with the given code.

  ```cpp
  typename ap::action_type::on_flag::type print_help(
      const ap::argument_parser& parser,
      const std::optional<int> exit_code = std::nullopt,
      std::ostream& os = std::cout
  ) noexcept;
  ```

- `check_file_exists` | observe (value type: `std::string`)

  Throws if the provided file path does not exist.

  ```cpp
  ap::action::util::callable_type<ap::action_type::observe, std::string> check_file_exists() noexcept;
  ```

- `gt` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

  Validates that the value is strictly greater than `lower_bound`.

  ```cpp
  template <ap::util::c_arithmetic T>
  ap::action::util::callable_type<ap::action_type::observe, T> gt(const T lower_bound) noexcept;
  ```

- `geq` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

  Validates that the value is greater than or equal to `lower_bound`.

  ```cpp
  template <ap::util::c_arithmetic T>
  ap::action::util::callable_type<ap::action_type::observe, T> geq(const T lower_bound) noexcept;
  ```

- `lt` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

  Validates that the value is strictly less than `upper_bound`.

  ```cpp
  template <ap::util::c_arithmetic T>
  ap::action::util::callable_type<ap::action_type::observe, T> lt(const T upper_bound) noexcept
  ```

- `leq` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

  Validates that the value is less than or equal to `upper_bound`.

  ```cpp
  template <ap::util::c_arithmetic T>
  ap::action::util::callable_type<ap::action_type::observe, T> leq(const T upper_bound) noexcept
  ```

- `within` | observe (value type: [arithmetic](https://en.cppreference.com/w/cpp/types/is_arithmetic))

  Checks if the value is within the given interval. Bound inclusivity is customizable using template parameters.

  ```cpp
  template <ap::util::c_arithmetic T, bool LeftInclusive = true, bool RightInclusive = true>
  ap::action::util::callable_type<ap::action_type::observe, T> within(
      const T lower_bound, const T upper_bound
  ) noexcept
  ```

<br/>
<br/>
<br/>

## Default Arguments

The `CPP-AP` library defines several default arguments, which can be added to the parser's configuration as follows.

```cpp
parser.default_arguments(<args>);
```

> [!NOTE]
>
> The `default_arguments` function can be called with:
> - A variadic number of `ap::default_argument` values
> - An arbitrary [`std::ranges::range`](https://en.cppreference.com/w/cpp/ranges/range.html) type with the `ap::default_argument` value type

- `p_input`:

  ```cpp
  // equivalent to:
  parser.add_positional_argument<std::string>("input")
        .action<ap::action_type::modify>(ap::action::check_file_exists())
        .help("Input file path");
  ```

- `p_output`:

  ```cpp
  // equivalent to:
  parser.add_positional_argument("output").help("Output file path");
  ```

- `o_help`:

  ```cpp
  // equivalent to:
  parser.add_optional_argument<ap::none_type>("help", "h")
        .action<action_type::on_flag>(ap::action::print_help(parser, EXIT_SUCCESS))
        .help("Display the help message");
  ```

- `o_input` and `o_multi_input`:

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

- `o_output` and `o_multi_output`:

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

## Argument Groups

Argument groups provide a way to organize related optional arguments into logical sections. They make the command-line interface easier to read in help messages, and can enforce rules such as **mutual exclusivity** or **required usage**.

By default, every parser comes with two predefined groups:

- **Positional Arguments** – contains all arguments added via `add_positional_argument`.
- **Optional Arguments** – contains all arguments added via `add_optional_argument` or `add_flag` without explicitly specifying an argument group.

User-defined groups can only contain optional arguments (including flags). This allows you to structure your command-line interface into meaningful sections such as "Input Options", "Output Options", or "Debug Settings".


### Creating New Groups

A new group can be created by calling the `add_group` method of an argument parser:

```cpp
ap::argument_parser parser("myprog");
auto& out_opts = parser.add_group("Output Options");
```

The group’s name will appear as a dedicated section in the help message and arguments added to this group will be listed under `Output Options` instead of the default `Optional Arguments` section.

> [!NOTE]
>
> If a group has no visible arguments, it will not be included in the parser's help message output at all.

### Adding Arguments to Groups

Arguments are added to a group by passing the group reference as the first parameter to the `add_optional_argument` and `add_flag` functions:

```cpp
parser.add_optional_argument(out_opts, "output", "o")
      .nargs(1)
      .help("Print output to the given file");

parser.add_flag(out_opts, "print", "p")
      .help("Print output to the console");
```

### Group Attributes

User-defined groups can be configured with special attributes that change how the parser enforces their usage:

- `required()` – at least one argument from the group must be provided by the user, otherwise parsing will fail.
- `mutually_exclusive()` – at most one argument from the group can be provided; using more than one at the same time results in an error.

Both attributes are **off by default**, and they can be combined (e.g., a group can require that exactly one argument is chosen).

```cpp
auto& out_opts = parser.add_group("Output Options")
                       .required()            // at least one option is required
                       .mutually_exclusive(); // but at most one can be chosen
```

> [!IMPORANT]
>
> If a group is defined as **mutually exclusive** and an argument from this group is used, then the `required` and `nargs` attribute requirements of other arguments from the group **will NOT be verified**.
>
> Consider the example in the section below. Normally the `--output, -o` argument would expect a value to be given in the command-line. However, if the `--print, -p` flag is used, then the `nargs` requirement of the `--output, -o` argument will not be verified, and therefore no exception will be thrown, even though the `nargs` requirement is not satisfied.

### Complete Example

Below is a small program that demonstrates how to use a mutually exclusive group of required arguments:

```cpp
#include <ap/argument_parser.hpp>

int main(int argc, char* argv[]) {
    ap::argument_parser parser("myprog");
    parser.default_arguments(ap::default_argument::o_help);

    // create the argument group
    auto& out_opts = parser.add_group("Output Options")
                           .required()
                           .mutually_exclusive();

    // add arguments to the custom group
    parser.add_optional_argument(out_opts, "output", "o")
          .nargs(1)
          .help("Print output to a given file");

    parser.add_flag(out_opts, "print", "p")
          .help("Print output to the console");

    parser.try_parse_args(argc, argv);

    return 0;
}
```

When invoked with the `--help` flag, the above program produces a help message that clearly shows the group and its rules:

```
Program: myprog

Output Options: (required, mutually exclusive)

  --output, -o : Print output to a given file
  --print, -p  : Print output to the console
```

<br/>
<br/>
<br/>

## Parsing Arguments

To parse the command-line arguments use the `void argument_parser::parse_args(const AR& argv)` method, where `AR` must be a type that satisfies the [`std::ranges::forward_range`](https://en.cppreference.com/w/cpp/ranges/forward_range.html) concept and its value type is convertible to `std::string`.

The `argument_parser` class also defines the `void parse_args(int argc, char* argv[])` overload, which works directly with the `argc` and `argv` arguments of the `main` function.

> [!IMPORTANT]
>
> The `parse_args(argc, argv)` method ignores the first argument (the program name) and is equivalent to calling:
>
> ```cpp
> parse_args(std::span(argv + 1, argc - 1));
> ```

> [!TIP]
>
> The `parse_args` function may throw an `ap::argument_parser_exception` if the configuration of the defined arguments is invalid or the parsed command-line arguments do not match the expected configuration. To simplify error handling, the `argument_parser` class provides a `try_parse_args` methods, which will automatically catch these exceptions, print the error message as well as the help message of the deepest used parser (see [Subparsers](#subparsers)), and exit with a failure status.
>
> Internally, This is equivalent to:
>
> ```cpp
> try {
>     parser.parse_args(...);
> }
> catch (const ap::argument_parser_exception& err) {
>     std::cerr << "[ap::error] " << err.what() << std::endl << parser.resolved_parser() << std::endl;
>     std::exit(EXIT_FAILURE);
> }
> ```

The simple example below demonstrates how (in terms of the program's structure) the argument parsing should look like.

```cpp
// include the main library header
#include <ap/argument_parser.hpp>

int main(int argc, char* argv[]) {
    // create the parser class instance
    ap::argument_parser parser("some-program");

    // define the parser's attributes and default arguments
    parser.program_version({0u, 0u, 0u})
          .program_description("The program does something with command-line arguments")
          .default_arguments(ap::default_argument::o_help);

    // define the program arguments
    parser.add_positional_argument("positional").help("A positional argument");
    parser.add_optional_argument("optional", "o").help("An optional argument");
    parser.add_flag("flag", "f").help("A boolean flag");

    // parse command-line arguments
    parser.try_parse_args(argc, argv);

    // use the program's arguments
    std::cout << "positional: " << parser.value("positional") << std::endl
              << "optional: " << ap::util::join(parser.values("optional")) << std::endl
              << "flag: " << std::boolalpha << parser.value<bool>("flag") << std::endl;

    return 0;
}
```

### Basic Argument Parsing Rules

#### 1. Optional arguments are parsed only with a flag

An optional argument is recognized only when its primary or secondary flag appears in the command-line input. For example:

```cpp
parser.add_optional_argument("optional", "o");
```

Here, the argument is parsed only if either `--optional` (primary flag) or `-o` (secondary flag) is present. If neither flag is given, the argument is ignored.

> [!IMPORTANT]
>
> The parser will try to assign the values following such flag to the specified argument until:
>
> - A different argument flag is encountered:
>
> ```cpp
> // program.cpp
> parser.add_optional_argument("first", "f");
> parser.add_optional_argument("second", "s");
>
> parser.try_parse_args(argc, argv);
>
> std::cout << "first: " << ap::util::join(parser.values("first")) << std::endl
>           << "second: " << ap::util::join(parser.values("second")) << std::endl;
>
> /* Example execution:
> > ./program --first value1 value2 --second value3 value4
> first: value1, value2
> second: value3, value4
> ```
>
> - The upper bound of the argument's [nargs](#1-nargs---sets-the-allowed-number-of-values-to-be-parsed-for-an-argument-this-can-be-set-as-a) parameter is reached:
>
> **NOTE:** By default an optional argument accepts an arbitrary number of values (the number of values has no upper bound).
>
> ```cpp
> parser.add_optional_argument<int>("numbers", "n")
>       .nargs(ap::nargs::up_to(3))
>       .help("A list of numbers");
> ```
> ```txt
> > ./program --numbers 1 2 3 4 5
> [ERROR] : Failed to deduce the argument for values [4, 5]
> Program: program
>
>   An example program
>
> Optional arguments:
>
>   --help, -h    : Display the help message
>   --numbers, -n : A list of numbers
> ```

<br />

#### 2. Positional arguments are parsed in the order of definition

Positional arguments are assigned values in the same order they are defined in the program. They are parsed from the command-line input **excluding any values that have already been consumed by optional arguments**. This means positional arguments no longer need to appear at the beginning of the argument list.

For example:

```cpp
parser.add_positional_argument("positional1");
parser.add_positional_argument("positional2");

parser.try_parse_args(argc, argv);

std::cout << "positional1: " << parser.value("positional1") << std::endl
          << "positional2: " << parser.value("positional2") << std::endl;

/* Example execution:
> ./program value1 value2
positional1: value1
positional2: value2
```

> [!IMPORTANT]
>
> - All positional arguments expect **at most one value**.
> - A positional argument's value doesn't have to be preset in the command-line only if the argument is defined as **not** [required](#3-required---if-this-option-is-set-for-an-argument-and-its-value-is-not-passed-in-the-command-line-an-exception-will-be-thrown).

<br />

#### 3. Positional arguments consume free values

A positional argument consumes only those values that cannot be assigned to optional arguments. This allows positional arguments to appear after optional arguments in the command-line input.

```cpp
parser.add_positional_argument("positional1");
parser.add_positional_argument("positional2");
parser.add_optional_argument("optional").nargs(1); // limit the number of arguments

parser.try_parse_args(argc, argv);

std::cout << "positional1: " << parser.value("positional1") << std::endl
          << "positional2: " << parser.value("positional2") << std::endl
          << "optional: " << parser.value("optional") << std::endl;

/* Example executions:
> ./program pos1-value pos2-value --optional opt-value
positional1: pos1-value
positional2: pos2-value
optional: opt-value

> ./program --optional opt-value pos1-value pos2-value
positional1: pos1-value
positional2: pos2-value
optional: opt-value

> ./program pos1-value --optional opt-value pos2-value
positional1: pos1-value
positional2: pos2-value
optional: opt-value
```

> [!TIP]
>
> Because of the optional arguments accept an arbitrary number of arguments by default, it is a good practice to set the [nargs](#1-nargs---sets-the-allowed-number-of-values-to-be-parsed-for-an-argument-this-can-be-set-as-a) parameter for optional arguments (where it makes sense).

<br />

#### 4. Unknown Argument Flag Handling

A command-line argument beginning with a flag prefix (`--` or `-`) that doesn't match any of the specified optional arguments or a compound of optional arguments (only for short flags) is considered **unknown** or **unrecognized**.

By default an argument parser will throw an exception if an unkown argument flag is encountered.

This behavior can be modified using the `unknown_arguments_policy` method of the `argument_parser` class, which sets the policy for handling unknown argument flags.


**Example:**

```cpp
#include <ap/argument_parser.hpp>

int main(int argc, char* argv[]) {
    ap::argument_parser parser("unknown-policy-test");

    parser.program_description("A simple test program for unknwon argument handling policies")
          .default_arguments(ap::default_argument::o_help)
          // set the unknown argument flags handling policy
          .unknown_arguments_policy(ap::unknown_policy::<policy>);

    parser.add_optional_argument("known", "k")
          .help("A known optional argument");

    parser.try_parse_args(argc, argv);

    std::cout << "known = " << ap::util::join(parser.values("known")) << std::endl;

    return 0;
}
```

The available policies are:
- `ap::unknown_policy::fail` (default) - throws an exception if an unknown argument flag is encountered:

    ```txt
    > ./unknown-policy-test --known --unknown
    [ap::error] Unknown argument [--unknown].
    Program: unknown-policy-test

      A simple test program for unknwon argument handling policies

    Optional arguments:

      --help, -h  : Display the help message
      --known, -k : A known optional argument
    ```

- `ap::unknown_policy::warn` - prints a warning message to the standard error stream and continues parsing the remaining arguments:

    ```txt
    > ./unknown-policy-test --known --unknown
    [ap::warning] Unknown argument '--unknown' will be ignored.
    known =
    ```

- `ap::unknown_policy::ignore` - ignores unknown argument flags and continues parsing the remaining arguments:

    ```txt
    ./unknown-policy-test --known --unknown
    known =
    ```

- `ap::unknown_policy::as_values` - treats unknown argument flags as values:

    ```txt
    > ./unknown-policy-test --known --unknown
    known = --unknown
    ```

> [!IMPORTANT]
>
> - The unkown argument flags handling polciy only affects the parser's behaviour when calling the `parse_args` or `try_parse_args` methods.
> - When parsing known args with `parse_known_args` or `try_parse_known_args` all unknown arguments (flags and values) are collected and returned as the parsing result, ignoring the specified policy for handling unknown arguments.
>
> Consider a similar example as above with only the argument parsing function changed:
> ```cpp
> const auto unknown_args = parser.try_parse_known_args(argc, argv);
> std::cout << "known = " << ap::util::join(parser.values("known")) << std::endl
>           << "unknown = " << ap::util::join(unknown_args) << std::endl;
> ```
> This would produce the following output regardless of the specified unknown arguments policy.
> ```shell
> > ./test --known --unknown
> known =
> unknown = --unknown
> ```

<br />
<br />

### Compound Arguments

Compound argument flags are **secondary** argument flags of which **every** character matches the secondary name of an optional argument.

Example:

```cpp
parser.add_optional_argument("verbose", "v")
      .nargs(0)
      .help("Increase verbosity level");

parser.add_flag("option", "o")
      .help("Enable an option flag");

parser.add_optional_argument<int>("numbers", "n")
      .help("Provide integer values");

parser.try_parse_args(argc, argv);

std::cout << "Verbosity level: " << parser.count("verbose")
          << "\nOption used: " << std::boolalpha << parser.value<bool>("use-option")
          << "\nNumbers: " << ap::util::join(parser.values<int>("numbers"), ", ")
          << std::endl;

/*
> ./program -vvon 1 2 3
Verbosity level: 2
Option used: true
Numbers: 1, 2, 3
```

> [!IMPORTANT]
>
> - If there exists an argument whose secondary name matches a possible compound of other arguments, the parser will still treat the flag as a flag of the **single matching argument**, not as multiple flags.
> - The argument parser will try to assign the values following a compound argument flag to the argument represented by the **last character** of the compound flag.

<br />

### Parsing Known Arguments

If you wish to handle only the specified command-line arguments and leave all unkown/unrecognized arguments, you can use the `parse_known_args` method.

This method behaves similarly to `parse_args()` (see [Parsing Arguments](#parsing-arguments)), however it does not throw an error if unknown arguments are detected. Instead it returnes all the unknown arguments detected during parsing as a `std::vector<std::string>`.

Consider a simple example:

```cpp
parser.add_optional_argument("recognized", "r")
      .nargs(ap::nargs::up_to(2))
      .help("A recognized optional argument");

parser.parse_args(argc, argv);

std::cout << "recognized = " << ap::util::join(parser.values("recognized")) << std::endl;

/* Example executions:
> ./program --recognized value1 value2
recognized = value1, value2

> ./program --recognized value1 value2 value3
terminate called after throwing an instance of 'ap::parsing_failure'
  what():  Failed to deduce the argument for values [value3]
Aborted (core dumped)

> ./program value0 --recognized value1 value2
terminate called after throwing an instance of 'ap::parsing_failure'
  what():  Failed to deduce the argument for values [value0]
Aborted (core dumped)

> ./program --recognized value1 value2 --unrecognized value
terminate called after throwing an instance of 'ap::parsing_failure'
  what():  Unknown argument [--unrecognized].
Aborted (core dumped)
>
```

Here the parser throws exceptions for arguments it doesn't recognize. Now consider the same example with `parse_known_args`:

```cpp
parser.add_optional_argument("recognized", "r")
      .nargs(ap::nargs::up_to(2))
      .help("A recognized optional argument");

const auto unknown_args = parser.parse_known_args(argc, argv);

std::cout << "recognized = " << ap::util::join(parser.values("recognized")) << std::endl
          << "unkown = " << ap::util::join(unknown_args) << std::endl;

/* Example execution:
> ./program value0 --recognized value1 value2 value3 --unrecognized value
recognized = value1, value2
unkown = value0, value3, --unrecognized, value
```

Now all the values, that caused an exception for the `parse_args` example, are collected and returned as the result of argument parsing.

> [!IMPORTANT]
>
> If a parser encounters an unrecognized argument flag during *known* args parsing, then the flag will be collected and the currently processed optional argument will be reset. That means that any value following an unrecognized flag will be used to parse positional arguments or treated as an unknown argument as well (if there are no unparsed positional arguments). Let's consider an example:
>
> ```cpp
> parser.add_positional_argument("positional")
>       .help("A positinal argument");
> parser.add_optional_argument("recognized", "r")
>       .nargs(ap::nargs::any())
>       .help("A recognized optional argument");
>
> const auto unknown_args = parser.parse_known_args(argc, argv);
>
> std::cout << "positional = " << parser.value("positional") << std::endl
>           << "recognized = " << ap::util::join(parser.values("recognized")) << std::endl
>           << "unkown = " << ap::util::join(unknown_args) << std::endl;
>
> /* Example execution:
> > ./program --recognized value1 value2 value3 --unrecognized value4 value5 --recognized value6
> positional = value4
> recognized = value1, value2, value3, value6
> unkown = --unrecognized, value5
>
> > ./program value0 --recognized value1 value2 value3 --unrecognized value4 --recognized value5
> positional = value0
> recognized = value1, value2, value3, value5
> unkown = --unrecognized, value4
> ```
>
> Here `value` is treated either as the `positional` argument's value or as an unknown argument (depending on the input arguments) even though the `recognized` optional argument still accepts values and only after the `--recognized` argument flag is encountered the parser continues collecting values for this argument.

> [!TIP]
>
> Similarly to the `parse_args` method, `parse_known_args` has a `try` equivalent - `try_parse_known_args` - which will automatically catch these exceptions, print the error message, and exit with a failure status.

<br/>
<br/>
<br/>

## Retrieving Argument Values

You can retrieve the argument's value(s) with:

```cpp
(const) value_type value = parser.value<value_type>("argument_name"); // (1)
(const) value_type value = parser.value_or<value_type>("argument_name", fallback_value); // (2)
(const) std::vector<value_type> values = parser.values<value_type>("argument_name"); // (3)
```

1. Returns the given argument's value.

    - Returns the argument's parsed value if it has one.
    - If more than one value has been parsed for the argument, this function will return the first parsed value.
    - Returns the argument's predefined value if no value has been parsed for the argument.

2. Returns the given argument's value or the specified fallback value if the argument has no values.

    - If the argument has a value (parsed or predefind), the behavior is the same as in case **(1)**.
    - If the argument has no values, this will return `value_type{std::forward<U>(fallback_value)}` (where `U` is the deduced type of `fallback_value`).

3. Returns a vector of the given argument's values.

    - If the argument has any values (parsed or predefined), they will be returned as a `std::vector<value_type>`.
    - If th argument has no values an empty vector will be returned.

> [!NOTE]
>
> The argument value getter functions might throw an exception if:
> - An argument with the given name does not exist
> - The argument does not contain any values - parsed or predefined (only getter function `(1)`)
> - The specified `value_type` does not match the value type of the argument

<br/>
<br/>
<br/>

<br/>
<br/>
<br/>

## Subparsers

Subparsers allow you to build **hierarchical command-line interfaces**, where a top-level parser delegates parsing to its subcommands. This is particularly useful for creating CLI applications like `git`, where commands such as `git add`, `git commit`, and `git push` each have their own arguments.

### Creating Subparsers

```cpp
auto& subparser = parser.add_subparser("subprogram");
```

Each subparser is a separate instance of `ap::argument_parser` and therefore it can have it can have its own parameters, including a description, arguments, argument groups, subparsers, etc.

For example:

```cpp
// top-level parser
ap::argument_parser git("ap-git");
git.program_version({.major = 2u, .minor = 43u, .patch = 0u})
   .program_description("A version control system built with CPP-AP")
   .default_arguments(ap::default_argument::o_help, ap::default_argument::o_version);

// subcommand: status
auto& status = git.add_subparser("status");
status.default_arguments(ap::default_argument::o_help)
      .program_description("Show the working tree status");
status.add_flag("short", "s")
      .help("Give the output in the short-format");
```

This defines `git` and `git status` parsers, each with their own sets of arguments.

### Using Multiple Subparsers

You can add as many subparsers as you like, each corresponding to a different command:

```cpp
auto& init = git.add_subparser("init");
init.program_description("Create an empty Git repository or reinitialize an existing one");

auto& add = git.add_subparser("add");
add.program_description("Add file contents to the index");

auto& commit = git.add_subparser("commit");
commit.program_description("Record changes to the repository");

auto& status = git.add_subparser("status");
status.program_description("Show the working tree status");

auto& push = git.add_subparser("push");
push.program_description("Update remote refs along with associated objects");
```

All defined subparsers will be included in the parent parser's help message:

```txt
> ap-git --help
Program: ap-git (v2.43.0)

  A version control system built with CPP-AP

Commands:

  init   : Create an empty Git repository or reinitialize an existing one
  add    : Add file contents to the index
  commit : Record changes to the repository
  status : Show the working tree status
  push   : Update remote refs along with associated objects

Optional Arguments:

  --help, -h    : Display the help message
  --version, -v : Dsiplay program version info
```

### Parsing Arguments with Subparsers

When parsing command-line arguments, the parent parser will attempt to match the **first command-line token** against the name of one of its subparsers.

- If a match is found, the parser delegates the remaining arguments to the matched subparser.
- This process repeats **recursively**, so each subparser may also match one of its own subparsers.
- Parsing stops when no subparser matches the first token of the *current* argument list. At that point, the parser processes its own arguments.

For example:

```cpp
ap::argument_parser git("ap-git");
auto& submodule = git.add_subparser("submodule");
auto& submodule_init = submodule.add_subparser("init");
```

Running `ap-git submodule init <args>` will result in `<args>` being parsed by the `submodule_init` parser.

### Tracking Parser State

Each parser tracks its state during parsing. The methods described below let you inspect this state:

- `invoked() -> bool` : Returns `true` if the parser’s name appeared on the command line.

  A parser is *invoked* as soon as the parser is selected during parsing, even if parsing is later delegated to one of its subparsers.

- `finalized() -> bool` : Returns `true` if the parser has processed its own arguments.

  This is distinct from `invoked()`: a parser can be invoked but not finalized if one of its subparsers handled the arguments instead.

- `resolved_parser() -> ap::argument_parser&` : Returns a reference to the *deepest invoked parser*.

  sIf no subparser was invoked, this simply returns the current parser.

<br />

#### Example: Inspecting Parsing States

```cpp
// define the parser hierarchy
ap::argument_parser git("ap-git");
auto& submodule = git.add_subparser("submodule");
auto& submodule_init = submodule.add_subparser("init");

// parse arguments
git.try_parse_args(argc, argv);

// print state for each parser
std::cout << std::boolalpha;

std::cout << "git            : invoked=" << git.invoked()
          << ", finalized=" << git.finalized() << '\n';

std::cout << "submodule      : invoked=" << submodule.invoked()
          << ", finalized=" << submodule.finalized() << '\n';

std::cout << "submodule_init : invoked=" << submodule_init.invoked()
          << ", finalized=" << submodule_init.finalized() << '\n';

auto& active = git.resolved_parser();
std::cout << "\nResolved parser : " << active.name() << " (" << active.program_name() << ")\n";
```

If you run: `./ap-git submodule intit`, you will get the following state:

```txt
git            : invoked=true, finalized=false
submodule      : invoked=true, finalized=false
submodule_init : invoked=true, finalized=true

Resolved parser : init (ap-git submodule init)
```

## Examples

The library usage examples / demo projects can be found in the [cpp-ap-demo](https://github.com/SpectraL519/cpp-ap-demo) repository.

<br/>
<br/>
<br/>

## Common Utility

The CPP-AP library provides some additional utility, the descriptions of which can be found on the [Utility topic page](https://spectral519.github.io/cpp-ap/latest/group__util.html).
