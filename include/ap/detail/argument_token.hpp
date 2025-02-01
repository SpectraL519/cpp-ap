#pragma once

#include <string>

namespace ap::detail {

/// @brief Structure representing a single command-line argument token.
struct argument_token {
    /// @brief The token type discriminator.
    enum class token_type : bool { t_flag, t_value };
    using enum token_type;

    argument_token() = default;

    argument_token(const argument_token&) = default;
    argument_token(argument_token&&) = default;

    argument_token& operator=(const argument_token&) = default;
    argument_token& operator=(argument_token&&) = default;

    /**
     * @brief Constructor of a command-line argument.
     * @param type Type type of the token (flag or value).
     * @param value The value of the argument.
     */
    argument_token(const token_type type, const std::string& value) : type(type), value(value) {}

    ~argument_token() = default;

    /**
     * @brief Equality operator for comparing argument_token instances.
     * @param other An argument_token instance to compare with.
     * @return Boolean statement of equality comparison.
     */
    bool operator==(const argument_token& other) const noexcept {
        return this->type == other.type and this->value == other.value;
    }

    token_type type;
    std::string value;
};

} // namespace ap::detail
