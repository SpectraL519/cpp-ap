import argparse
import re
import sys
from pathlib import Path


def get_cmake_version(cmake_path: Path):
    text = cmake_path.read_text()
    match = re.search(r'project\s*\([^\)]*VERSION\s+(\d+\.\d+\.\d+)', text, re.IGNORECASE)
    if match:
        return match.group(1)
    raise ValueError(f"Could not find project version in {cmake_path}")


def get_doxy_version(doxyfile_path: Path):
    text = doxyfile_path.read_text()
    match = re.search(r'^\s*PROJECT_NUMBER\s*=\s*("?)([\d\.]+)\1', text, re.MULTILINE)
    if match:
        return match.group(2)
    raise ValueError(f"Could not find PROJECT_NUMBER in {doxyfile_path}")


def main(cmake: Path, doxygen: Path):
    try:
        cmake_version = get_cmake_version(cmake)
        doxy_version = get_doxy_version(doxygen)
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

    if cmake_version != doxy_version:
        print(f"Version mismatch: CMakeLists.txt = {cmake_version}, Doxyfile = {doxy_version}", file=sys.stderr)
        sys.exit(1)

    print(cmake_version) # print the version to stdout for shell capture


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-c", "--cmake",
        type=Path,
        default="CMakeLists.txt",
        nargs=1,
        help="Path to the root CMake file"
    )
    parser.add_argument(
        "-d", "--doxygen",
        type=Path,
        default="Doxyfile",
        nargs=1,
        help="Path to the doxygen config file"
    )

    main(**vars(parser.parse_args()))
