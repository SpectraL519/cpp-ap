// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/action/detail/utility.hpp
 * @brief Defines general action-related utility.
 */

#pragma once

#include "ap/action/types.hpp"

#include <utility>
#include <variant>

namespace ap::action::detail {

/**
 * @brief The concept is satisfied when `AS` is a valid *value* action action specifier.
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_value_action_specifier =
    ap::detail::c_one_of<AS, action_type::observe, action_type::transform, action_type::modify>;

/**
 * @brief The concept is satisfied when `AS` is a valid *on-flag* action action specifier.
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_flag_action_specifier = ap::detail::c_one_of<AS, action_type::on_flag>;

/**
 * @brief The concept is satisfied when `AS` is a valid action action specifier.
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_action_specifier = c_value_action_specifier<AS> or std::same_as<AS, action_type::on_flag>;

/// @brief Template argument action callable type alias.
template <c_value_action_specifier AS, ap::detail::c_argument_value_type T>
using callable_type = typename AS::template type<T>;

/// @brief Template argument action callabla variant type alias.
template <ap::detail::c_argument_value_type T>
using value_action_variant_type = std::variant<
    callable_type<action_type::observe, T>,
    callable_type<action_type::transform, T>,
    callable_type<action_type::modify, T>>;

/**
 * @brief A visitor structure used to apply *value* actions.
 * @tparam T The argument's value type
 */
template <ap::detail::c_argument_value_type T>
struct apply_visitor {
    using value_type = T;

    /**
     * @brief The call operator overload for the *observe* action type.
     * @param action The *observe* action callable.
     */
    void operator()(const callable_type<action_type::observe, value_type>& action) const {
        action(std::as_const(value));
    }

    /**
     * @brief The call operator overload for the *transform* action type.
     * @param action The *transform* action callable.
     */
    void operator()(const callable_type<action_type::transform, value_type>& action) const {
        value = action(std::as_const(value));
    }

    /**
     * @brief The call operator overload for the *modify* action type.
     * @param action The *modify* action callable.
     */
    void operator()(const callable_type<action_type::modify, value_type>& action) const {
        action(value);
    }

    value_type& value; ///< A reference to the argument's value for which the action will be applied.
};

} // namespace ap::action::detail
