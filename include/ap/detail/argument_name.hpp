// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace ap::detail {

/// @brief Structure holding the argument name.
struct argument_name {
    argument_name() = delete;

    argument_name(const argument_name&) = default;
    argument_name(argument_name&&) = default;

    argument_name& operator=(const argument_name&) = delete;
    argument_name& operator=(argument_name&&) = delete;

    /**
     * @brief Primary name constructor.
     * @param primary The primary name of the argument.
     */
    argument_name(std::string_view primary);

    /**
     * @brief Primary and secondary name constructor.
     * @param primary The primary name of the argument.
     * @param secondary The secondary (short) name of the argument.
     */
    argument_name(std::string_view primary, std::string_view secondary);

    /// @brief Class destructor.
    ~argument_name() = default;

    /**
     * @brief Equality comparison operator.
     * @param other The argument_name instance to compare with.
     * @return Equality of argument names.
     */
    bool operator==(const argument_name& other) const noexcept;

    /**
     * @brief Matches the given string to the argument_name instance.
     * @param arg_name The name string to match.
     * @return True if name is equal to either the primary or the secondary name of the argument_name instance.
     */
    [[nodiscard]] bool match(std::string_view arg_name) const noexcept;

    /**
     * @brief Matches the given argument name to the argument_name instance.
     * @param arg_name The argument_name instance to match.
     * @return True if arg_name's primary or secondary value matches the argument_name instance.
     */
    [[nodiscard]] bool match(const argument_name& arg_name) const noexcept;

    /// @brief Get a string representation of the argument_name.
    [[nodiscard]] std::string str() const noexcept;

    /**
     * @brief Stream insertion operator for argument names.
     * @param os The output stream.
     * @param arg_name The argument name to be inserted into the stream.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const argument_name& arg_name) noexcept;

    const std::string primary; ///< The primary name of the argument.
    const std::optional<std::string> secondary; ///< The optional (short) name of the argument.
};

} // namespace ap::detail
