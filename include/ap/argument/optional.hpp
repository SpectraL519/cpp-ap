// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file optional.hpp

#pragma once

#include "ap/action/detail/utility.hpp"
#include "ap/action/predefined_actions.hpp"
#include "ap/detail/argument_base.hpp"
#include "ap/detail/argument_descriptor.hpp"
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
template <detail::c_argument_value_type T = std::string>
class optional : public detail::argument_base {
public:
    using value_type = T; ///< The argument's value type.
    using count_type = nargs::range::count_type; ///< The argument's value count type.

    optional() = delete;

    /**
     * @brief Constructor for optional argument with the `name` identifier.
     * @param name The `name` identifier of the optional argument.
     */
    optional(const detail::argument_name& name) : argument_base(name) {}

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
    optional& help(std::string_view help_msg) noexcept {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Mark the optional argument as required.
     * @return Reference to the optional argument.
     *
     * \todo Add a `const bool` parameter to enable explicit enabling/disabling of this option.
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
    optional& nargs(const nargs::range& range) noexcept {
        this->_nargs_range = range;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param n The exact bound for nargs range.
     * @return Reference to the optional argument.
     */
    optional& nargs(const count_type n) noexcept {
        this->_nargs_range = nargs::range(n);
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param lower_bound The lower bound for nargs range.
     * @param upper_bound The upper bound for nargs range.
     * @return Reference to the optional argument.
     */
    optional& nargs(const count_type lower_bound, const count_type upper_bound) noexcept {
        this->_nargs_range = nargs::range(lower_bound, upper_bound);
        return *this;
    }

    /**
     * @brief Set the action for the optional argument.
     * @tparam AS The action specifier type (see @ref ap/action/specifiers.hpp).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the optional argument.
     */
    template <action::detail::c_action_specifier AS, typename F>
    optional& action(F&& action) noexcept {
        if constexpr (action::detail::c_value_action_specifier<AS>) {
            using callable_type = action::detail::callable_type<AS, value_type>;
            this->_value_actions.emplace_back(std::forward<callable_type>(action));
        }
        else {
            this->_flag_actions.emplace_back(std::forward<flag_action_type>(action));
        }

        return *this;
    }

    /**
     * @brief Set the choices for the optional argument.
     * @tparam CR The choices range type.
     * @param choices The range of valid choices for the argument.
     * @return Reference to the optional argument.
     * @note `value_type` must be equality comparable.
     * @note `CR` must be a range such that its value type is convertible to `value_type`.
     */
    template <detail::c_range_of<value_type, detail::type_validator::convertible> CR>
    optional& choices(const CR& choices) noexcept
    requires(std::equality_comparable<value_type>)
    {
        for (const auto& choice : choices)
            this->_choices.emplace_back(choice);
        return *this;
    }

    /**
     * @brief Set the choices for the optional argument.
     * @param choices The list of valid choices for the argument.
     * @return Reference to the optional argument.
     * @note `value_type` must be equality comparable.
     */
    optional& choices(std::initializer_list<value_type> choices) noexcept
    requires(std::equality_comparable<value_type>)
    {
        return this->choices<>(choices);
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

    /// @brief Friend class declaration for access by argument_parser.
    friend class ::ap::argument_parser;

#ifdef AP_TESTING
    /**
     * @brief Friend struct declaration for testing purposes.
     */
    friend struct ::ap_testing::optional_argument_test_fixture;
#endif

private:
    using value_action_type =
        action::detail::value_action_variant_type<T>; ///< The argument's value action type.
    using flag_action_type = typename action_type::on_flag::type;

    /**
     * @param verbose The verbosity mode value.
     * @param flag_char The character used for the argument flag prefix.
     * @return An argument descriptor object for the argument.
     */
    [[nodiscard]] detail::argument_descriptor desc(const bool verbose, const char flag_char)
        const noexcept override {
        detail::argument_descriptor desc(this->_name.str(flag_char), this->_help_msg);

        if (not verbose)
            return desc;

        desc.params.reserve(6);
        if (this->_required)
            desc.add_param("required", "true");
        if (this->_bypass_required)
            desc.add_param("bypass required", "true");
        if (this->_nargs_range.is_bound())
            desc.add_param("nargs", this->_nargs_range);
        if constexpr (detail::c_writable<value_type>) {
            if (not this->_choices.empty())
                desc.add_range_param("choices", this->_choices);
            if (this->_default_value.has_value())
                desc.add_param("default value", std::any_cast<value_type>(this->_default_value));
            if (this->_implicit_value.has_value())
                desc.add_param("implicit value", std::any_cast<value_type>(this->_implicit_value));
        }

        return desc;
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
    bool mark_used() override {
        ++this->_count;
        for (const auto& action : this->_flag_actions)
            action();
        return this->_accepts_further_values();
    }

    /// @return True if the optional argument is used, false otherwise.
    [[nodiscard]] bool is_used() const noexcept override {
        return this->_count > 0;
    }

    /// @return The number of times the optional argument is used.
    [[nodiscard]] std::size_t count() const noexcept override {
        return this->_count;
    }

    /**
     * @brief Set the value for the optional argument.
     * @param str_value The string value to set.
     * @return Reference to the optional argument.
     * @throws ap::parsing_failure
     */
    bool set_value(const std::string& str_value) override {
        if (not this->_accepts_further_values())
            throw parsing_failure::invalid_nvalues(this->_name, std::weak_ordering::greater);

        value_type value;
        if (not (std::istringstream(str_value) >> value))
            throw parsing_failure::invalid_value(this->_name, str_value);

        if (not detail::is_valid_choice(value, this->_choices))
            throw parsing_failure::invalid_choice(this->_name, str_value);

        const auto apply_visitor = action::detail::apply_visitor<value_type>{value};
        for (const auto& action : this->_value_actions)
            std::visit(apply_visitor, action);

        this->_values.emplace_back(std::move(value));
        return this->_accepts_further_values();
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
    [[nodiscard]] std::weak_ordering nvalues_ordering() const noexcept override {
        if (this->_values.empty() and this->_has_predefined_value())
            return std::weak_ordering::equivalent;

        return this->_nargs_range.ordering(this->_values.size());
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

    /**
     * @return Reference to the predefined value of the optional argument.
     * @throws std::logic_error
     */
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

    [[nodiscard]] bool _accepts_further_values() const noexcept {
        return not std::is_gt(this->_nargs_range.ordering(this->_values.size() + 1ull));
    }

    bool _required = false;
    bool _bypass_required = false;
    nargs::range _nargs_range = nargs::any();
    std::any _default_value;
    std::any _implicit_value;
    std::vector<value_type> _choices;
    std::vector<flag_action_type> _flag_actions;
    std::vector<value_action_type> _value_actions;

    std::size_t _count = 0ull;
    std::vector<std::any> _values;
};

} // namespace ap::argument
