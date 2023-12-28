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

    * With GNU Make

        ```
        > cd <project-root>/test
        > make all
        ```
        ```
        > ./test
        ```
        ```
        > ./test -ts="<test-suite-name>"
        ```
    
    * With CMake
        ```
        > cd <project-root>/test/cmake
        > cmake ..
        > make
        ```
        ```
        > ./test/out/test
        ```
        ```
        > ./test/out/test -ts="<test-suite-name>"
        ```

3. Error fixing:

    * Makefile error
        In case you face an error in Windows saying that:
        ```
        ...\profile.ps1 cannot be loaded because running scripts is disable on this system. ...
        ```
        Then you should run the following command to enable running scripts by Windows' Makefile:
        ```
        > PowerShell -ExecutionPolicy Bypass
        ```
    * Wrong CMake generator
        In case you generate VS Studio files by using CMake instead of Makefile then you should change used generator in CMake by using the following command:
        ```
        > cmake -G "Unix Makefiles" ..
        ```
        Instead of this command:
        ```
        > cmake ..
        ```
        In case this fix does not work then you will probably have to search for another generator matching your system's requirements. You can do that by listing all available generators in "Generators" section when running the following command:
        ```
        > cmake --help
        ```
        Just try to find a right generator for your system and run mentioned command but subtituting a chosen generator into this command:
        ```
        > cmake -G "<generator>" ..
        ```

4. Tips and tricks:

    * CMake compiler swap
        In case you would like to swap used compiler in CMake to the same compiler with other version or a different compiler then you should run cmake command by using -D flag in the following way:
        ```
        > cmake -DCMAKE_CXX_COMPILER=<compiler> ..
        ```
        The default is:
        ```
        > cmake -DCMAKE_CXX_COMPILER=g++-20 ..
        ```

    * GNU Make compiler swap
        In case you would like to swap used compiler in GNU Make to clang++ then you should run make with following parameters:
        ```
        > make <your_target> CXX=clang++
        ```
        Default compiler is g++-20.

    Remember that this project does not work with older versions of g++ than g++-11.


## Documentation

The documentation for this project is generated using Doxygen. Follow the steps below to generate and view the documentation.

### Prerequisites

1. **Doxygen Installation**: Make sure you have Doxygen installed on your system. If not, you can download it from [here](https://www.doxygen.nl/download.html).

### Generating Documentation

1. Open a terminal in the root directory of the project.

2. Run the following command to generate the documentation:

   ```bash
   doxygen Doxyfile
   ```
