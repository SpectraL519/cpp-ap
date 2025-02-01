// Copyright (c) 2024-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ap/detail/concepts.hpp"

#include <functional>

namespace ap::action_type {

// TODO:
// * on_read_action
// * on_flag_action

/*
 * @brief Represents a transformating action.
 *
 * Represents an argument action which transforms the parsed value and
 * returns a new value with which the argument will be initialized.
 */
struct transform {
    template <ap::detail::c_argument_value_type T>
    using type = std::function<T(const T&)>;
};

/*
 * @brief Represents a modifying action.
 *
 * Represents an argument action which modifies the value of an
 * already initialized argument.
 *
 * NOTE: The modify action doesn't have to actually modify the
 * underlying value - it can simply perform some action on it.
 * Example: `ap::action::check_file_exists`
 */
struct modify {
    template <ap::detail::c_argument_value_type T>
    using type = std::function<void(T&)>;
};

} // namespace ap::action_type
