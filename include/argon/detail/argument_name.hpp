// Copyright (c) 2023-2026 Jakub Musiał
// This file is part of the CPP-ARGON project (https://github.com/SpectraL519/cpp-argon).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file argon/detail/argument_name.hpp

#pragma once

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <string_view>

namespace argon {

namespace detail {

/// @brief Structure holding the argument's name.
struct argument_name {
    /// @brief Specifies the type of argument name match.
    enum class match_type : std::uint8_t {
        m_any, ///< Matches either the primary or the secondary name.
        m_primary, ///< Matches only the primary name.
        m_secondary ///< Matches only the secondary name.
    };
    using enum match_type;

    argument_name() = delete;

    argument_name& operator=(const argument_name&) = delete;
    argument_name& operator=(argument_name&&) = delete;

    argument_name(const argument_name&) = default;
    argument_name(argument_name&&) = default;

    /**
     * @brief Primary and secondary name constructor.
     * @param primary The primary name of the argument.
     * @param secondary The secondary (short) name of the argument.
     * @param flag_char The flag character (used for optional argument names).
     */
    argument_name(
        std::optional<std::string> primary,
        std::optional<std::string> secondary = std::nullopt,
        std::optional<char> flag_char = std::nullopt
    )
    : primary(std::move(primary)),
      secondary(std::move(secondary)),
      flag_char(std::move(flag_char)) {
        if (not (this->primary or this->secondary))
            throw std::logic_error("An argument name cannot be empty! At least one of "
                                   "primary/secondary must be specified");
    }

    /// @brief Class destructor.
    ~argument_name() = default;

    /**
     * @brief Equality comparison operator.
     * @param other The argument_name instance to compare with.
     * @return Equality of argument names.
     */
    bool operator==(const argument_name& other) const noexcept {
        return this->primary == other.primary and this->secondary == other.secondary;
    }

    /**
     * @brief Matches the given string to the argument_name instance.
     * @param arg_name The name string to match.
     * @param m_type The match type used to find the argument.
     * @return `true` if the given name matches the primary/secondary name (depending on the match type).
     */
    [[nodiscard]] bool match(std::string_view arg_name, const match_type m_type = m_any)
        const noexcept {
        switch (m_type) {
        case m_any:
            return this->primary == arg_name or this->secondary == arg_name;
        case m_primary:
            return this->primary == arg_name;
        case m_secondary:
            return this->secondary == arg_name;
        }

        return false;
    }

    /**
     * @brief Matches the given argument name to the argument_name instance.
     * @param arg_name The argument_name instance to match.
     * @param m_type UNUSED - necessary to match the signature of the `string_view` overload of the `match` function.
     * @return True if arg_name's primary or secondary value matches the argument_name instance.
     */
    [[nodiscard]] bool match(
        const argument_name& arg_name, [[maybe_unused]] const match_type m_type = m_any
    ) const noexcept {
        if (arg_name.primary and this->match(arg_name.primary.value()))
            return true;

        if (arg_name.secondary)
            return this->match(arg_name.secondary.value());

        return false;
    }

    /**
     * @brief Get a string representation of the argument_name.
     * @param flag_char The character used for the argument flag prefix.
     */
    [[nodiscard]] std::string str() const noexcept {
        // if flag_char = nullopt, then the fallback character doesn't matter - the string will be empty
        const std::string fc(this->flag_char.has_value(), this->flag_char.value_or(char()));

        std::string primary_str =
            this->primary ? std::format("{}{}{}", fc, fc, this->primary.value()) : "";
        std::string separator = this->primary and this->secondary ? ", " : "";
        std::string secondary_str =
            this->secondary ? std::format("{}{}", fc, this->secondary.value()) : "";

        return std::format("{}{}{}", primary_str, separator, secondary_str);
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

    const std::optional<std::string> primary; ///< The primary name of the argument.
    const std::optional<std::string> secondary; ///< The optional (short) name of the argument.
    const std::optional<char> flag_char; ///< The flag character (used for optional argument names).
};

/**
 * @brief Argument name member discriminator.
 *
 * This discriminator type is used in the argument initializing methods of the `argument_parser` class.
 */
enum class argument_name_discriminator : bool {
    n_primary, ///< Represents the primary name (used with a long flag prefix --).
    n_secondary ///< Represents the secondary name (used with a short flag prefix --).
};

} // namespace detail

using enum detail::argument_name_discriminator;

} // namespace argon
