// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/util/ranges.hpp
 * @brief Provides common ranges utility functions.
 */

#pragma once

#include "ap/util/concepts.hpp"

#include <any>
#include <ranges>

namespace ap::util {

/**
 * @brief Casts a range of `std::any` to a range of type `T`.
 * @tparam T The target type.
 * @param range The input range of `std::any`.
 * @return A view representing the casted range of type `T`.
 * @throws std::bad_any_cast if any element in the input range cannot be cast to type `T`.
 * @ingroup util
 */
template <typename T>
auto any_range_cast_view(const c_range_of<std::any> auto& range) {
    return range | std::views::transform([](const std::any& value) -> T {
               return std::any_cast<T>(value);
           });
}

} // namespace ap::util
