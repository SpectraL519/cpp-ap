# Key Changes in v2

<br />

## v2.0.0

### The Structure

The *v2* version of the library no longer uses a single-header approach, however it is still a header-only library. With this change it is now required to download the entire library - either with [CMake](/README.md#cmake-integration) or from the [releases page](https://github.com/SpectraL519/cpp-ap/releases).

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

A more detailed explanation of these functions is available in the [Tutorial](/README.md#tutorial) section.

<br />
<br />

## v2.0.1

- `Doxyfile` cleanup
- Extracted the `tutorial` and `dev notes` sections from readme to separate files
- Added the `documentation` workflow
- Aligned the current workflows to be triggered on PRs to master
