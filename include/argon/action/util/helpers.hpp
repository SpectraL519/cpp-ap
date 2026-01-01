// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-ARGON project (https://github.com/SpectraL519/cpp-argon).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file argon/action/util/helpers.hpp
 * @brief Defines general action-related helper utility.
 */

#pragma once

#include "argon/action/util/concepts.hpp"

#include <utility>
#include <variant>

namespace argon::action::util {

/// @brief Template argument action callable type alias.
/// @ingroup util
template <c_value_action_specifier AS, argon::util::c_argument_value_type T>
using callable_type = typename AS::template type<T>;

/// @brief Template argument action callabla variant type alias.
template <argon::util::c_argument_value_type T>
using value_action_variant_type = std::variant<
    callable_type<action_type::observe, T>,
    callable_type<action_type::transform, T>,
    callable_type<action_type::modify, T>>;

/**
 * @brief A visitor structure used to apply *value* actions.
 * @tparam T The argument's value type
 * @ingroup util
 */
template <argon::util::c_argument_value_type T>
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

} // namespace argon::action::util
