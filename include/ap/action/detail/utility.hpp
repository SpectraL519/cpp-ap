// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ap/action/specifiers.hpp"

namespace ap::action::detail {

/**
 * @brief The concept is satisfied when `AS` is either a valued or void argument action
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_action_specifier = ap::detail::c_one_of<AS, action_type::transform, action_type::modify>;

/// @brief Template argument action callable type alias.
template <c_action_specifier AS, ap::detail::c_argument_value_type T>
using callable_type = typename AS::template type<T>;

/// @brief Template argument action callabla variant type alias.
template <ap::detail::c_argument_value_type T>
using action_variant_type =
    std::variant<callable_type<action_type::transform, T>, callable_type<action_type::modify, T>>;

/**
 * @brief Checks if an argument action variant holds a void action.
 * @tparam T The argument value type.
 * @param action The action variant.
 * @return True if the held action is a void action.
 */
template <ap::detail::c_argument_value_type T>
[[nodiscard]] constexpr inline bool is_modify_action(const action_variant_type<T>& action
) noexcept {
    return std::holds_alternative<callable_type<action_type::modify, T>>(action);
}

} // namespace ap::action::detail
