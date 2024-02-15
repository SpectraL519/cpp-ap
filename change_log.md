# CPP-AP : Change log

### Version 1.0

* Initial version of `argument_parser.hpp` with corresponding tests
* Initial versions of `.clang-format` and `Doxyfile` configuration files
* Formatting scripts:
    * Unix: `format/unix_like.sh`
    * Windows `format/win.ps1`
* Examples:
    * convert_number
    * merge_files
    * power
    * verbosity

<br />

### Version 1.1

* Added `change_log.md`
* Aligned the `.clang-format` configuration file and moved formatting scripts to a new directory `<project-root>/scripts`
* Added the `install_clang_format_17.sh` script
* Added the `format` workflow

TODO:
* Split dev notes into sections and include them in table of contents
* Explicit formatting instructions in dev notes
* Add format checking workflow
* Explicit clang compiler flags setting in CMake
* Add `CMakeLists.txt` in project root
* Add CMake integration instructions in dev notes
