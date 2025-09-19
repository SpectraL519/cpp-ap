// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/action/util/concepts.hpp
 * @brief Defines action-related concepts.
 */

#pragma once

#include "ap/action/types.hpp"

#include <utility>
#include <variant>

namespace ap::action::util {

/**
 * @brief The concept is satisfied when `AS` is a valid *value* action action specifier.
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_value_action_specifier =
    ap::util::c_one_of<AS, action_type::observe, action_type::transform, action_type::modify>;

/**
 * @brief The concept is satisfied when `AS` is a valid *on-flag* action action specifier.
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_flag_action_specifier = ap::util::c_one_of<AS, action_type::on_flag>;

/**
 * @brief The concept is satisfied when `AS` is a valid action action specifier.
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_action_specifier = c_value_action_specifier<AS> or std::same_as<AS, action_type::on_flag>;

} // namespace ap::action::util
