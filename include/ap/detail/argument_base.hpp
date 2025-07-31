// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file argument_base.hpp
 * @brief Defines the base argument class and common utility.
 */

#pragma once

#include "argument_descriptor.hpp"
#include "argument_name.hpp"

#include <any>
#include <iostream>
#include <vector>

namespace ap {

class argument_parser;

namespace detail {

/// @brief Argument class interface
class argument_base {
public:
    virtual ~argument_base() = default;

    friend class ::ap::argument_parser;

protected:
    argument_base(const argument_name& name, const bool required = false)
    : _name(name), _required(required) {}

    /// @return Reference the name of the positional argument.
    [[nodiscard]] const ap::detail::argument_name& name() const noexcept {
        return this->_name;
    }

    /// @return Optional help message for the positional argument.
    [[nodiscard]] const std::optional<std::string>& help() const noexcept {
        return this->_help_msg;
    }

    /// @return `true` if the argument is required, `false` otherwise
    [[nodiscard]] bool is_required() const noexcept {
        return this->_required;
    }

    /**
     * @return `true` if required argument bypassing is enabled for the argument, `false` otherwise.
     * @note Required argument bypassing is enabled only when both `required` and `bypass_required` flags are set to `true`.
     */
    [[nodiscard]] bool bypass_required_enabled() const noexcept {
        return not this->_required and this->_bypass_required;
    }

    /**
     * @param verbose The verbosity mode value.
     * @param flag_char The character used for the argument flag prefix.
     * @return An argument descriptor object for the argument.
     */
    virtual detail::argument_descriptor desc(const bool verbose, const char flag_char)
        const noexcept = 0;

    /**
     * @brief Mark the argument as used.
     * @return `true` if the argument accepts further values, `false` otherwise.
     */
    virtual bool mark_used() = 0;

    /// @return `true` if the argument has been used, `false` otherwise.
    virtual bool is_used() const noexcept = 0;

    /// @return The number of times the positional argument is used.
    virtual std::size_t count() const noexcept = 0;

    /**
     * @brief Set the value for the argument.
     * @param value The string representation of the value.
     * @return `true` if the argument accepts further values, `false` otherwise.
     */
    virtual bool set_value(const std::string& value) = 0;

    /// @return `true` if the argument has a value, `false` otherwise.
    virtual bool has_value() const noexcept = 0;

    /// @return `true` if the argument has parsed values., `false` otherwise.
    virtual bool has_parsed_values() const noexcept = 0;

    /// @return The ordering relationship of argument range.
    virtual std::weak_ordering nvalues_ordering() const noexcept = 0;

    /// @return Reference to the stored value of the argument.
    virtual const std::any& value() const = 0;

    /// @return Reference to the vector of parsed values of the argument.
    virtual const std::vector<std::any>& values() const = 0;

    const ap::detail::argument_name _name;
    std::optional<std::string> _help_msg;

    bool _required : 1;
    bool _bypass_required : 1 = false;
};

/**
 * @brief Checks if the provided choice is valid for the given set of choices.
 * @param value A value, the validity of which is to be checked.
 * @param choices The set against which the choice validity will be checked.
 * @return `true` if the choice is valid, `false` otherwise.
 * \todo replace with `std::ranges::contains` after transition to C++23
 */
template <c_argument_value_type T>
[[nodiscard]] bool is_valid_choice(const T& value, const std::vector<T>& choices) noexcept {
    return choices.empty() or std::ranges::find(choices, value) != choices.end();
}

} // namespace detail
} // namespace ap
