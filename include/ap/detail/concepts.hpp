#pragma once

#include <iostream>

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
 * @brief The concept is satisfied when `T` is c_readable and semiregular.
 * @tparam T Type to check.
 */
template <typename T>
concept c_argument_value_type = c_readable<T> and std::semiregular<T>;

/**
 * @brief Holds the boolean value indicating whether type `T` is the same as one of the `ValidTypes`.
 * @tparam T Type to check.
 * @tparam ValidTypes The valid types to compare against.
 */
template <typename T, typename... ValidTypes>
inline constexpr bool is_valid_type_v = std::disjunction_v<std::is_same<T, ValidTypes>...>;

} // namespace ap::detail
