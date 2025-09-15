// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file ap/version.hpp

#pragma once

#include <format>
#include <iostream>

namespace ap {

/// @brief A helper structure used to represent a program's version.
struct version {
    std::uint32_t major = 1u; ///< The major version number.
    std::uint32_t minor = 0u; ///< The minor version number.
    std::uint32_t patch = 0u; ///< The patch number.

    /// @brief Converts the structure into a string in the `v{major}.{minor}.{path}` format
    [[nodiscard]] std::string str() const noexcept {
        return std::format("v{}.{}.{}", this->major, this->minor, this->patch);
    }

    /// @brief The stream insertion operator.
    friend std::ostream& operator<<(std::ostream& os, const version& v) {
        os << v.str();
        return os;
    }
};

} // namespace ap
