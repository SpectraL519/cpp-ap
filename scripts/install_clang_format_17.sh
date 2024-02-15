#!/bin/bash

if command -v clang-format-17 &>/dev/null; then
    echo "clang-format-17 is already installed."
    exit 0
fi

echo "Adding LLVM apt repository"
wget -q -O - https://apt.llvm.org/llvm-snapshot.gpg.key | sudo apt-key add - >/dev/null 2>&1
echo | sudo add-apt-repository "deb http://apt.llvm.org/$(lsb_release -sc)/ llvm-toolchain-$(lsb_release -sc)-17 main" &>/dev/null 2>&1

echo "Updating package list"
sudo apt update &>/dev/null 2>&1

echo "Installing clang-format-17"
sudo apt install clang-format-17 -y &>/dev/null 2>&1
echo "Success!"
