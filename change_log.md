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
* Reorganized project folder strucure:
    * Renamed folders `src` to `source`
    * Moved formatting scripts to `scripts/format/`
* Aligned the `.clang-format` configuration file and moved formatting scripts to a new directory `<project-root>/scripts`
* Added the `install_clang_format_17.sh` env script
* Added the `format` workflow
