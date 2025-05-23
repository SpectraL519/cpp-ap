// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "concepts.hpp"

#include <sstream>
#include <string_view>

namespace ap::detail {

/**
 * \todo replace with std::views::join_with after transition to C++23
 */
template <std::ranges::range R>
requires(c_writable<std::ranges::range_value_t<R>>)
[[nodiscard]] std::string join_with(const R& range, const std::string_view delimiter = ", ") {
    std::ostringstream oss;

    auto it = std::begin(range);
    const auto end = std::end(range);
    if (it != end) {
        oss << *it;
        ++it;
        for (; it != end; ++it)
            oss << delimiter << *it;
    }

    return oss.str();
}

} // namespace ap::detail
