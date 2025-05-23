// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file specifiers.hpp
 * @brief Defies the action specifier types.
 */

#pragma once

#include "ap/detail/concepts.hpp"

#include <functional>

namespace ap::action_type {

/**
 * @brief An observing *value* action specifier.
 *
 * Represents an argument action which observes the parsed value and
 * performs some logic on it without modifying it.
 */
struct observe {
    template <ap::detail::c_argument_value_type T>
    using type = std::function<void(const T&)>;
};

/**
 * @brief A transformating *value* action specifier.
 *
 * Represents an argument action which transforms the parsed value and
 * returns a new value with which the argument will be initialized.
 */
struct transform {
    template <ap::detail::c_argument_value_type T>
    using type = std::function<T(const T&)>;
};

/**
 * @brief A modifying *value* action specifier.
 *
 * Represents an argument action which modifies the value of an
 * already initialized argument.
 */
struct modify {
    template <ap::detail::c_argument_value_type T>
    using type = std::function<void(T&)>;
};

/**
 * @brief An on-flag action specifier.
 *
 * Represents an action associated with an argument that performs
 * some logic, independently of the parsed value.
 *
 * @note An on-flag action is executed immediately after parsing the argument's flag and before parsing the following values.
 */
struct on_flag {
    using type = std::function<void()>;
};

} // namespace ap::action_type
