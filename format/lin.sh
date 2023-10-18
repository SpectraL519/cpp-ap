#!/bin/bash

# Recursively find .hpp files and format them with clang-format
find . -type f -name "*.hpp" -o -name "*.cpp" -exec clang-format -i {} \;
