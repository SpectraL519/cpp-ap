<h1>
  CPP-AP
  <a href="https://github.com/SpectraL519/cpp-ap" target="_blank">
    <i class="fa fa-github" style="font-size: 1.3em; margin-left: 6px; position: relative; top: -0.08em;"></i>
  </a>
</h1>

Command-line argument parser for C++20

<br />

<div align="center">

[![GCC](https://github.com/SpectraL519/cpp-ap/actions/workflows/gcc.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/gcc)
[![Clang](https://github.com/SpectraL519/cpp-ap/actions/workflows/clang.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/clang)
[![MSVC](https://github.com/SpectraL519/cpp-ap/actions/workflows/msvc.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/msvc)

</div>

<div align="center">

[![format](https://github.com/SpectraL519/cpp-ap/actions/workflows/format.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/format)
[![demo](https://github.com/SpectraL519/cpp-ap/actions/workflows/demo.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/demo)
[![documentation](https://github.com/SpectraL519/cpp-ap/actions/workflows/documentation.yaml/badge.svg)](https://github.com/SpectraL519/cpp-ap/actions/workflows/documentation)
[![changelog](https://img.shields.io/badge/changelog-blue.svg?logo=github)](https://github.com/SpectraL519/cpp-ap/releases)

</div>

<br />

## Overview

`CPP-AP` is a lightweight and feature-rich command-line argument parsing library for the C++20 and newer standards, designed as an alternative to `boost::program_options`. It offers a modern and intuitive interface allowing for straightforward argument configuration and parsing.

> [!NOTE]
>
> [v1.0](https://github.com/SpectraL519/cpp-ap/commit/9a9e5360766b732f322ae2efe3cf5ec5f9268eef) of the library has been developed for the *Team Programming* course at the *Wrocław University of Science and Technology*.
>
> Faculty: *W04N - Faculty of Information and Communication Technology*
>
> Field of study: *Algorithmic Computer Science*
>
> The project has received the 1st place at the 2024 CreatiWITy competition organized by the faculty. The article in Polish can be found on the [faculty website](https://wit.pwr.edu.pl/aktualnosci/oto-laureaci-konkursu-creatiwity-273.html). Please note that this is not a technical article :)

<br />

## Related Pages

- [Tutorial](/docs/tutorial.md#tutorial)
  - [Setting Up CPP-AP](/docs/tutorial.md#setting-up-cpp-ap)
    - [CMake Integration](/docs/tutorial.md#cmake-integration)
    - [Bazel Build System](/docs/tutorial.md#bazel-build-system)
    - [Downloading the Library](/docs/tutorial.md#downloading-the-library)
  - [The Parser Class](/docs/tutorial.md#the-parser-class)
  - [Adding Arguments](/docs/tutorial.md#adding-arguments)
  - [Argument Parameters](/docs/tutorial.md#argument-parameters)
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
  - [Default Arguments](/docs/tutorial.md#default-arguments)
  - [Parsing Arguments](/docs/tutorial.md#parsing-arguments)
    - [Basic Argument Parsing Rules](/docs/tutorial.md#basic-argument-parsing-rules)
    - [Compound Arguments](/docs/tutorial.md#compound-arguments)
    - [Parsing Known Arguments](/docs/tutorial.md#parsing-known-arguments)
  - [Retrieving Argument Values](/docs/tutorial.md#retrieving-argument-values)
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

The `CPP-AP` project uses the [MIT License](https://mit-license.org/) which can be found in the [LICENSE](/LICENSE.md#mit-license) file
