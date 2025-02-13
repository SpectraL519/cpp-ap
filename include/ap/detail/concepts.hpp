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
 * @brief Specifies the type validation rule.
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
 */
template <typename T, typename U, type_validator TV>
inline constexpr bool is_valid_type_v = false;

/**
 * @brief Checks if `T` and `U` are the same type.
 * @tparam T The type to check.
 * @tparam U The type to check agains.
 */
template <typename T, typename U>
inline constexpr bool is_valid_type_v<T, U, type_validator::same> = std::same_as<T, U>;

/**
 * @brief Checks if `T` is convertible to `U`.
 * @tparam T The type to check.
 * @tparam U The type to check agains.
 */
template <typename T, typename U>
inline constexpr bool is_valid_type_v<T, U, type_validator::convertible> =
    std::convertible_to<T, U>;

/**
 * @brief Concept that enforces `is_valid_type_v`.
 * @tparam T The type to check.
 * @tparam U The type to check agains.
 * @tparam TV The validation rule (`same` or `convertible`).
 */
template <typename T, typename U, type_validator TV = type_validator::same>
concept c_valid_type = is_valid_type_v<T, U, TV>;

/**
 * @brief Validates that R is a range of type T (ignoring the cvref attributes).
 * @tparam R The range type to check.
 * @tparam V The expected range value type.
 * @tparam TV The validation rule (`same` or `convertible`).
 */
template <typename R, typename V, type_validator TV = type_validator::same>
concept c_range_of =
    std::ranges::range<R>
    and c_valid_type<std::remove_cvref_t<std::ranges::range_value_t<R>>, V, TV>;

/**
 * @brief Validates that R is a sized range of type T (ignoring the cvref attributes).
 * @tparam R The range type to check.
 * @tparam V The expected range value type.
 * @tparam TV The validation rule (`same` or `convertible`).
 */
template <typename R, typename V, type_validator TV = type_validator::same>
concept c_sized_range_of =
    std::ranges::sized_range<R>
    and c_valid_type<std::remove_cvref_t<std::ranges::range_value_t<R>>, V, TV>;


} // namespace ap::detail
