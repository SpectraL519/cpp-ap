// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ap/action/detail/utility.hpp"
#include "ap/action/predefined_actions.hpp"
#include "ap/detail/argument_interface.hpp"
#include "ap/detail/concepts.hpp"
#include "ap/nargs/range.hpp"

#ifdef AP_TESTING

namespace ap_testing {
struct optional_argument_test_fixture;
} // namespace ap_testing

#endif

namespace ap::argument {

/**
 * @brief The optioanl argument class.
 * @tparam T The argument's value type.
 */
template <ap::detail::c_argument_value_type T = std::string>
class optional : public ap::detail::argument_interface {
public:
    using value_type = T; ///< The argument's value type.
    using count_type = ap::nargs::range::count_type; ///< The argument's value count type.
    using action_type = ap::action::detail::action_variant_type<T>; ///< The argument's action type.

    optional() = delete;

    /**
     * @brief Constructor for optional argument with the `name` identifier.
     * @param name The `name` identifier of the optional argument.
     */
    optional(const ap::detail::argument_name& name) : _name(name) {}

    ~optional() = default;

    /**
     * @brief Equality comparison operator for optional argument.
     * @param other The optional argument to compare with.
     * @return Equality of comparison.
     */
    bool operator==(const optional& other) const noexcept {
        return this->_name == other._name;
    }

    /**
     * @brief Set the help message for the optional argument.
     * @param help_msg The help message to set.
     * @return Reference to the optional argument.
     */
    optional& help(std::string_view help_msg) noexcept override {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Mark the optional argument as required.
     * @return Reference to the optional argument.
     */
    optional& required() noexcept {
        this->_required = true;
        return *this;
    }

    /**
     * @brief Enable bypassing the required status for the optional argument.
     * @return Reference to the optional argument.
     */
    optional& bypass_required() noexcept {
        this->_bypass_required = true;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param range The nargs range to set.
     * @return Reference to the optional argument.
     */
    optional& nargs(const ap::nargs::range& range) noexcept {
        this->_nargs_range = range;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param count The count for nargs range.
     * @return Reference to the optional argument.
     */
    optional& nargs(const count_type count) noexcept {
        this->_nargs_range = ap::nargs::range(count);
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param nlow The lower bound for nargs range.
     * @param nhigh The upper bound for nargs range.
     * @return Reference to the optional argument.
     */
    optional& nargs(const count_type nlow, const count_type nhigh) noexcept {
        this->_nargs_range = ap::nargs::range(nlow, nhigh);
        return *this;
    }

    /**
     * @brief Set the action for the optional argument.
     * @tparam AS The action specifier type (valued_action or void_action).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the optional argument.
     */
    template <ap::action::detail::c_action_specifier AS, std::invocable<value_type&> F>
    optional& action(F&& action) noexcept {
        using callable_type = ap::action::detail::callable_type<AS, value_type>;
        this->_action = std::forward<callable_type>(action);
        return *this;
    }

    /**
     * @brief Set the choices for the optional argument.
     * @param choices The vector of valid choices for the argument.
     * @return Reference to the optional argument.
     * @note Requires T to be equality comparable.
     */
    optional& choices(const std::vector<value_type>& choices) noexcept
    requires(std::equality_comparable<value_type>)
    {
        this->_choices = choices;
        return *this;
    }

    /**
     * @brief Set the default value for the optional argument.
     * @param default_value The default value to set.
     * @return Reference to the optional argument.
     */
    optional& default_value(const value_type& default_value) noexcept {
        this->_default_value = default_value;
        return *this;
    }

    /**
     * @brief Set the implicit value for the optional argument.
     * @param implicit_value The implicit value to set.
     * @return Reference to the optional argument.
     */
    optional& implicit_value(const value_type& implicit_value) noexcept {
        this->_implicit_value = implicit_value;
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
    friend struct ::ap_testing::optional_argument_test_fixture;
#endif

private:
    /// @return Reference to the name of the optional argument.
    [[nodiscard]] const ap::detail::argument_name& name() const noexcept override {
        return this->_name;
    }

    /// @return Reference to the optional help message for the optional argument.
    [[nodiscard]] const std::optional<std::string>& help() const noexcept override {
        return this->_help_msg;
    }

    /// @return True if the optional argument is required, false otherwise.
    [[nodiscard]] bool is_required() const noexcept override {
        return this->_required;
    }

    /// @return True if bypassing the required status is enabled for the optional argument, false otherwise.
    [[nodiscard]] bool bypass_required_enabled() const noexcept override {
        return this->_bypass_required;
    }

    /// @brief Mark the optional argument as used.
    void mark_used() noexcept override {
        ++this->_nused;
    }

    /// @return True if the optional argument is used, false otherwise.
    [[nodiscard]] bool is_used() const noexcept override {
        return this->_nused > 0;
    }

    /// @return The number of times the optional argument is used.
    [[nodiscard]] std::size_t nused() const noexcept override {
        return this->_nused;
    }

    /**
     * @brief Set the value for the optional argument.
     * @param str_value The string value to set.
     * @return Reference to the optional argument.
     */
    optional& set_value(const std::string& str_value) override {
        this->_ss.clear();
        this->_ss.str(str_value);

        value_type value;
        if (not (this->_ss >> value))
            throw error::invalid_value(this->_name, this->_ss.str());

        if (not this->_is_valid_choice(value))
            throw error::invalid_choice(this->_name, str_value);

        this->_apply_action(value);

        if (not (this->_nargs_range or this->_values.empty()))
            throw error::value_already_set(this->_name);

        this->_values.emplace_back(std::move(value));
        return *this;
    }

    /// @return True if the optional argument has a value, false otherwise.
    [[nodiscard]] bool has_value() const noexcept override {
        return this->has_parsed_values() or this->_has_predefined_value();
    }

    /// @return True if parsed values are available for the optional argument, false otherwise.
    [[nodiscard]] bool has_parsed_values() const noexcept override {
        return not this->_values.empty();
    }

    /// @return ordering relationship of optional argument range.
    [[nodiscard]] std::weak_ordering nvalues_in_range() const noexcept override {
        if (not this->_nargs_range)
            return std::weak_ordering::equivalent;

        if (this->_values.empty() and this->_has_predefined_value())
            return std::weak_ordering::equivalent;

        return this->_nargs_range->contains(this->_values.size());
    }

    /// @return Reference to the stored value of the optional argument.
    [[nodiscard]] const std::any& value() const override {
        return this->_values.empty() ? this->_predefined_value() : this->_values.front();
    }

    /// @return Reference to the vector of parsed values for the optional argument.
    [[nodiscard]] const std::vector<std::any>& values() const override {
        return this->_values;
    }

    /// @return True if the optional argument has a predefined value, false otherwise.
    [[nodiscard]] bool _has_predefined_value() const noexcept {
        return this->_default_value.has_value()
            or (this->is_used() and this->_implicit_value.has_value());
    }

    /// @return Reference to the predefined value of the optional argument.
    [[nodiscard]] const std::any& _predefined_value() const {
        if (this->is_used()) {
            if (not this->_implicit_value.has_value())
                throw(std::logic_error(
                    std::format("No implicit value specified for argument `{}`.", this->_name.str())
                ));

            return this->_implicit_value;
        }

        if (not this->_default_value.has_value())
            throw(std::logic_error(
                std::format("No default value specified for argument `{}`.", this->_name.str())
            ));

        return this->_default_value;
    }

    /**
     * @brief Check if the provided choice is valid for the optional argument.
     * @param choice The value to check against choices.
     * @return True if choice is valid, false otherwise.
     */
    [[nodiscard]] bool _is_valid_choice(const value_type& choice) const noexcept {
        return this->_choices.empty()
            or std::ranges::find(this->_choices, choice) != this->_choices.end();
    }

    /**
     * @brief Apply the specified action to the value of the optional argument.
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

    static constexpr bool _optional = true;
    const ap::detail::argument_name _name;
    std::optional<std::string> _help_msg;

    bool _required = false;
    bool _bypass_required = false;
    std::optional<ap::nargs::range> _nargs_range;
    action_type _action =
        ap::action::none<value_type>(); ///< Action associated with the opitonal argument.
    std::vector<value_type> _choices; ///< Vector of valid choices for the optional argument.
    std::any _default_value;
    std::any _implicit_value;

    std::size_t _nused = 0u; ///< Number of used optional arguments.
    std::vector<std::any> _values; ///< Vector holding parsed values for the optional argument.

    std::stringstream _ss; ///< Stringstream used for parsing values.
};

} // namespace ap::argument
