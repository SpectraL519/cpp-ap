# cpp-ap
Argument Parser for C++20

<br />

> CPP-AP is a project developed for the *Team Programming* course at the *Wrocław University of Science and Technology*
>
> Faculty: *W04N - Faculty of Information and Communication Technology*
>
> Field of study: *Algorithmic Computer Science*

<br />
<br />

## DEV NOTES

1. Requirements:
   * g++ >= g++-11
   * clang-format-17 ([ubuntu download tutorial](https://ubuntuhandbook.org/index.php/2023/09/how-to-install-clang-17-or-16-in-ubuntu-22-04-20-04/amp/?fbclid=IwAR1ZfJpoiitjwn8aMlKVWpFdkYmUqtaQwraJBju09v1gtc0jQANTgVeCuMY))

<br />

2. Building and running tests:

    * Building

        ```
        > cd <project-root>/test
        > make all
        ```

    * Running
        ```
        > ./test
        ```
        ```
        > ./test -ts="<test-suite-name>"
        ```

## Documentation

The documentation for this project is generated using Doxygen. Follow the steps below to generate and view the documentation.

### Prerequisites

1. **Doxygen Installation**: Make sure you have Doxygen installed on your system. If not, you can download it from [here](https:## Documentation

The documentation for this project is generated using Doxygen. Follow the steps below to generate and view the documentation.

### Prerequisites

1. **Doxygen Installation**: Make sure you have Doxygen installed on your system. If not, you can download it from [here](https://www.doxygen.nl/download.html).

### Generating Documentation

1. Open a terminal in the root directory of the project.

2. Run the following command to generate the documentation:

   ```bash
   doxygen Doxyfile
   ```
