// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-ARGON project (https://github.com/SpectraL519/cpp-argon).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file argon/detail/argument_base.hpp
 * @brief Defines the base argument class and common utility.
 */

#pragma once

#include "argon/detail/argument_name.hpp"
#include "argon/detail/help_builder.hpp"

#include <any>
#include <iostream>
#include <vector>

namespace argon {

class argument_parser;

namespace detail {

/// @brief Argument class interface
class argument_base {
public:
    virtual ~argument_base() = default;

    /// @return `true` if the argument is a positional argument instance, `false` otherwise.
    virtual bool is_positional() const noexcept = 0;

    /// @return `true` if the argument is an optional argument instance, `false` otherwise.
    virtual bool is_optional() const noexcept = 0;

    /// @return Returns the argument's name.
    virtual const argument_name& name() const noexcept = 0;

    /// @return Returns the argument's help message.
    virtual const std::optional<std::string>& help() const noexcept = 0;

    /// @return `true` if the argument is hidden from help output, `false` otherwise.
    virtual bool is_hidden() const noexcept = 0;

    /// @return `true` if the argument is required, `false` otherwise.
    virtual bool is_required() const noexcept = 0;

    /// @return `true` if argument checks suppressing is enabled for the argument, `false` otherwise.
    virtual bool suppresses_arg_checks() const noexcept = 0;

    /// @return `true` if argument group checks suppressing is enabled for the argument, `false` otherwise.
    virtual bool suppresses_group_checks() const noexcept = 0;

    /// @return `true` if the argument is greedy, `false` otherwise.
    virtual bool is_greedy() const noexcept = 0;

    friend class ::argon::argument_parser;

protected:
    /// @param verbose The verbosity mode value. If `true` all non-default parameters will be included in the output.
    /// @return A help message builder object for the argument.
    virtual detail::help_builder help_builder(const bool verbose) const noexcept = 0;

    /// @brief Mark the argument as used.
    /// @return `true` if the argument accepts further values, `false` otherwise.
    virtual bool mark_used() = 0;

    /// @return `true` if the argument has been used, `false` otherwise.
    virtual bool is_used() const noexcept = 0;

    /// @return The number of times an argument has been used.
    virtual std::size_t count() const noexcept = 0;

    /// @brief Set the value for the argument.
    /// @param value The string representation of the value.
    /// @return `true` if the argument accepts further values, `false` otherwise.
    virtual bool set_value(const std::string& value) = 0;

    /// @return `true` if the argument has a value, `false` otherwise.
    virtual bool has_value() const noexcept = 0;

    /// @return `true` if the argument has parsed values., `false` otherwise.
    virtual bool has_parsed_values() const noexcept = 0;

    /// @return `true` if the argument has predefined values, `false` otherwise.
    virtual bool has_predefined_values() const noexcept = 0;

    /// @return The ordering relationship of argument range.
    virtual std::weak_ordering nvalues_ordering() const noexcept = 0;

    /// @return Reference to the stored value of the argument.
    virtual const std::any& value() const = 0;

    /// @return Reference to the vector of parsed values of the argument.
    virtual const std::vector<std::any>& values() const = 0;
};

} // namespace detail
} // namespace argon
