// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file exceptions.hpp

#pragma once

#include "ap/detail/argument_name.hpp"
#include "ap/detail/str_utility.hpp"

#include <format>

namespace ap {

/// @brief Base type for the argument parser functionality errors/exceptions.
struct argument_parser_exception : public std::runtime_error {
    /// @param message A descriptive message providing information about the exception.
    explicit argument_parser_exception(const std::string& message) : std::runtime_error(message) {}
};

/// @brief Exception type used for invalid configuration of an argument parser or its arguments.
struct invalid_configuration : public argument_parser_exception {
    explicit invalid_configuration(const std::string& message)
    : argument_parser_exception(message) {}

    static invalid_configuration invalid_argument_name(
        const std::string_view arg_name, const std::string_view reason
    ) noexcept {
        return invalid_configuration(
            std::format("Given name [{}] is invalid.\nReason: {}", arg_name, reason)
        );
    }

    static invalid_configuration argument_name_used(const detail::argument_name& arg_name
    ) noexcept {
        return invalid_configuration(std::format("Given name [{}] already used.", arg_name.str()));
    }
};

/// @brief Exception type used for errors encountered during the argument parsing operation.
struct parsing_failure : public argument_parser_exception {
    explicit parsing_failure(const std::string& message) : argument_parser_exception(message) {}

    static parsing_failure unknown_argument(const std::string_view arg_name) noexcept {
        return parsing_failure(std::format("Unknown argument [{}].", arg_name));
    }

    static parsing_failure value_already_set(const detail::argument_name& arg_name) noexcept {
        return parsing_failure(
            std::format("Value for argument [{}] has already been set.", arg_name.str())
        );
    }

    static parsing_failure invalid_value(
        const detail::argument_name& arg_name, const std::string& value
    ) noexcept {
        return parsing_failure(
            std::format("Cannot parse value `{}` for argument [{}].", value, arg_name.str())
        );
    }

    static parsing_failure invalid_choice(
        const detail::argument_name& arg_name, const std::string& value
    ) noexcept {
        return parsing_failure(std::format(
            "Value `{}` is not a valid choice for argument [{}].", value, arg_name.str()
        ));
    }

    static parsing_failure required_argument_not_parsed(const detail::argument_name& arg_name
    ) noexcept {
        return parsing_failure(
            std::format("No values parsed for a required argument [{}]", arg_name.str())
        );
    }

    static parsing_failure argument_deduction_failure(const std::vector<std::string_view>& values
    ) noexcept {
        return parsing_failure(std::format(
            "Failed to deduce the argument for values [{}]", detail::join(values)
        ));
    }

    static parsing_failure invalid_nvalues(
        const detail::argument_name& arg_name, const std::weak_ordering ordering
    ) noexcept {
        if (std::is_lt(ordering))
            return parsing_failure(
                std::format("Not enough values provided for optional argument [{}]", arg_name.str())
            );

        if (std::is_gt(ordering))
            return parsing_failure(
                std::format("Too many values provided for optional argument [{}]", arg_name.str())
            );

        return parsing_failure(
            std::format("Invalid number of values provided for argument [{}]", arg_name.str())
        );
    }
};

/// @brief Exception type used for type-related errors.
/// \todo Use demangled type names
struct type_error : public argument_parser_exception {
    explicit type_error(const std::string& message) : argument_parser_exception(message) {}

    static type_error invalid_value_type(
        const detail::argument_name& arg_name, const std::type_info& value_type
    ) noexcept {
        return type_error(std::format(
            "Invalid value type specified for argument [{}] = {}.",
            arg_name.str(),
            value_type.name()
        ));
    }
};

/// @brief Exception type used for element lookup errors.
struct lookup_failure : public argument_parser_exception {
    explicit lookup_failure(const std::string& message) : argument_parser_exception(message) {}

    static lookup_failure argument_not_found(const std::string_view& arg_name) noexcept {
        return lookup_failure(std::format("Argument with given name [{}] not found.", arg_name));
    }
};

} // namespace ap
