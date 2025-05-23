// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file positional.hpp

#pragma once

#include "ap/action/detail/utility.hpp"
#include "ap/action/predefined_actions.hpp"
#include "ap/detail/argument_interface.hpp"
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
template <ap::detail::c_argument_value_type T = std::string>
class positional : public ap::detail::argument_interface {
public:
    using value_type = T; ///< The argument's value type.
    using action_type = ap::action::detail::action_variant_type<T>;

    positional() = delete;

    /**
     * @brief Constructor for positional argument with the `name` identifier.
     * @param name The `name` identifier of the positional argument.
     */
    positional(const ap::detail::argument_name& name) : _name(name) {}

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
     * @tparam AS The action specifier type (valued_action or void_action).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the positional argument.
     */
    template <ap::action::detail::c_action_specifier AS, std::invocable<value_type&> F>
    positional& action(F&& action) noexcept {
        using callable_type = ap::action::detail::callable_type<AS, value_type>;
        this->_action = std::forward<callable_type>(action);
        return *this;
    }

    /// @return True if argument is optional, false otherwise.
    [[nodiscard]] bool is_optional() const noexcept override {
        return this->_optional;
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
    /// @return Reference the name of the positional argument.
    [[nodiscard]] const ap::detail::argument_name& name() const noexcept override {
        return this->_name;
    }

    /// @return Optional help message for the positional argument.
    [[nodiscard]] const std::optional<std::string>& help() const noexcept override {
        return this->_help_msg;
    }

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
    void mark_used() noexcept override {}

    /// @return True if the positional argument is used, false otherwise.
    [[nodiscard]] bool is_used() const noexcept override {
        return this->_value.has_value();
    }

    /// @return The number of times the positional argument is used.
    [[nodiscard]] std::size_t nused() const noexcept override {
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
    positional& set_value(const std::string& str_value) override {
        if (this->_value.has_value())
            throw error::value_already_set(this->_name);

        this->_ss.clear();
        this->_ss.str(str_value);

        value_type value;
        if (not (this->_ss >> value))
            throw error::invalid_value(this->_name, this->_ss.str());

        if (not this->_is_valid_choice(value))
            throw error::invalid_choice(this->_name, str_value);

        this->_apply_action(value);

        this->_value = value;
        return *this;
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
    [[nodiscard]] std::weak_ordering nvalues_in_range() const noexcept override {
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

    /**
     * @brief Check if the provided choice is valid for the positional argument.
     * @param choice The value to check against choices.
     * @return True if the choice valid, false otherwise.
     */
    [[nodiscard]] bool _is_valid_choice(const value_type& choice) const noexcept {
        // TODO: replace with `std::ranges::contains` after transition to C++23
        return this->_choices.empty()
            or std::ranges::find(this->_choices, choice) != this->_choices.end();
    }

    /**
     * @brief Apply the specified action to the value of the positional argument.
     * @param value The value to apply the action to.
     */
    void _apply_action(value_type& value) const noexcept {
        if (action::detail::is_modify_action(this->_action)) {
            using callable_type =
                action::detail::callable_type<ap::action_type::modify, value_type>;
            std::get<callable_type>(this->_action)(value);
        }
        else {
            using callable_type =
                action::detail::callable_type<ap::action_type::transform, value_type>;
            value = std::get<callable_type>(this->_action)(value);
        }
    }

    static constexpr bool _optional = false;

    const ap::detail::argument_name _name;
    std::optional<std::string> _help_msg;

    static constexpr bool _required = true; ///< Positional arguments are required by default.
    static constexpr bool _bypass_required =
        false; ///< Bypassing required status is defaultly not allowed for positional arguments.
    std::vector<value_type> _choices; ///< Vector of valid choices for the positional argument.
    action_type _action =
        ap::action::none<value_type>(); ///< Action associated with the positional argument.

    std::any _value; ///< Stored value of the positional argument.

    std::stringstream _ss; ///< Stringstream used for parsing values.
};

} // namespace ap::argument
