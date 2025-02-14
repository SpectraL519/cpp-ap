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
>
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

- [Key Changes in v2](/docs/changelog_v2.md)
- [Tutorial](/docs/tutorial.md)
- [Dev notes](/docs/dev_notes.md)
- [Documentation](#documentation)
- [Compiler support](#compiler-compatibilty)
- [License](#license)

<br />
<br />

## Documentation

The documentation for this project can be generated using Doxygen:

1. Make sure you have `Doxygen` installed on your machine. If not, you can download it from [here](https://www.doxygen.nl/download.html).

2. Generate the documentation:

  Open your terminal and use the following instructions:

  ```shell
  cd <project-root>
  doxygen Doxyfile
  ```

> [!NOTE]
>
> In the Doxygen output markdown links to other files might not be rendered properly, however the referenced pages will still be generated in the *Related Pages* section.

<br />
<br />

## Compiler Compatibilty

| Compiler | Min Version |
| :-: | :-: |
| GNU G++ | 13 |
| Clang | 17 |

> [!NOTE]
>
> Although currently the project has been properly verified using only the G++ and Clang compilers it should work fine with other compilers with C++20 support like MSVC.

<br />
<br />

## License

The `CPP-AP` project uses the [MIT License](https://mit-license.org/) which can be found in the [LICENSE.md](/LICENSE.md) file
