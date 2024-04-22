# CPP-AP : Change log

### Version 1.0

* Initial version of `argument_parser.hpp` with corresponding tests
* Initial versions of `.clang-format` and `Doxyfile` configuration files
* Formatting scripts:
    * Unix: `format/unix_like.sh`
    * Windows `format/win.ps1`
* Example programs:
    * convert_number
    * merge_files
    * power
    * verbosity
* Github workflows:
    * g++
    * clang++
    * test
* MIT Licence

<br />
<br />

### Version 1.1

* Added `change_log.md`
* Reorganized project folder strucure:
    * Renamed folders `src` to `source`
    * Moved formatting scripts to `scripts/format/`
* Aligned the `.clang-format` configuration file
* Added the `install_clang17_toolchain.sh` env script
* Added the `format` workflow
* Switched to the `std::ranges` and `std::views` algorithms for all current container operations
* Modified the `argument_name` structure - renamed members: `name` to `primary`, `short_name` to `secondary`
* Added `argument_name::match(string_view)` and `argument_name::match(argument_name)` functions
* Added aliases for default argument enum classes:
    * `ap::default_argument::positional` = `ap::default_posarg`
    * `ap::default_argument::optional` = `ap::default_optarg`
* Renamed the predefined: `ap::action::check_file_exists_action` -> `ap::action::check_file_exists`
