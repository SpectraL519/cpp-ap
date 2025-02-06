// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include <iostream>
#include <ranges>

namespace ap::detail {

/**
 * @brief The concept is satisfied when `T` overloads the std::istream operator `>>`.
 * @tparam T Type to check.
 */
template <typename T>
concept c_readable = requires(T value, std::istream& input_stream) { input_stream >> value; };

/**
 * @brief The concept is satisfied when `T` overloads the std::ostream operator `<<`.
 * @tparam T Type to check.
 */
template <typename T>
concept c_writable = requires(T value, std::ostream& output_stream) { output_stream << value; };

/**
 * @brief The concept is used to verify the validity of the arguments' value types.
 * @tparam T Type to check.
 */
template <typename T>
concept c_argument_value_type = c_readable<T> and std::semiregular<T>;

/**
 * @brief Validates that `T` is the same as one of the types defined by `Types`.
 * @tparam T Type to check.
 * @tparam Types The valid types to compare against.
 */
template <typename T, typename... Types>
concept c_one_of = std::disjunction_v<std::is_same<T, Types>...>;

/**
 * @brief Validates that R is a range of type T (ignoring the cvref attributes).
 * @tparam R The range type to check.
 * @tparam V The expected range value type.
 */
template <typename R, typename V>
concept c_range_of =
    std::ranges::range<R> and std::same_as<V, std::remove_cvref_t<std::ranges::range_value_t<R>>>;

} // namespace ap::detail
