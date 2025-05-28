// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file positional.hpp

#pragma once

#include "ap/action/detail/utility.hpp"
#include "ap/action/predefined_actions.hpp"
#include "ap/detail/argument_base.hpp"
#include "ap/detail/concepts.hpp"

#ifdef AP_TESTING

namespace ap_testing {
struct positional_argument_test_fixture;
} // namespace ap_testing

#endif

namespace ap::argument {

/**
 * @brief The positional argument class.
 * @tparam T The argument's value type.
 */
template <detail::c_argument_value_type T = std::string>
class positional : public detail::argument_base {
public:
    using value_type = T; ///< The argument's value type.

    positional() = delete;

    /**
     * @brief Constructor for positional argument with the `name` identifier.
     * @param name The `name` identifier of the positional argument.
     */
    positional(const detail::argument_name& name) : argument_base(name) {}

    ~positional() = default;

    /**
     * @brief Equality operator for positional argument.
     * @param other Another positional argument for comparison.
     * @return Result of equality
     */
    bool operator==(const positional& other) const noexcept {
        return this->_name == other._name;
    }

    /**
     * @brief Set the help message for the positional argument.
     * @param help_msg The help message to set.
     * @return Reference to the positional argument.
     */
    positional& help(std::string_view help_msg) noexcept {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Set the choices for the positional argument.
     * @tparam CR The choices range type.
     * @param choices The range of valid choices for the argument.
     * @return Reference to the positional argument.
     * @note `value_type` must be equality comparable.
     * @note `CR` must be a range such that its value type is convertible to `value_type`.
     */
    template <detail::c_range_of<value_type, detail::type_validator::convertible> CR>
    positional& choices(const CR& choices) noexcept
    requires(std::equality_comparable<value_type>)
    {
        for (const auto& choice : choices)
            this->_choices.emplace_back(choice);
        return *this;
    }

    /**
     * @brief Set the choices for the positional argument.
     * @param choices The list of valid choices for the argument.
     * @return Reference to the positional argument.
     * @note `value_type` must be equality comparable.
     */
    positional& choices(std::initializer_list<value_type> choices) noexcept
    requires(std::equality_comparable<value_type>)
    {
        return this->choices<>(choices);
    }

    /**
     * @brief Set the action for the positional argument.
     * @tparam AS The value action specifier type (valued_action or void_action).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the positional argument.
     */
    template <action::detail::c_value_action_specifier AS, std::invocable<value_type&> F>
    positional& action(F&& action) noexcept {
        using callable_type = action::detail::callable_type<AS, value_type>;
        this->_value_actions.emplace_back(std::forward<callable_type>(action));
        return *this;
    }

    /// @brief Friend class declaration for access by argument_parser.
    friend class ::ap::argument_parser;

#ifdef AP_TESTING
    /**
     * @brief Friend struct declaration for testing purposes.
     */
    friend struct ::ap_testing::positional_argument_test_fixture;
#endif

private:
    using value_action_type =
        action::detail::value_action_variant_type<T>; ///< The argument's value action type.

    /**
     * @param verbose The verbosity mode value.
     * @return An argument_descriptor instance for the argument.
     */
    [[nodiscard]] detail::argument_descriptor desc(const bool verbose) const noexcept override {
        detail::argument_descriptor desc(this->_name.str(), this->_help_msg);

        if (not verbose)
            return desc;

        if constexpr (detail::c_writable<value_type>) {
            if (not this->_choices.empty())
                desc.add_range_param("choices", this->_choices);
        }

        return desc;
    }

    /// @return True if the positional argument is required, false otherwise
    [[nodiscard]] bool is_required() const noexcept override {
        return this->_required;
    }

    /// @return True if bypassing the required status is enabled for the positional argument, false otherwise.
    [[nodiscard]] bool bypass_required_enabled() const noexcept override {
        return this->_bypass_required;
    }

    /**
     * @brief Mark the positional argument as used.
     * @note No logic is performed for positional arguments
     */
    bool mark_used() override {
        return false;
    }

    /// @return True if the positional argument is used, false otherwise.
    [[nodiscard]] bool is_used() const noexcept override {
        return this->_value.has_value();
    }

    /// @return The number of times the positional argument is used.
    [[nodiscard]] std::size_t count() const noexcept override {
        return static_cast<std::size_t>(this->_value.has_value());
    }

    /**
     * @brief Set the value for the positional argument.
     * @param str_value The string representation of the value.
     * @return Reference to the positional argument.
     * @throws ap::error::value_already_set
     * @throws ap::error::invalid_value
     * @throws ap::error::invalid_choice
     */
    bool set_value(const std::string& str_value) override {
        if (this->_value.has_value())
            throw error::value_already_set(this->_name);

        value_type value;
        if (not (std::istringstream(str_value) >> value))
            throw error::invalid_value(this->_name, str_value);

        if (not detail::is_valid_choice(value, this->_choices))
            throw error::invalid_choice(this->_name, str_value);

        const auto apply_visitor = action::detail::apply_visitor<value_type>{value};
        for (const auto& action : this->_value_actions)
            std::visit(apply_visitor, action);

        this->_value = value;
        return false;
    }

    /// @return True if the positional argument has a value, false otherwise.
    [[nodiscard]] bool has_value() const noexcept override {
        return this->_value.has_value();
    }

    /// @return True if the positional argument has parsed values, false otherwise.
    [[nodiscard]] bool has_parsed_values() const noexcept override {
        return this->has_value();
    }

    /// @return Ordering relationship of positional argument range.
    [[nodiscard]] std::weak_ordering nvalues_ordering() const noexcept override {
        return this->_value.has_value() ? std::weak_ordering::equivalent : std::weak_ordering::less;
    }

    /**
     * @brief Get the stored value of the positional argument.
     * @throws std::logic_error
     */
    [[nodiscard]] const std::any& value() const override {
        if (not this->_value.has_value())
            throw std::logic_error(
                std::format("No value parsed for the `{}` positional argument.", this->_name.str())
            );
        return this->_value;
    }

    /**
     * @return Reference to the vector of parsed values for the positional argument.
     * @throws std::logic_error
     */
    [[nodiscard]] const std::vector<std::any>& values() const override {
        throw std::logic_error(
            std::format("Positional argument `{}` has only 1 value.", this->_name.primary)
        );
    }

    static constexpr bool _required = true;
    static constexpr bool _bypass_required = false;
    std::vector<value_type> _choices;
    std::vector<value_action_type> _value_actions;

    std::any _value; ///< Stored value of the positional argument.
};

} // namespace ap::argument
