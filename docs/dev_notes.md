# Dev Notes

<br />

## Building and Testing

> [!NOTE]
>
> The project uses [doctest](https://github.com/doctest/doctest) framework for unit testing, however it is already installed in the [tests/external](/tests/external/) directory, so there is no need to install it sepparately.

### Build the Testing Executable

```shell
cmake -B build -DBUILD_TESTS=ON
cmake --build build/ # -j<njobs>
```

This will build a `run_uts` executable executable in the `build/tests/` directory.

### Runing the Tests

To run the project's unit tests, simply run the testing executable:

```shell
./build/tests/run_uts
```

<!-- TODO: add test suites and use full include paths in implementation -->

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

> [!NOTE]
>
> You can view the online documentation for the latest version [here](https://spectral519.github.io/cpp-ap/latest/).
>
> To view the documentation for the previous versions use the following URL pattern:
> ```
> https://spectral519.github.io/cpp-ap/<version>
> ```

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
> Markdown links to other files or sections and the GFM-style callouts might not be rendered properly in the Doxygen output. To fix these issues you can run the postprocessing script:
> ```
> python3 scripts/postprocess_doxyhtml.py ./documentation
> ```
> or you can view those documents by using the *Related Pages* section on the navigation bar.
