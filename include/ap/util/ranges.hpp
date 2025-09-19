// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/util/ranges.hpp
 * @brief Provides common ranges utility functions.
 */

#pragma once

#include "concepts.hpp"

#include <any>
#include <ranges>

namespace ap::detail {

template <typename T>
auto any_range_cast_view(const c_range_of<std::any> auto& range) {
    return range | std::views::transform([](const std::any& value) -> T {
               return std::any_cast<T>(value);
           });
}

} // namespace ap::detail
