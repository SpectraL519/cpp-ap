// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ap/detail/argument_name.hpp"

#include <format>

namespace ap {

/**
 * @brief Base class for exceptions thrown by the argument parser.
 *
 * This class is derived from std::runtime_error and serves as the base class for all
 * exceptions related to the argument parser functionality.
 */
class argument_parser_exception : public std::runtime_error {
public:
    /**
     * @brief Constructor for the argument_parser_exception class.
     * @param message A descriptive error message providing information about the exception.
     */
    explicit argument_parser_exception(const std::string& message) : std::runtime_error(message) {}
};

namespace error {

/// @brief Exception thrown when attempting to set a value for an argument that already has one.
class value_already_set : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the value_already_set class.
     * @param arg_name The name of the argument that already has a value set.
     */
    explicit value_already_set(const detail::argument_name& arg_name)
    : argument_parser_exception(
          std::format("Value for argument {} has already been set.", arg_name.str())
      ) {}
};

/// @brief Exception thrown when the value provided for an argument cannot be parsed.
class invalid_value : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the invalid_value class.
     * @param arg_name The name of the argument for which the value parsing failed.
     * @param value The value that failed to parse.
     */
    explicit invalid_value(const detail::argument_name& arg_name, const std::string& value)
    : argument_parser_exception(
          std::format("Cannot parse value `{}` for argument {}.", value, arg_name.str())
      ) {}
};

/// @brief Exception thrown when the provided value is not in the choices for an argument.
class invalid_choice : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the invalid_choice class.
     * @param arg_name The name of the argument for which the value is not in choices.
     * @param value The value that is not in the allowed choices.
     */
    explicit invalid_choice(const detail::argument_name& arg_name, const std::string& value)
    : argument_parser_exception(
          std::format("Value `{}` is not a valid choice for argument {}.", value, arg_name.str())
      ) {}
};

class invalid_argument_name_pattern : public argument_parser_exception {
public:
    explicit invalid_argument_name_pattern(
        const std::string_view arg_name, const std::string_view reason
    )
    : argument_parser_exception(
          std::format("Given name `{}` is invalid.\nReason: {}", arg_name, reason)
      ) {}
};

/// @brief Exception thrown when there is a collision in argument names.
class argument_name_used : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the argument_name_used class.
     * @param arg_name The name of the argument causing the collision.
     */
    explicit argument_name_used(const detail::argument_name& arg_name)
    : argument_parser_exception(std::format("Given name `{}` already used.", arg_name.str())) {}
};

/// @brief Exception thrown when an argument with a specific name is not found.
class argument_not_found : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the argument_name_not_found_error class.
     * @param arg_name The name of the argument that was not found.
     */
    explicit argument_not_found(const std::string_view& arg_name)
    : argument_parser_exception(std::format("Argument with given name `{}` not found.", arg_name)) {
    }
};

/// @brief Exception thrown when there is an attempt to cast to an invalid type.
class invalid_value_type : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the invalid_value_type class.
     * @param arg_name The name of the argument that had invalid value type.
     * @param value_type The type information that failed to cast.
     */
    explicit invalid_value_type(
        const detail::argument_name& arg_name, const std::type_info& value_type
    )
    : argument_parser_exception(std::format(
          "Invalid value type specified for argument {} = {}.", arg_name.str(), value_type.name()
      )) {}
};

/// @brief Exception thrown when a required argument is not parsed.
class required_argument_not_parsed : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the required_argument_not_parsed class.
     * @param arg_name The name of the required argument that was not parsed.
     */
    explicit required_argument_not_parsed(const detail::argument_name& arg_name)
    : argument_parser_exception("No values parsed for a required argument " + arg_name.str()) {}
};

/// @brief Exception thrown when there is an error deducing the argument for a given value.
class free_value : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the free_value class.
     * @param value The value for which the argument deduction failed.
     */
    explicit free_value(const std::string& value)
    : argument_parser_exception(
          std::format("Failed to deduce the argument for the given value `{}`", value)
      ) {}
};

/// @brief Exception thrown when an invalid number of values is provided for an argument.
class invalid_nvalues : public argument_parser_exception {
public:
    /**
     * @brief Constructor for the invalid_nvalues class.
     * @param ordering The result of the weak_ordering comparison.
     * @param arg_name The name of the argument for which the error occurred.
     */
    explicit invalid_nvalues(
        const std::weak_ordering ordering, const detail::argument_name& arg_name
    )
    : argument_parser_exception(msg(ordering, arg_name)) {}

private:
    /**
     * @brief Helper function to generate an error message based on the weak_ordering result.
     * @param ordering The result of the weak_ordering comparison.
     * @param arg_name The name of the argument for which the error occurred.
     * @return The error message.
     */
    [[nodiscard]] static std::string msg(
        const std::weak_ordering ordering, const detail::argument_name& arg_name
    ) {
        if (std::is_lt(ordering))
            return "Not enought values provided for optional argument " + arg_name.str();
        else
            return "Too many values provided for optional argument " + arg_name.str();
    }
};

} // namespace error
} // namespace ap
