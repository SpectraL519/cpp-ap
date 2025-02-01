import argparse
import sys

from collections.abc import Iterable
from enum import IntEnum
from pathlib import Path

from common import find_files


LICENCE_INFO = [
    "// Copyright (c) 2024-2025 Jakub Musiał",
    "// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).",
    "// Licensed under the MIT License. See the LICENSE file in the project root for full license information.",
]

class DefaultParameters:
    search_paths: list[str] = ["include"]
    file_patterns: list[str] = ["*.cpp", "*.hpp", "*.c", "*.h"]
    exclude_paths: list[str] = []
    help: bool = False


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-p", "--search-paths",
        type=str,
        default=DefaultParameters.search_paths,
        nargs="*",
        action="extend",
        help="list of search directory paths"
    )
    parser.add_argument(
        "-f", "--file-patterns",
        type=str,
        default=DefaultParameters.file_patterns,
        nargs="*",
        action="extend",
        help="list of file patterns to include"
    )
    parser.add_argument(
        "-e", "--exclude-paths",
        type=str,
        default=DefaultParameters.exclude_paths,
        nargs="*",
        action="extend",
        help="list of directory paths to exclude"
    )

    return vars(parser.parse_args())


class ReturnCode(IntEnum):
    ok = 0
    file_to_short = -1
    missing_licence = -2
    invalid_licence = -3


def check_licence(expected_licence: Iterable[str], files: set[Path]) -> int:
    n_files = len(files)
    print(f"Files to check: {n_files}")

    return_code = None
    def _set_return_code(c: ReturnCode):
        nonlocal return_code
        return_code = c if not return_code else return_code

    n_licence_lines = len(expected_licence)

    def _check_file(file: Path):
        with open(file, "r", encoding="utf-8") as f:
            lines = [line.strip() for line in f.readlines()]
            n_potential_licence_lines = min(len(lines), n_licence_lines)

            if n_potential_licence_lines < n_licence_lines:
                _set_return_code(ReturnCode.file_to_short)
                print(f"[error] File `{file}` to short")
                return

            matching_lines = [lines[i] == expected_licence[i] for i in range(n_licence_lines)]
            correct_licence = all(matching_lines)
            if not correct_licence:
                missing_info = any(matching_lines)
                if missing_info:
                    _set_return_code(ReturnCode.invalid_licence)
                    print(f"[error] Incomplete license info in file `{file}`")
                else:
                    _set_return_code(ReturnCode.missing_licence)
                    print(f"[error] Missing license info in file `{file}`")


    for i, file in enumerate(files):
        print(f"[{i + 1}/{n_files}] {file}")
        _check_file(file)

    print("Done!")

    return return_code


def main(
    search_paths: Iterable[str],
    file_patterns: Iterable[str],
    exclude_paths: Iterable[str]
):
    files_to_check = find_files(search_paths, file_patterns, exclude_paths)
    sys.exit(check_licence(LICENCE_INFO, files_to_check))


if __name__ == "__main__":
    main(**parse_args())
