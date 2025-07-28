// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file argument_name.hpp

#pragma once

#include <format>
#include <optional>
#include <string>
#include <string_view>
#include <cstdint>

namespace ap::detail {

/// @brief Structure holding the argument's name.
struct argument_name {
    enum class match_type : std::uint8_t { m_any, m_primary, m_secondary };
    using enum match_type;

    argument_name() = delete;

    argument_name& operator=(const argument_name&) = delete;
    argument_name& operator=(argument_name&&) = delete;

    argument_name(const argument_name&) = default;
    argument_name(argument_name&&) = default;

    /**
     * @brief Primary name constructor.
     * @param primary The primary name of the argument.
     */
    argument_name(std::string_view primary) : primary(primary) {}

    /**
     * @brief Primary and secondary name constructor.
     * @param primary The primary name of the argument.
     * @param secondary The secondary (short) name of the argument.
     */
    argument_name(std::string_view primary, std::string_view secondary)
    : primary(primary), secondary(secondary) {}

    /// @brief Class destructor.
    ~argument_name() = default;

    /**
     * @brief Equality comparison operator.
     * @param other The argument_name instance to compare with.
     * @return Equality of argument names.
     */
    bool operator==(const argument_name& other) const noexcept {
        if (not (this->secondary and other.secondary) and (this->secondary or other.secondary))
            return false;

        if (this->primary != other.primary)
            return false;

        return this->secondary ? this->secondary.value() == other.secondary.value() : true;
    }

    /**
     * @brief Matches the given string to the argument_name instance.
     * @param arg_name The name string to match.
     * @return True if name is equal to either the primary or the secondary name of the argument_name instance.
     */
    [[nodiscard]] bool match(std::string_view arg_name, const match_type m_type = m_any) const noexcept {
        switch (m_type) {
        case m_any:
            return this->match_primary(arg_name) or this->match_secondary(arg_name);
        case m_primary:
            return this->match_primary(arg_name);
        case m_secondary:
            return this->match_secondary(arg_name);
        }

        return false;
    }

    /**
     * @brief Matches the given argument name to the argument_name instance.
     * @param arg_name The argument_name instance to match.
     * @return True if arg_name's primary or secondary value matches the argument_name instance.
     */
    [[nodiscard]] bool match(const argument_name& arg_name, [[maybe_unused]] const match_type m_type = m_any) const noexcept {
        if (this->match(arg_name.primary))
            return true;

        if (arg_name.secondary)
            return this->match(arg_name.secondary.value());

        return false;
    }

    /**
     * @brief Matches the given string to the primary name of an argument_name instance.
     * @param arg_name The name string to match.
     * @return True if name is equal to either the primary name of the argument_name instance.
     */
    [[nodiscard]] bool match_primary(std::string_view arg_name) const noexcept {
        return arg_name == this->primary;
    }

    /**
     * @brief Matches the given string to the primary name of an argument_name instance.
     * @param arg_name The name string to match.
     * @return True if name is equal to either the primary name of the argument_name instance.
     */
    [[nodiscard]] bool match_secondary(std::string_view arg_name) const noexcept {
        return this->secondary and arg_name == this->secondary.value();
    }

    /**
     * @brief Get a string representation of the argument_name.
     * @param flag_char The character used for the argument flag prefix.
     */
    [[nodiscard]] std::string str(const std::optional<char> flag_char = std::nullopt)
        const noexcept {
        // if flag_char = nullopt, then the fallback character doesn't matter - the string will be empty
        const std::string fc(flag_char.has_value(), flag_char.value_or(char()));
        return this->secondary
                 ? std::format("{}{}{}, {}{}", fc, fc, this->primary, fc, this->secondary.value())
                 : std::format("{}{}{}", fc, fc, this->primary);
    }

    /**
     * @brief Stream insertion operator for argument names.
     * @param os The output stream.
     * @param arg_name The argument name to be inserted into the stream.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const argument_name& arg_name) noexcept {
        os << arg_name.str();
        return os;
    }

    const std::string primary; ///< The primary name of the argument.
    const std::optional<std::string> secondary; ///< The optional (short) name of the argument.
};

} // namespace ap::detail
