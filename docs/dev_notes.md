# Dev Notes

<br />

## Building and testing

> [!NOTE]
>
> The project uses [doctest](https://github.com/doctest/doctest) framework for unit testing, however it is already installed in the [tests/external](/tests/external/) directory, so there is no need to install it sepparately.

### Build the testing executable

```shell
cmake -B build -DBUILD_TESTS=ON
cd build
make # -j <n>
```

This will build the test executable `run` in the `<project-root>/build/tests` directory.

### Run the tests

```shell
cd build
./tests/run # -ts=<test-suite-name>
```

> [!NOTE]
>
> Test suites in the project have the same names as the files they're in except for the `test_extarnal_libs_config.cpp` file which defines the `test_doctest_config` test suite.

<br />
<br />

## Formatting

> [!NOTE]
>
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
>
> The script requires `python >= 3.9`

<br />
<br />

## Documentation

> [!INFO]
>
> You can view the online documentation for the latest version [here](https://spectral519.github.io/cpp-ap/latest/). To view the documentation for the previous versions use the following URL pattern: `https://spectral519.github.io/cpp-ap/<version>`.
>
> **NOTE:** The online documentation is available only for versions `>= 2.2.5` - for older versions the documentation has to be built locally.

The documentation for this project can be generated using Doxygen, styled with a custom [fork](https://github.com/SpectraL519/doxygen-awesome-css/tree/theme-alignment) of the [doxygen-awesome-css](https://github.com/jothepro/doxygen-awesome-css) theme.

### Prerequisites

1. Ensure that Doxygen is installed on your system. If not, you can download it from the [official website](https://www.doxygen.nl/download.html).

2. The `doxygen-awesome-ccs` theme fork is included as a submodule within the project.

### Generating the Documentation

To generate the documentation, run the following commands in your terminal:

```shell
cd <project-root>
doxygen Doxyfile
```

This should create a `documentation` directory containing the project's documentation in html format.

> [!NOTE]
>
> Markdown links to other files might not be rendered properly in the Doxygen output, however the referenced pages will still be generated in the *Related Pages* section.
