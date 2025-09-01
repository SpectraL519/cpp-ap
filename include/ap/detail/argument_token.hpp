// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file argument_token.hpp

#pragma once

#include "argument_base.hpp"
#include "typing_utility.hpp"

#include <cstdint>
#include <functional>
#include <optional>
#include <string>

namespace ap::detail {

/// @brief Structure representing a single command-line argument token.
struct argument_token {
    using arg_ptr_opt_t = uptr_opt_t<detail::argument_base>;

    /// @brief The token type discriminator.
    enum class token_type : std::uint8_t {
        t_flag_primary, ///< Represents the primary (--) flag argument.
        t_flag_secondary, ///< Represents the secondary (-) flag argument.
        t_value ///< Represents a value argument.
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
     * @return true if `type` is either `t_flag_primary` or `t_flag_secondary`, false otherwise.
     */
    [[nodiscard]] bool is_flag_token() const noexcept {
        return this->type == t_flag_primary or this->type == t_flag_secondary;
    }

    token_type type; ///< The token's type discrimiator value.
    std::string value; ///< The actual token's value.
    arg_ptr_opt_t arg = std::nullopt; ///< The corresponding argument.
};

} // namespace ap::detail
