<p align="center">
  <a href="https://github.com/SpectraL519/cpp-argon">
    <img src="docs/img/cpp-argon-hex.png" alt="CPP-ARGON" width="360" />
  </a>
</p>

<br />

<div align="center">

[![GCC](https://github.com/SpectraL519/cpp-argon/actions/workflows/gcc.yaml/badge.svg)](https://github.com/SpectraL519/cpp-argon/actions/workflows/gcc)
[![Clang](https://github.com/SpectraL519/cpp-argon/actions/workflows/clang.yaml/badge.svg)](https://github.com/SpectraL519/cpp-argon/actions/workflows/clang)
[![MSVC](https://github.com/SpectraL519/cpp-argon/actions/workflows/msvc.yaml/badge.svg)](https://github.com/SpectraL519/cpp-argon/actions/workflows/msvc)

</div>

<div align="center">

[![format](https://github.com/SpectraL519/cpp-argon/actions/workflows/format.yaml/badge.svg)](https://github.com/SpectraL519/cpp-argon/actions/workflows/format)
[![demo](https://github.com/SpectraL519/cpp-argon/actions/workflows/demo.yaml/badge.svg)](https://github.com/SpectraL519/cpp-argon/actions/workflows/demo)
[![documentation](https://github.com/SpectraL519/cpp-argon/actions/workflows/documentation.yaml/badge.svg)](https://github.com/SpectraL519/cpp-argon/actions/workflows/documentation)
[![changelog](https://img.shields.io/badge/changelog-blue.svg?logo=github)](https://github.com/SpectraL519/cpp-argon/releases)

</div>

<br />

## Overview

`CPP-ARGON` is a lightweight and feature-rich command-line argument parsing library for the C++20 and newer standards, designed as an alternative to `boost::program_options`. It offers a modern and intuitive interface allowing for straightforward argument configuration and parsing.

> [!CAUTION]
> Versions of the `CPP-ARGON` library up to prior to `v4.0.0` were developed under the name `CPP-AP`.
>
> **Legacy Support:** The deprecated URLs (`https://github.com/SpectraL519/cpp-ap` and `.../cpp-ap-demo`) automatically redirect to the new locations, and build targets defined in previous versions remain fully functional.
>
> **Action Required:**
> - **Repositories:** We highly recommend updating your remote references and submodules to the new canonical URLs:
>   - Library: https://github.com/SpectraL519/cpp-argon
>   - Demo: https://github.com/SpectraL519/cpp-argon-demo
> - **Documentation:** The old documentation URL (`.../cpp-ap/<version>/`) is **no longer active**. Please use the new address:
>   - https://spectral519.github.io/cpp-argon/latest/

> [!NOTE]
>
> [v1.0](https://github.com/SpectraL519/cpp-argon/releases/tag/v1.0) of the library has been developed for the *Team Programming* course at the *Wrocław University of Science and Technology*.
>
> Faculty: *W04N - Faculty of Information and Communication Technology*
>
> Field of study: *Algorithmic Computer Science*
>
> The project has received the 1st place at the 2024 CreatiWITy competition organized by the faculty. The article in Polish can be found on the [faculty website](https://wit.pwr.edu.pl/aktualnosci/oto-laureaci-konkursu-creatiwity-273.html). Please note that this is not a technical article :)

<br />

## Related Pages

- [Tutorial](/docs/tutorial.md#tutorial)
  - [Setting Up CPP-ARGON](/docs/tutorial.md#setting-up-cpp-argon)
    - [CMake Integration](/docs/tutorial.md#cmake-integration)
    - [Bazel Build System](/docs/tutorial.md#bazel-build-system)
    - [Downloading the Library](/docs/tutorial.md#downloading-the-library)
  - [The Parser Class](/docs/tutorial.md#the-parser-class)
  - [Adding Arguments](/docs/tutorial.md#adding-arguments)
    - [Syntax](/docs/tutorial.md#syntax)
    - [Names](/docs/tutorial.md#names)
    - [Value Types](/docs/tutorial.md#value-types)
    - [Boolean Flags](/docs/tutorial.md#boolean-flags)
  - [Argument Parameters](/docs/tutorial.md#argument-parameters)
    - [Common Parameters](/docs/tutorial.md#common-parameters)
    - [Parameters Specific for Optional Arguments](/docs/tutorial.md#parameters-specific-for-optional-arguments)
  - [Default Arguments](/docs/tutorial.md#default-arguments)
  - [Argument Groups](/docs/tutorial.md#argument-groups)
    - [Creating New Groups](/docs/tutorial.md#creating-new-groups)
    - [Adding Arguments to Groups](/docs/tutorial.md#adding-arguments-to-groups)
    - [Group Attributes](/docs/tutorial.md#group-attributes)
    - [Complete Example](/docs/tutorial.md#complete-example)
    - [Suppressing Argument Group Checks](/docs/tutorial.md#suppressing-argument-group-checks)
  - [Parsing Arguments](/docs/tutorial.md#parsing-arguments)
    - [Basic Argument Parsing Rules](/docs/tutorial.md#basic-argument-parsing-rules)
    - [Compound Arguments](/docs/tutorial.md#compound-arguments)
    - [Parsing Known Arguments](/docs/tutorial.md#parsing-known-arguments)
  - [Retrieving Argument Values](/docs/tutorial.md#retrieving-argument-values)
  - [Subparsers](/docs/tutorial.md#subparsers)
    - [Creating Subparsers](/docs/tutorial.md#creating-subparsers)
    - [Using Multiple Subparsers](/docs/tutorial.md#using-multiple-subparsers)
    - [Parsing Arguments with Subparsers](/docs/tutorial.md#parsing-arguments-with-subparsers)
    - [Tracking Parser State](/docs/tutorial.md#tracking-parser-state)
    - [Suppressing Argument Group Checks](/docs/tutorial.md#suppressing-argument-group-checks)
  - [Examples](/docs/tutorial.md#examples)
  - [Common Utility](/docs/tutorial.md#common-utility)
- [Dev notes](/docs/dev_notes.md#dev-notes)
  - [Building and testing](/docs/dev_notes.md#building-and-testing)
  - [Formatting](/docs/dev_notes.md#formatting)
  - [Documentation](/docs/dev_notes.md#documentation)

<br />

## Compiler Compatibilty

| Compiler | Min Version |
| :-: | :-: |
| GNU G++ | 13 |
| Clang | 17 |
| MSVC | 19.44 (Visual Studio 2022) |

<br />

## License

The `CPP-ARGON` project uses the [MIT License](https://mit-license.org/) which can be found in the [LICENSE](/LICENSE.md#mit-license) file
