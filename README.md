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
    - [Downloading the Library](/docs/tutorial.md#downloading-the-library)
  - [The Parser Class](/docs/tutorial.md#the-parser-class)
  - [Adding Arguments](/docs/tutorial.md#adding-arguments)
  - [Argument Parameters](/docs/tutorial.md#argument-parameters)
  - [Default Arguments](/docs/tutorial.md#default-arguments)
  - [Parsing Arguments](/docs/tutorial.md#parsing-arguments)
  - [Retrieving Argument Values](/docs/tutorial.md#retrieving-argument-values)
  - [Examples](/docs/tutorial.md#examples)
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
