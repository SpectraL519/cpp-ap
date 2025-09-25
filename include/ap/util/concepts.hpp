// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/util/concepts.hpp
 * @brief Provides the general concept definitions.
 */

#pragma once

#include "ap/types.hpp"

#include <iostream>
#include <ranges>

namespace ap::util {

/**
 * @brief The concept is satisfied when `T` is @ref ap::none_type.
 * @tparam T Type to check.
 * @ingroup util
 */
template <typename T>
concept c_is_none = std::same_as<T, none_type>;

/**
 * @brief The concept is satisfied when `T` overloads the std::istream operator `>>`.
 * @tparam T Type to check.
 * @ingroup util
 */
template <typename T>
concept c_readable = requires(T value, std::istream& input_stream) { input_stream >> value; };

/**
 * @brief The concept is satisfied when `T` can be constructed from `const std::string&`.
 * @tparam T Type to check.
 * @ingroup util
 */
template <typename T>
concept c_trivially_readable = std::constructible_from<T, const std::string&>;

/**
 * @brief The concept is satisfied when `T` overloads the std::ostream operator `<<`.
 * @tparam T Type to check.
 * @ingroup util
 */
template <typename T>
concept c_writable = requires(T value, std::ostream& output_stream) { output_stream << value; };

/**
 * @brief The concept is satisfied when `T` is an arithmetic type.
 * @tparam T Type to check.
 * @ingroup util
 */
template <typename T>
concept c_arithmetic = std::is_arithmetic_v<T>;

/**
 * @brief The concept is used to verify the validity of the arguments' value types.
 * @tparam T Type to check.
 * @ingroup util
 */
template <typename T>
concept c_argument_value_type =
    std::same_as<T, ap::none_type>
    or (std::semiregular<T> and (c_trivially_readable<T> or c_readable<T>));

/**
 * @brief Validates that `T` is the same as one of the types defined by `Types`.
 * @tparam T Type to check.
 * @tparam Types The valid types to compare against.
 * @ingroup util
 */
template <typename T, typename... Types>
concept c_one_of = std::disjunction_v<std::is_same<T, Types>...>;

/**
 * @brief Specifies the type validation rule.
 * @ingroup util
 */
enum class type_validator : bool {
    same, ///< Exact type match.
    convertible ///< Implicit conversion allowed.
};

/**
 * @brief Checks if two types satisfy a given `type_validator` rule.
 *
 * The primary template of the `is_valid_type_v` helper variable,
 * which does not perform any logic and defaults to `false`.
 *
 * @tparam T The type to check.
 * @tparam U The type to check agains.
 * @tparam TV The validation rule.
 * @ingroup util
 */
template <typename T, typename U, type_validator TV>
inline constexpr bool is_valid_type_v = false;

/**
 * @brief Checks if `T` and `U` are the same type.
 * @tparam T The type to check.
 * @tparam U The type to check agains.
 * @ingroup util
 */
template <typename T, typename U>
inline constexpr bool is_valid_type_v<T, U, type_validator::same> = std::same_as<T, U>;

/**
 * @brief Checks if `T` is convertible to `U`.
 * @tparam T The type to check.
 * @tparam U The type to check agains.
 * @ingroup util
 */
template <typename T, typename U>
inline constexpr bool is_valid_type_v<T, U, type_validator::convertible> =
    std::convertible_to<T, U>;

/**
 * @brief Concept that enforces `is_valid_type_v`.
 * @tparam T The type to check.
 * @tparam U The type to check agains.
 * @tparam TV The validation rule (`same` or `convertible`).
 * @ingroup util
 */
template <typename T, typename U, type_validator TV = type_validator::same>
concept c_valid_type = is_valid_type_v<T, U, TV>;

/**
 * @brief Validates that R is a range of type T (ignoring the cvref qualifiers).
 * @tparam R The range type to check.
 * @tparam V The expected range value type.
 * @tparam TV The validation rule (`same` or `convertible`).
 * @ingroup util
 */
template <typename R, typename V, type_validator TV = type_validator::same>
concept c_range_of =
    std::ranges::range<R>
    and c_valid_type<std::remove_cvref_t<std::ranges::range_value_t<R>>, V, TV>;

/**
 * @brief Validates that It is a forward iterator of type T (ignoring the cvref qualifiers).
 * @tparam It The iterator type to check.
 * @tparam V The expected iterator value type.
 * @tparam TV The validation rule (`same` or `convertible`).
 * @ingroup util
 */
template <typename R, typename V, type_validator TV = type_validator::same>
concept c_forward_range_of =
    std::ranges::range<R>
    and c_valid_type<std::remove_cvref_t<std::ranges::range_value_t<R>>, V, TV>;

/**
 * @brief Validates that It is a forward iterator of type T (ignoring the cvref qualifiers).
 * @tparam It The iterator type to check.
 * @tparam V The expected iterator value type.
 * @tparam TV The validation rule (`same` or `convertible`).
 * @ingroup util
 */
template <typename It, typename V, type_validator TV = type_validator::same>
concept c_forward_iterator_of =
    std::input_iterator<It> and c_valid_type<std::iter_value_t<It>, V, TV>;

} // namespace ap::util
