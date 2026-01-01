// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-ARGON project (https://github.com/SpectraL519/cpp-argon).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file argon/detail/argument_token.hpp

#pragma once

#include "argon/detail/argument_base.hpp"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace argon::detail {

/// @brief Structure representing a single command-line argument token.
struct argument_token {
    using arg_ptr_t = std::shared_ptr<detail::argument_base>; ///< Argument pointer type alias.
    using arg_ptr_vec_t = std::vector<arg_ptr_t>;

    /// @brief The token type discriminator.
    enum class token_type : std::uint8_t {
        t_value, ///< Represents a value argument.
        t_flag_primary, ///< Represents a primary (--) flag argument.
        t_flag_secondary, ///< Represents a secondary (-) flag argument.
        t_flag_compound ///< Represents a compound flag argument (secondary flag matching multiple arguments).
    };
    using enum token_type;

    /**
     * @brief Equality operator for comparing argument_token instances.
     * @param other An argument_token instance to compare with.
     * @return Boolean statement of equality comparison.
     */
    bool operator==(const argument_token& other) const noexcept {
        return this->type == other.type and this->value == other.value;
    }

    /**
     * @brief Checks whether the `type` member is a flag token type.
     * @return true if the token has a *flag* type
     * @note The token's type is considered a *flag* type if it has one of the following values:
     * @note - t_flag_primary
     * @note - t_flag_secondary
     * @note - t_flag_compound
     */
    [[nodiscard]] bool is_flag_token() const noexcept {
        switch (this->type) {
        case t_flag_primary:
            [[fallthrough]];
        case t_flag_secondary:
            [[fallthrough]];
        case t_flag_compound:
            return true;
        default:
            return false;
        }
    }

    /**
     * @brief Checks whether the token represents a valid flag.
     *
     * A token is considered a valid flag token if:
     * 1. The token's type if a valid flag token type (see @ref is_flag_token)
     * 2. The token's `arg` member is set.
     *
     * @return true if `type` is either `t_flag_primary` or `t_flag_secondary`, false otherwise.
     */
    [[nodiscard]] bool is_valid_flag_token() const noexcept {
        return this->is_flag_token() and not this->args.empty();
    }

    token_type type; ///< The token's type discrimiator value.
    std::string value; ///< The actual token's value.
    arg_ptr_vec_t args = {}; ///< The corresponding argument
};

} // namespace argon::detail
