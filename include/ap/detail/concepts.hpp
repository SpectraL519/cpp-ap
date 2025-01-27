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

} // namespace ap::detail
