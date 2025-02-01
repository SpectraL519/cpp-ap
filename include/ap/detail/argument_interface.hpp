// Copyright (c) 2024-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "argument_name.hpp"

#include <any>
#include <iostream>
#include <vector>

namespace ap {

class argument_parser;

namespace detail {

/// @brief Argument class interface
class argument_interface {
public:
    virtual ~argument_interface() = default;

    /**
     * @brief Set the help message for the argument.
     * @param msg The help message to set.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& help(std::string_view) noexcept = 0;

    /// @return True if the argument is optional, false otherwise.
    virtual bool is_optional() const noexcept = 0;

    /**
     * @brief Overloaded stream insertion operator.
     * @param os The output stream.
     * @param argument The argument_interface to output.
     * @return The output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const argument_interface& argument) noexcept {
        os << argument.name() << " : " << argument.help().value_or("No help message provided");
        return os;
    }

    friend class ::ap::argument_parser;

protected:
    /// @return Reference to the name of the argument.
    virtual const argument_name& name() const noexcept = 0;

    /// @return True if the argument is required, false otherwise
    virtual bool is_required() const noexcept = 0;

    /// @return True if bypassing the required status is enabled for the argument, false otherwise.
    virtual bool bypass_required_enabled() const noexcept = 0;

    /// @return Optional help message for the argument.
    virtual const std::optional<std::string>& help() const noexcept = 0;

    /// @brief Mark the argument as used.
    virtual void mark_used() noexcept = 0;

    /// @return True if the argument has been used, false otherwise.
    virtual bool is_used() const noexcept = 0;

    /// @return The number of times the positional argument is used.
    virtual std::size_t nused() const noexcept = 0;

    /**
     * @brief Set the value for the argument.
     * @param value The string representation of the value.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& set_value(const std::string&) = 0;

    /// @return True if the argument has a value, false otherwise.
    virtual bool has_value() const noexcept = 0;

    /// @return True if the argument has parsed values., false otherwise.
    virtual bool has_parsed_values() const noexcept = 0;

    /// @return The ordering relationship of argument range.
    virtual std::weak_ordering nvalues_in_range() const noexcept = 0;

    /// @return Reference to the stored value of the argument.
    virtual const std::any& value() const = 0;

    /// @return Reference to the vector of parsed values of the argument.
    virtual const std::vector<std::any>& values() const = 0;
};

} // namespace detail
} // namespace ap
