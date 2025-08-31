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
    positional(const detail::argument_name& name) : argument_base(name, true) {}

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
     * @brief Set the `hidden` attribute for the positional argument.
     * @param h The attribute value.
     * @return Reference to the positional argument.
     */
    positional& hidden(const bool h = true) noexcept {
        this->_hidden = h;
        return *this;
    }

    /**
     * @brief Set the `required` attribute of the positional argument
     * @param r The attribute value.
     * @return Reference to the positional argument.
     * @attention Setting the `required` attribute to true disables the `bypass_required` attribute.
     */
    positional& required(const bool r = true) noexcept {
        this->_required = r;
        if (this->_required)
            this->_bypass_required = false;
        return *this;
    }

    /**
     * @brief Enable/disable bypassing the `required` attributeattribute for the positional argument.
     * @param br The attribute value.
     * @return Reference to the positional argument.
     * @attention Setting the `bypass_required` attribute to true disables the `required` attribute.
     */
    positional& bypass_required(const bool br = true) noexcept {
        this->_bypass_required = br;
        if (this->_bypass_required)
            this->_required = false;
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
     * @brief Set the default value for the positional argument.
     * @param default_value The default value to set.
     * @return Reference to the positional argument.
     * @attention Setting the default value disables the `required` attribute.
     */
    positional& default_value(const std::convertible_to<value_type> auto& default_value) noexcept {
        this->_default_value = std::make_any<value_type>(default_value);
        this->_required = false;
        return *this;
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
     * @return An argument descriptor object for the argument.
     */
    [[nodiscard]] detail::argument_descriptor desc(const bool verbose) const noexcept override {
        detail::argument_descriptor desc(this->_name.str(), this->_help_msg);

        if (not verbose)
            return desc;

        if (not this->_required)
            desc.add_param("required", "false");
        if (this->bypass_required_enabled())
            desc.add_param("bypass required", "true");
        if constexpr (detail::c_writable<value_type>) {
            if (not this->_choices.empty())
                desc.add_range_param("choices", this->_choices);
            if (this->_default_value.has_value())
                desc.add_param("default value", std::any_cast<value_type>(this->_default_value));
        }

        return desc;
    }

    /**
     * @brief Mark the positional argument as used.
     * @remark No logic is performed for positional arguments
     */
    bool mark_used() override {
        return false;
    }

    /// @return True if the positional argument is used, false otherwise.
    [[nodiscard]] bool is_used() const noexcept override {
        return this->_value.has_value();
    }

    /// @return 1 if a value has been parsed for the positional argument, 0 otherwise.
    [[nodiscard]] std::size_t count() const noexcept override {
        return static_cast<std::size_t>(this->has_parsed_values());
    }

    /**
     * @brief Set the value for the positional argument.
     * @param str_value The string representation of the value.
     * @return Reference to the positional argument.
     * @throws ap::parsing_failure
     */
    bool set_value(const std::string& str_value) override {
        if (this->_value.has_value())
            throw parsing_failure::value_already_set(this->_name);

        value_type value;
        if constexpr (detail::c_trivially_readable<value_type>) {
            value = value_type(str_value);
        }
        else {
            if (not (std::istringstream(str_value) >> value))
                throw parsing_failure::invalid_value(this->_name, str_value);
        }

        if (not detail::is_valid_choice(value, this->_choices))
            throw parsing_failure::invalid_choice(this->_name, str_value);

        const auto apply_visitor = action::detail::apply_visitor<value_type>{value};
        for (const auto& action : this->_value_actions)
            std::visit(apply_visitor, action);

        this->_value = value;
        return false;
    }

    /// @return True if the positional argument has a value, false otherwise.
    [[nodiscard]] bool has_value() const noexcept override {
        return this->has_parsed_values() or this->_default_value.has_value();
    }

    /// @return True if the positional argument has parsed values, false otherwise.
    [[nodiscard]] bool has_parsed_values() const noexcept override {
        return this->_value.has_value();
    }

    /// @return Ordering relationship of positional argument range.
    [[nodiscard]] std::weak_ordering nvalues_ordering() const noexcept override {
        if (not this->_required)
            return std::weak_ordering::equivalent;

        return this->has_value() ? std::weak_ordering::equivalent : std::weak_ordering::less;
    }

    /**
     * @brief Get the stored value of the positional argument.
     * @throws std::logic_error
     */
    [[nodiscard]] const std::any& value() const override {
        if (this->has_parsed_values())
            return this->_value;

        if (this->_default_value.has_value())
            return this->_default_value;

        throw std::logic_error(
            std::format("No value parsed for the `{}` positional argument.", this->_name.str())
        );
    }

    /**
     * @return Reference to the vector of parsed values for the positional argument.
     * @throws std::logic_error
     */
    [[nodiscard]] const std::vector<std::any>& values() const override {
        throw std::logic_error(
            std::format("Positional argument `{}` has only 1 value.", this->_name.str())
        );
    }

    std::any _default_value;
    std::vector<value_type> _choices;
    std::vector<value_action_type> _value_actions;

    std::any _value; ///< Stored value of the positional argument.
};

} // namespace ap::argument
