// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "action/detail/utility.hpp"
#include "action/predefined_actions.hpp"
#include "detail/argument_base.hpp"
#include "detail/argument_descriptor.hpp"
#include "detail/concepts.hpp"
#include "nargs/range.hpp"
#include "none_type.hpp"

#ifdef AP_TESTING

namespace ap_testing {
struct argument_test_fixture;
} // namespace ap_testing

#endif

namespace ap {

enum class argument_type : bool { positional, optional };

template <argument_type ArgT, detail::c_argument_value_type T = std::string>
class argument : public detail::argument_base {
public:
    using value_type = T;
    using count_type = nargs::count_type;

    static constexpr argument_type type = ArgT;

    argument() = delete;

    argument(const detail::argument_name& name)
    requires(type == argument_type::positional)
    : _name(name), _nargs_range(_default_nargs_range_actual), _required(_default_required) {}

    argument(const detail::argument_name& name)
    requires(type == argument_type::optional)
    : _name(name),
      _nargs_range(_default_nargs_range_actual),
      _required(_default_required),
      _count(0ull) {}

    /**
     * @brief Equality comparison operator for optional argument.
     * @param other The optional argument to compare with.
     * @return Equality of comparison.
     */
    bool operator==(const argument& other) const noexcept {
        return this->_name == other._name;
    }

    [[nodiscard]] bool is_positional() const noexcept override {
        return type == argument_type::positional;
    }

    [[nodiscard]] bool is_optional() const noexcept override {
        return type == argument_type::optional;
    }

    /// @return Reference the name of the positional argument.
    [[nodiscard]] const ap::detail::argument_name& name() const noexcept override {
        return this->_name;
    }

    /// @return Optional help message for the positional argument.
    [[nodiscard]] const std::optional<std::string>& help() const noexcept override {
        return this->_help_msg;
    }

    /// @return `true` if the argument is hidden, `false` otherwise
    [[nodiscard]] bool is_hidden() const noexcept override {
        return this->_hidden;
    }

    /// @return `true` if the argument is required, `false` otherwise
    [[nodiscard]] bool is_required() const noexcept override {
        return this->_required;
    }

    /**
     * @return `true` if required argument bypassing is enabled for the argument, `false` otherwise.
     * @note Required argument bypassing is enabled only when both `required` and `bypass_required` flags are set to `true`.
     */
    [[nodiscard]] bool is_bypass_required_enabled() const noexcept override {
        return not this->_required and this->_bypass_required;
    }

    // attribute setters

    /**
     * @brief Set the help message for the optional argument.
     * @param help_msg The help message to set.
     * @return Reference to the optional argument.
     */
    argument& help(std::string_view help_msg) noexcept {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Set the `hidden` attribute for the positional argument.
     * @param h The attribute value.
     * @return Reference to the positional argument.
     */
    argument& hidden(const bool h = true) noexcept {
        this->_hidden = h;
        return *this;
    }

    /**
     * @brief Set the `required` attribute of the optional argument
     * @param r The attribute value.
     * @return Reference to the optional argument.
     * @attention Setting the `required` attribute to true disables the `bypass_required` attribute.
     */
    argument& required(const bool r = true) noexcept {
        this->_required = r;
        if (this->_required)
            this->_bypass_required = false;
        return *this;
    }

    /**
     * @brief Enable/disable bypassing the `required` attribute for the optional argument.
     * @param br The attribute value.
     * @return Reference to the optional argument.
     * @attention Setting the `bypass_required` option to true disables the `required` attribute.
     */
    argument& bypass_required(const bool br = true) noexcept {
        this->_bypass_required = br;
        if (this->_bypass_required)
            this->_required = false;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param range The nargs range to set.
     * @return Reference to the optional argument.
     */
    argument& nargs(const nargs::range& range) noexcept
    requires(not detail::c_is_none<value_type>)
    {
        this->_nargs_range = range;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param n The exact bound for nargs range.
     * @return Reference to the optional argument.
     */
    argument& nargs(const count_type n) noexcept
    requires(not detail::c_is_none<value_type>)
    {
        this->_nargs_range = nargs::range(n);
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param lower The lower bound for nargs range.
     * @param upper The upper bound for nargs range.
     * @return Reference to the optional argument.
     */
    argument& nargs(const count_type lower, const count_type upper) noexcept
    requires(not detail::c_is_none<value_type>)
    {
        this->_nargs_range = nargs::range(lower, upper);
        return *this;
    }

    /**
     * @brief Set the action for the optional argument.
     * @tparam AS The action specifier type (see @ref ap/action/specifiers.hpp).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the optional argument.
     */
    template <action::detail::c_value_action_specifier AS, typename F>
    argument& action(F&& action) noexcept
    requires(not detail::c_is_none<value_type>)
    {
        using callable_type = action::detail::callable_type<AS, value_type>;
        this->_value_actions.emplace_back(std::forward<callable_type>(action));
        return *this;
    }

    /**
     * @brief Set the action for the optional argument.
     * @tparam AS The action specifier type (see @ref ap/action/specifiers.hpp).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the optional argument.
     */
    template <action::detail::c_flag_action_specifier AS, typename F>
    argument& action(F&& action) noexcept
    requires(type == argument_type::optional)
    {
        this->_flag_actions.emplace_back(std::forward<flag_action_type>(action));
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
    argument& choices(const CR& choices) noexcept
    requires(not detail::c_is_none<value_type> and std::equality_comparable<value_type>)
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
    argument& choices(std::initializer_list<value_type> choices) noexcept
    requires(not detail::c_is_none<value_type> and std::equality_comparable<value_type>)
    {
        return this->choices<>(choices);
    }

    /**
     * @brief Set the default value for the optional argument.
     * @param default_value The default value to set.
     * @return Reference to the optional argument.
     * @attention Setting the default value disables the `required` attribute.
     */
    argument& default_value(const std::convertible_to<value_type> auto& default_value) noexcept
    requires(not detail::c_is_none<value_type>)
    {
        this->_default_value = std::make_any<value_type>(default_value);
        this->_required = false;
        return *this;
    }

    /**
     * @brief Set the implicit value for the optional argument.
     * @param implicit_value The implicit value to set.
     * @return Reference to the optional argument.
     */
    argument& implicit_value(const std::convertible_to<value_type> auto& implicit_value) noexcept
    requires(not detail::c_is_none<value_type> and type == argument_type::optional)
    {
        this->_implicit_value = std::make_any<value_type>(implicit_value);
        return *this;
    }

#ifdef AP_TESTING
    friend struct ::ap_testing::argument_test_fixture;
#endif

private:
    using value_action_type =
        action::detail::value_action_variant_type<T>; ///< The argument's value action type.
    using flag_action_type = typename action_type::on_flag::type;

    template <typename _T>
    using value_arg_specific_type =
        std::conditional_t<detail::c_is_none<value_type>, none_type, _T>;

    template <typename _T>
    using positional_specific_type =
        std::conditional_t<type == argument_type::positional, _T, none_type>;

    template <typename _T>
    using optional_specific_type =
        std::conditional_t<type == argument_type::optional, _T, none_type>;

    /**
     * @param verbose The verbosity mode value.
     * @return A descriptor object for the argument.
     */
    [[nodiscard]] detail::argument_descriptor desc(const bool verbose) const noexcept override {
        detail::argument_descriptor desc(this->_name.str(), this->_help_msg);

        if (not verbose)
            return desc;

        desc.params.reserve(6ull);
        if (this->_required != _default_required)
            desc.add_param("required", std::format("{}", this->_required));
        if (this->is_bypass_required_enabled())
            desc.add_param("bypass required", "true");
        if (this->_nargs_range != _default_nargs_range)
            desc.add_param("nargs", this->_nargs_range);
        if constexpr (detail::c_writable<value_type>) {
            if (not this->_choices.empty())
                desc.add_range_param("choices", this->_choices);
            if (this->_default_value.has_value())
                desc.add_param("default value", std::any_cast<value_type>(this->_default_value));
            if constexpr (type == argument_type::optional) {
                if (this->_implicit_value.has_value())
                    desc.add_param(
                        "implicit value", std::any_cast<value_type>(this->_implicit_value)
                    );
            }
        }

        return desc;
    }

    /// @brief Mark the optional argument as used.
    bool mark_used() override {
        if constexpr (type == argument_type::optional) {
            ++this->_count;
            for (const auto& action : this->_flag_actions)
                action();
        }

        return this->_accepts_further_values();
    }

    /// @return True if the optional argument is used, false otherwise.
    [[nodiscard]] bool is_used() const noexcept override {
        return this->count() > 0ull;
    }

    /// @return The number of times the optional argument attribute has been used.
    [[nodiscard]] std::size_t count() const noexcept override {
        if constexpr (type == argument_type::optional)
            return this->_count;
        else
            return static_cast<std::size_t>(this->has_parsed_values());
    }

    /**
     * @brief Set the value for the optional argument.
     * @param str_value The string value to set.
     * @return Reference to the optional argument.
     * @throws ap::parsing_failure
     */
    bool set_value(const std::string& str_value) override {
        return this->_set_value_impl(str_value);
    }

    /// @return True if the optional argument has a value, false otherwise.
    [[nodiscard]] bool has_value() const noexcept override {
        return this->has_parsed_values() or this->_has_predefined_values();
    }

    /// @return True if parsed values are available for the optional argument, false otherwise.
    [[nodiscard]] bool has_parsed_values() const noexcept override {
        return not this->_values.empty();
    }

    /// @return ordering relationship of optional argument range.
    [[nodiscard]] std::weak_ordering nvalues_ordering() const noexcept override {
        if (this->_values.empty() and this->_has_predefined_values())
            return std::weak_ordering::equivalent;

        return this->_values.size() <=> this->_nargs_range;
    }

    /// @return Reference to the stored value of the optional argument.
    [[nodiscard]] const std::any& value() const override {
        if (not this->_values.empty())
            return this->_values.front();

        if constexpr (detail::c_is_none<value_type>)
            throw std::logic_error(
                std::format("No values parsed for argument '{}'.", this->_name.str())
            );
        else
            return this->_predefined_value();
    }

    /// @return Reference to the vector of parsed values for the optional argument.
    [[nodiscard]] const std::vector<std::any>& values() const override {
        return this->_values;
    }

    /// @return True if the optional argument has a predefined value, false otherwise.
    [[nodiscard]] bool _has_predefined_values() const noexcept
    requires(detail::c_is_none<value_type>)
    {
        return false;
    }

    /// @return True if the optional argument has a predefined value, false otherwise.
    [[nodiscard]] bool _has_predefined_values() const noexcept
    requires(not detail::c_is_none<value_type> and type == argument_type::positional)
    {
        return this->_default_value.has_value();
    }

    /// @return True if the optional argument has a predefined value, false otherwise.
    [[nodiscard]] bool _has_predefined_values() const noexcept
    requires(not detail::c_is_none<value_type> and type == argument_type::optional)
    {
        return this->_default_value.has_value()
            or (this->is_used() and this->_implicit_value.has_value());
    }

    /**
     * @return Reference to the predefined value of the optional argument.
     * @throws std::logic_error
     */
    [[nodiscard]] const std::any& _predefined_value() const
    requires(not detail::c_is_none<value_type>)
    {
        if constexpr (type == argument_type::optional) {
            if (this->is_used()) {
                if (not this->_implicit_value.has_value())
                    throw(std::logic_error(std::format(
                        "No implicit value specified for argument '{}'.", this->_name.str()
                    )));

                return this->_implicit_value;
            }
        }

        if (not this->_default_value.has_value())
            throw(std::logic_error(
                std::format("No default value specified for argument '{}'.", this->_name.str())
            ));

        return this->_default_value;
    }

    [[nodiscard]] bool _accepts_further_values() const noexcept {
        return not std::is_gt(this->_values.size() + 1ull <=> this->_nargs_range);
    }

    /// @todo Use std::ranges::contains after the switch to C++23
    [[nodiscard]] bool _is_valid_choice(const value_type& value) const noexcept
    requires(not detail::c_is_none<value_type>)
    {
        return this->_choices.empty()
            or std::ranges::find(this->_choices, value) != this->_choices.end();
    }

    /**
     * @brief Set the value for the optional argument.
     * @param str_value The string value to set.
     * @throws ap::parsing_failure
     * @attention Always throws! (`set_value` should never be called for a none-type argument).
     */
    bool _set_value_impl(const std::string& str_value)
    requires(detail::c_is_none<value_type>)
    {
        throw parsing_failure(std::format(
            "Cannot set values for a none-type argument '{}' (value: '{}')",
            this->_name.str(),
            str_value
        ));
    }

    /**
     * @brief Set the value for the optional argument.
     * @param str_value The string value to set.
     * @throws ap::parsing_failure
     */
    bool _set_value_impl(const std::string& str_value)
    requires(not detail::c_is_none<value_type>)
    {
        if (not this->_accepts_further_values())
            throw parsing_failure::invalid_nvalues(this->_name, std::weak_ordering::greater);

        value_type value;
        if constexpr (detail::c_trivially_readable<value_type>) {
            value = value_type(str_value);
        }
        else {
            if (not (std::istringstream(str_value) >> value))
                throw parsing_failure::invalid_value(this->_name, str_value);
        }

        if (not this->_is_valid_choice(value))
            throw parsing_failure::invalid_choice(this->_name, str_value);

        const auto apply_visitor = action::detail::apply_visitor<value_type>{value};
        for (const auto& action : this->_value_actions)
            std::visit(apply_visitor, action);

        this->_values.emplace_back(std::move(value));
        return this->_accepts_further_values();
    }

    // TODO: validate whether the members could be reordered to use less memory

    // attributes
    const ap::detail::argument_name _name;
    std::optional<std::string> _help_msg;
    nargs::range _nargs_range;
    [[no_unique_address]] value_arg_specific_type<std::any> _default_value;
    [[no_unique_address]] value_arg_specific_type<optional_specific_type<std::any>> _implicit_value;
    [[no_unique_address]] value_arg_specific_type<std::vector<value_type>> _choices;
    [[no_unique_address]] optional_specific_type<std::vector<flag_action_type>> _flag_actions;
    [[no_unique_address]] value_arg_specific_type<std::vector<value_action_type>> _value_actions;

    bool _required : 1;
    bool _bypass_required : 1 = false;
    bool _hidden : 1 = false;

    // parsing result
    [[no_unique_address]] optional_specific_type<std::size_t> _count;
    std::vector<std::any> _values;

    // default attribute values
    static constexpr bool _default_required = (type == argument_type::positional);
    static constexpr nargs::range _default_nargs_range =
        (type == argument_type::positional) ? nargs::range(1ull) : nargs::any();
    static constexpr nargs::range _default_nargs_range_actual =
        detail::c_is_none<value_type> ? nargs::range(0ull) : _default_nargs_range;
};

template <detail::c_argument_value_type T = std::string>
using positional_argument = argument<argument_type::positional, T>;

template <detail::c_argument_value_type T = std::string>
using optional_argument = argument<argument_type::optional, T>;

enum class default_argument {
    p_input,
    p_output,
    o_help,
    o_input,
    o_output,
    o_multi_input,
    o_multi_output
};

} // namespace ap
