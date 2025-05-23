// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file utility.hpp
 * @brief Defines general action-related utility.
 */

#pragma once

#include "ap/action/specifiers.hpp"

#include <utility>
#include <variant>

namespace ap::action::detail {

/**
 * @brief The concept is satisfied when `AS` is either a valued or void argument action
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_value_action_specifier =
    ap::detail::c_one_of<AS, action_type::observe, action_type::transform, action_type::modify>;

/// @brief Template argument action callable type alias.
template <c_value_action_specifier AS, ap::detail::c_argument_value_type T>
using callable_type = typename AS::template type<T>;

/// @brief Template argument action callabla variant type alias.
template <ap::detail::c_argument_value_type T>
using value_action_variant_type = std::variant<
    callable_type<action_type::observe, T>,
    callable_type<action_type::transform, T>,
    callable_type<action_type::modify, T>>;

template <ap::detail::c_argument_value_type T>
struct apply_visitor {
    using value_type = T;

    void operator()(const callable_type<action_type::observe, value_type>& action) const noexcept {
        action(std::as_const(value));
    }

    void operator()(const callable_type<action_type::transform, value_type>& action
    ) const noexcept {
        value = action(std::as_const(value));
    }

    void operator()(const callable_type<action_type::modify, value_type>& action) const noexcept {
        action(value);
    }

    value_type& value;
};

} // namespace ap::action::detail
