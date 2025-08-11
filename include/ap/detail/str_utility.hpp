// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file str_utility.hpp
 * @brief Provides common string utility functions.
 */

#pragma once

#include "concepts.hpp"

#include <algorithm>
#include <sstream>
#include <string_view>

namespace ap::detail {

/**
 * @brief Converts a value to `std::string`.
 * @tparam T The value type (must satisfy the @ref ap::detail::c_writable concept).
 * @param value The value to convert.
 */
template <c_writable T>
[[nodiscard]] std::string as_string(const T& value) noexcept {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

/// @brief Checks whether a string contains any whitespace characters.
[[nodiscard]] inline bool contains_whitespaces(std::string_view str) noexcept {
    return std::ranges::any_of(str, [](unsigned char c) { return std::isspace(c); });
}

/**
 * @brief Joins elements of a range into a single string with a delimiter.
 * @tparam R The type of the value range. The value type of R must satisfy the @ref c_writable concept.
 * @param range The input range to join.
 * @param delimiter The separator string to insert between elements.
 * @return A single string with all elements joined by the delimiter.
 * \todo Replace with std::views::join after transition to C++23.
 */
template <std::ranges::range R>
requires(c_writable<std::ranges::range_value_t<R>>)
[[nodiscard]] std::string join(const R& range, const std::string_view delimiter = ", ") {
    std::ostringstream oss;

    auto it = std::ranges::begin(range);
    const auto end = std::ranges::end(range);
    if (it != end) {
        oss << *it;
        ++it;
        for (; it != end; ++it)
            oss << delimiter << *it;
    }

    return oss.str();
}

} // namespace ap::detail
