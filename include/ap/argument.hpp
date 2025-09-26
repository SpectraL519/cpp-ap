// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file ap/argument.hpp

#pragma once

#include "action/predefined.hpp"
#include "action/util/helpers.hpp"
#include "detail/argument_base.hpp"
#include "detail/help_builder.hpp"
#include "nargs/range.hpp"
#include "types.hpp"
#include "util/concepts.hpp"
#include "util/ranges.hpp"

#ifdef AP_TESTING

namespace ap_testing {
struct argument_test_fixture;
} // namespace ap_testing

#endif

namespace ap {

/// @brief A discriminator type used to specify the type of an argument within the @ref ap::argument class.
enum class argument_type : bool { positional, optional };

/**
 * @brief Represents a command-line argument, either positional or optional.
 *
 * This class defines the behaviour of command-line arguments - both positional and optional,
 * depending on the given type discriminator.
 *
 * @note This class is not intended to be constructed directly, but rather throught the
 * @note - `add_positional_argument`
 * @note - `add_optional_argument`
 * @note - `add_flag`
 * @note methods of @ref ap::argument_parser.
 * @attention Some member functions are conditionally enabled/disabled depending on the argument type and value type.
 *
 * Example usage:
 * @code{.cpp}
 * ap::argument_parser parser;
 * parser.add_positional_argument("input", "i")
 *       .help("An input file path");
 * parser.add_optional_argument("output", "o")
 *       .default_values("out.txt")
 *       .help("An output file path");
 * @endcode
 *
 * @tparam ArgT The argument type, either @ref ap::argument_type::positional or @ref ap::argument_type::optional.
 * @tparam T The value type accepted by the argument (defaults to std::string).
 */
template <argument_type ArgT, util::c_argument_value_type T = std::string>
class argument : public detail::argument_base {
public:
    using value_type = T; ///< The argument's value type alias.
    using count_type = nargs::count_type; ///< The argument's count type alias.

    static constexpr argument_type type = ArgT; ///< The argument's type discriminator.

    argument() = delete;

    /**
     * @brief Positional argument constructor.
     * @param name The name of the positional argument.
     * @note The constructor is enabled only if `type` is `argument_type::positional`.
     */
    argument(const detail::argument_name& name)
    requires(type == argument_type::positional)
    : _name(name), _nargs_range(_default_nargs_range_actual), _required(_default_required) {}

    /**
     * @brief Optional argument constructor.
     * @param name The name of the optional argument.
     * @note The constructor is enabled only if `type` is `argument_type::optional`.
     */
    argument(const detail::argument_name& name)
    requires(type == argument_type::optional)
    : _name(name),
      _nargs_range(_default_nargs_range_actual),
      _required(_default_required),
      _count(0ull) {}

    /// @brief Checks if the argument is positional.
    /// @return `true` if the argument's `type` is `argument_type::positional`, `false` otherwise.
    [[nodiscard]] bool is_positional() const noexcept override {
        return type == argument_type::positional;
    }

    /// @brief Checks if the argument is optional.
    /// @return `true` if the argument's `type` is `argument_type::optional`, `false` otherwise.
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

    /// @return `true` if argument checks suppressing is enabled for the argument, `false` otherwise.
    [[nodiscard]] bool suppresses_arg_checks() const noexcept override {
        return this->_suppress_arg_checks;
    }

    /// @return `true` if argument group checks suppressing is enabled for the argument, `false` otherwise.
    [[nodiscard]] bool suppresses_group_checks() const noexcept override {
        return this->_suppress_group_checks;
    }

    /// @return `true` if the argument is greedy, `false` otherwise.
    [[nodiscard]] bool is_greedy() const noexcept override {
        return this->_greedy;
    }

    // attribute setters

    /**
     * @brief Set the help message for the argument.
     * @param help_msg The help message to set.
     * @return Reference to the argument instance.
     */
    argument& help(std::string_view help_msg) noexcept {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Set the `hidden` attribute for the argument.
     * @param h The attribute value.
     * @return Reference to the argument instance.
     */
    argument& hidden(const bool h = true) noexcept {
        this->_hidden = h;
        return *this;
    }

    /**
     * @brief Set the `required` attribute of the argument
     * @param value The attribute value (default: `true`).
     * @return Reference to the argument instance.
     */
    argument& required(const bool value = true) {
        if (value and (this->_suppress_arg_checks or this->_suppress_group_checks))
            throw invalid_configuration(
                std::format("A suppressing argument [{}] cannot be required!", this->_name.str())
            );

        this->_required = value;
        return *this;
    }

    /**
     * @brief Enable/disable suppressing argument checks for other arguments.
     * @param value The attribute value (default: `true`).
     * @return Reference to the argument instance.
     */
    argument& suppress_arg_checks(const bool value = true) {
        if (value and this->_required)
            throw invalid_configuration(std::format(
                "A required argument [{}] cannot suppress argument checks!", this->_name.str()
            ));

        this->_suppress_arg_checks = value;
        return *this;
    }

    /**
     * @brief Enable/disable suppressing argument group checks.
     * @param value The attribute value (default: `true`).
     * @return Reference to the argument instance.
     */
    argument& suppress_group_checks(const bool value = true) {
        if (value and this->_required)
            throw invalid_configuration(std::format(
                "A required argument [{}] cannot suppress argument group checks!", this->_name.str()
            ));

        this->_suppress_group_checks = value;
        return *this;
    }

    /**
     * @brief Set the `greedy` attribute of the argument.
     * @param value The attribute value (default: `true`).
     * @return Reference to the argument instance.
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    argument& greedy(const bool value = true) noexcept
    requires(not util::c_is_none<value_type>)
    {
        this->_greedy = value;
        return *this;
    }

    /**
     * @brief Set the nargs range for the argument.
     * @param range The attribute value.
     * @return Reference to the argument instance.
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    argument& nargs(const nargs::range& range) noexcept
    requires(not util::c_is_none<value_type>)
    {
        this->_nargs_range = range;
        return *this;
    }

    /**
     * @brief Set the nargs range for the argument.
     * @param n The exact bound for the nargs range attribute.
     * @return Reference to the argument instance.
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    argument& nargs(const count_type n) noexcept
    requires(not util::c_is_none<value_type>)
    {
        return this->nargs(nargs::range(n));
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param lower The lower bound for the nargs range attribute.
     * @param upper The upper bound for the nargs range attribute.
     * @return Reference to the argument instance.
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    argument& nargs(const count_type lower, const count_type upper) noexcept
    requires(not util::c_is_none<value_type>)
    {
        return this->nargs(nargs::range(lower, upper));
    }

    /**
     * @brief Set the *value* action for the argument.
     * @tparam AS The action specifier type (see @ref ap/action/type.hpp).
     * @tparam F The type of the action function.
     * @param action The action callable.
     * @return Reference to the argument instance.
     * @note The method is enabled only if:
     * @note - `value_type` is not `none_type`.
     * @note - `AS` is a valid value action specifier: `action_type::observe`, `action_type::transform`, `action_type::modify`.
     */
    template <action::util::c_value_action_specifier AS, typename F>
    argument& action(F&& action) noexcept
    requires(not util::c_is_none<value_type>)
    {
        using callable_type = action::util::callable_type<AS, value_type>;
        this->_value_actions.emplace_back(std::forward<callable_type>(action));
        return *this;
    }

    /**
     * @brief Set the *on-flag* action for the argument.
     * @tparam AS The action specifier type (see @ref ap/action/types.hpp).
     * @tparam F The type of the action function.
     * @param action The action callable.
     * @return Reference to the argument instance.
     * @note The method is enabled only for optional arguments and if `AS` is `action_type::on_flag`.
     */
    template <action::util::c_flag_action_specifier AS, typename F>
    argument& action(F&& action) noexcept
    requires(type == argument_type::optional)
    {
        this->_flag_actions.emplace_back(std::forward<flag_action_type>(action));
        return *this;
    }

    /**
     * @brief Add the choices for the argument.
     * @tparam CR The choices range type.
     * @param choices The range of valid choices for the argument.
     * @return Reference to the argument instance.
     * @note The method is enabled only if:
     * @note - `value_type` must not be `none_type` and must be equality comparable
     * @note - `CR` must be a range such that its value type is convertible to the argument's `value_type`
     */
    template <util::c_range_of<value_type, util::type_validator::convertible> CR>
    argument& choices(const CR& choices) noexcept
    requires(not util::c_is_none<value_type> and std::equality_comparable<value_type>)
    {
        for (const auto& choice : choices)
            this->_choices.emplace_back(choice);
        return *this;
    }

    /**
     * @brief Add the choices for the argument.
     * @param choices The list of valid choices for the argument.
     * @return Reference to the argument instance.
     * @note The method is enabled only if `value_type` is not `none_type` and is equality comparable.
     */
    argument& choices(std::initializer_list<value_type> choices) noexcept
    requires(not util::c_is_none<value_type> and std::equality_comparable<value_type>)
    {
        return this->choices<>(choices);
    }

    /**
     * @brief Add the choices for the argument.
     * @tparam Args The types of the choices.
     * @param choices The list of valid choices for the argument.
     * @return Reference to the argument instance.
     * @note The method is enabled only if `value_type` is not `none_type` and is equality comparable.
     */
    argument& choices(const std::convertible_to<value_type> auto&... choices) noexcept
    requires(not util::c_is_none<value_type> and std::equality_comparable<value_type>)
    {
        (this->_choices.emplace_back(choices), ...);
        return *this;
    }

    /**
     * @brief Add default values for the argument.
     * @param values The default values to add.
     * @return Reference to the argument instance.
     * @attention Setting the default values resets the `required` attribute to `false`.
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    template <util::c_range_of<value_type, util::type_validator::convertible> CR>
    argument& default_values(const CR& values) noexcept
    requires(not util::c_is_none<value_type> and std::equality_comparable<value_type>)
    {
        for (const auto& value : values)
            this->_default_values.emplace_back(std::make_any<value_type>(value));
        this->_required = false;
        return *this;
    }

    /**
     * @brief Add default values for the argument.
     * @param values The default values to add.
     * @return Reference to the argument instance.
     * @attention Setting the default values resets the `required` attribute to `false`.
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    argument& default_values(std::initializer_list<value_type> values) noexcept
    requires(not util::c_is_none<value_type> and std::equality_comparable<value_type>)
    {
        return this->default_values<>(values);
    }

    /**
     * @brief Add default values for the argument.
     * @param values The default values to add.
     * @return Reference to the argument instance.
     * @attention Setting the default values resets the `required` attribute to `false`.
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    argument& default_values(const std::convertible_to<value_type> auto&... values) noexcept
    requires(not util::c_is_none<value_type>)
    {
        (this->_default_values.emplace_back(std::make_any<value_type>(values)), ...);
        this->_required = false;
        return *this;
    }

    /**
     * @brief Add implicit values for the optional argument.
     * @tparam CR The choices range type.
     * @param values The range of implicit values to set.
     * @return Reference to the optional argument instance.
     * @note The method is enabled only for optional arguments and if `value_type` is not `none_type`.
     */
    template <util::c_range_of<value_type, util::type_validator::convertible> CR>
    argument& implicit_values(const CR& values) noexcept
    requires(not util::c_is_none<value_type> and type == argument_type::optional)
    {
        for (const auto& value : values)
            this->_implicit_values.emplace_back(std::make_any<value_type>(value));
        return *this;
    }

    /**
     * @brief Add implicit values for the optional argument.
     * @param values The initializer list of implicit values to set.
     * @return Reference to the optional argument instance.
     * @note The method is enabled only for optional arguments and if `value_type` is not `none_type`.
     */
    argument& implicit_values(std::initializer_list<value_type> values) noexcept
    requires(not util::c_is_none<value_type> and type == argument_type::optional)
    {
        return this->implicit_values<>(values);
    }

    /**
     * @brief Add a implicit values for the optional argument.
     * @param values The implicit values to set.
     * @return Reference to the optional argument instance.
     * @note The method is enabled only for optional arguments and if `value_type` is not `none_type`.
     */
    argument& implicit_values(const std::convertible_to<value_type> auto&... values) noexcept
    requires(not util::c_is_none<value_type> and type == argument_type::optional)
    {
        (this->_implicit_values.emplace_back(std::make_any<value_type>(values)), ...);
        return *this;
    }

#ifdef AP_TESTING
    friend struct ::ap_testing::argument_test_fixture;
#endif

private:
    /// @brief The argument's value action type alias.
    using value_action_type = action::util::value_action_variant_type<T>;

    /// @brief The argument's flag action type alias.
    using flag_action_type = typename action_type::on_flag::type;

    /// @brief The argument's value-argument-specific type alias.
    /// @tparam _T The actual type used if the argument's `value_type` is not `none_type`.
    template <typename _T>
    using value_arg_specific_type = std::conditional_t<
        util::c_is_none<value_type>,
        none_type,
        _T>; ///< Type alias for value-argument-specific types.

    /// @brief The argument's positional-argument-specific type alias.
    /// @tparam _T The actual type used if the argument's `type` is `argument_type::positional`.
    template <typename _T>
    using positional_specific_type =
        std::conditional_t<type == argument_type::positional, _T, none_type>;

    /// @brief The argument's optional-argument-specific type alias.
    /// @tparam _T The actual type used if the argument's `type` is `argument_type::optional`.
    template <typename _T>
    using optional_specific_type =
        std::conditional_t<type == argument_type::optional, _T, none_type>;

    /**
     * @brief Creates a help message builder object for the argument.
     * @param verbose The verbosity mode value.
     * @note If the `verbose` parameter is set to `true` all non-default parameters will be included in the output,
     * @note otherwise only the argument's name and help message will be included.
     */
    [[nodiscard]] detail::help_builder help_builder(const bool verbose) const noexcept override {
        detail::help_builder bld(this->_name.str(), this->_help_msg);

        if (not verbose)
            return bld;

        bld.params.reserve(6ull);
        if (this->_required != _default_required)
            bld.add_param("required", std::format("{}", this->_required));
        if (this->_suppress_arg_checks)
            bld.add_param("suppress arg checks", "true");
        if (this->_suppress_group_checks)
            bld.add_param("suppress group checks", "true");
        if (this->_nargs_range != _default_nargs_range)
            bld.add_param("nargs", this->_nargs_range);
        if constexpr (util::c_writable<value_type>) {
            if (not this->_choices.empty())
                bld.add_range_param("choices", this->_choices);
            if (not this->_default_values.empty())
                bld.add_range_param(
                    "default value(s)", util::any_range_cast_view<value_type>(this->_default_values)
                );
            if constexpr (type == argument_type::optional) {
                if (not this->_implicit_values.empty())
                    bld.add_range_param(
                        "implicit value(s)",
                        util::any_range_cast_view<value_type>(this->_implicit_values)
                    );
            }
        }

        return bld;
    }

    /// @brief Mark the optional argument as used.
    /// @return `true` if the argument accepts further values, `false` otherwise.
    bool mark_used() override {
        if constexpr (type == argument_type::optional) {
            ++this->_count;
            for (const auto& action : this->_flag_actions)
                action();
        }

        return this->_accepts_further_values();
    }

    /// @return `true` if the argument is used, `false` otherwise.
    [[nodiscard]] bool is_used() const noexcept override {
        return this->count() > 0ull;
    }

    /**
     * @return The number of times the argument has been used.
     * @note - For positional arguments, the count is either `0` (not used) or `1` (used).
     * @note - For optional arguments, the count reflects the number of times the argument's flag has been used.
     */
    [[nodiscard]] std::size_t count() const noexcept override {
        if constexpr (type == argument_type::optional)
            return this->_count;
        else
            return static_cast<std::size_t>(this->has_parsed_values());
    }

    /**
     * @brief Set the value for the optional argument.
     * @param str_value The string value to use.
     * @return `true` if the argument accepts further values, `false` otherwise.
     * @throws ap::parsing_failure
     */
    bool set_value(const std::string& str_value) override {
        return this->_set_value_impl(str_value);
    }

    /// @return `true` if the argument has a value, `false` otherwise.
    /// @note An argument is considered to have a value if it has parsed values or predefined values (default/implicit).
    [[nodiscard]] bool has_value() const noexcept override {
        return this->has_parsed_values() or this->_has_predefined_values_impl();
    }

    /// @return `true` if parsed values are available for the argument, `false` otherwise.
    [[nodiscard]] bool has_parsed_values() const noexcept override {
        return not this->_values.empty();
    }

    /// @return `true` if the argument has predefined values, `false` otherwise.
    [[nodiscard]] bool has_predefined_values() const noexcept override {
        return this->_has_predefined_values_impl();
    }

    /// @return The ordering relationship of the argument's values and its nargs range attribute.
    [[nodiscard]] std::weak_ordering nvalues_ordering() const noexcept override {
        if (this->_values.empty() and this->_has_predefined_values_impl())
            return std::weak_ordering::equivalent;

        return this->_values.size() <=> this->_nargs_range;
    }

    /**
     * @return Reference to the stored value of the argument.
     * @note If multiple values are available, the first one is returned.
     * @throws std::logic_error if no values are available.
     */
    [[nodiscard]] const std::any& value() const override {
        if (this->has_parsed_values())
            return this->_values.front();

        if constexpr (util::c_is_none<value_type>)
            throw std::logic_error(
                std::format("No values parsed for argument '{}'.", this->_name.str())
            );
        else
            return this->_predefined_values().front();
    }

    /// @return Reference to the vector of parsed values for the argument.
    [[nodiscard]] const std::vector<std::any>& values() const override {
        return this->_values_impl();
    }

    [[nodiscard]] const std::vector<std::any>& _values_impl() const noexcept
    requires(util::c_is_none<value_type>)
    {
        return this->_values;
    }

    [[nodiscard]] const std::vector<std::any>& _values_impl() const noexcept
    requires(not util::c_is_none<value_type>)
    {
        if (this->has_parsed_values())
            return this->_values;

        try {
            return this->_predefined_values();
        }
        catch (const std::logic_error&) {
            return this->_values; // fallback: empty vector
        }
    }

    /// @return `true` if the argument has a predefined value, `false` otherwise.
    [[nodiscard]] bool _has_predefined_values_impl() const noexcept
    requires(util::c_is_none<value_type>)
    {
        return false;
    }

    /**
     * @return `true` if the argument has a predefined value, `false` otherwise.
     * @note The method is enabled only if `value_type` is not `none_type`.
     * @note - For positional arguments, a predefined value exists if a default value is set.
     * @note - For optional arguments, a predefined value exists if either a default value is set or if the argument has been used and an implicit value is set.
     */
    [[nodiscard]] bool _has_predefined_values_impl() const noexcept
    requires(not util::c_is_none<value_type>)
    {
        if constexpr (type == argument_type::positional)
            return not this->_default_values.empty();
        else
            return not this->_default_values.empty()
                or (this->is_used() and not this->_implicit_values.empty());
    }

    /**
     * @return Reference to the argument's predefined value list.
     * @throws std::logic_error if no predefined values are available.
     * @note The method is enabled only if `value_type` is not `none_type`.
     * @note - For positional arguments, the default value list is returned.
     * @note - For optional arguments, if the argument has been used, the implicit value list is returned, otherwise the default value list is returned.
     */
    [[nodiscard]] const std::vector<std::any>& _predefined_values() const
    requires(not util::c_is_none<value_type>)
    {
        if constexpr (type == argument_type::optional) {
            if (this->is_used()) {
                if (this->_implicit_values.empty())
                    throw(std::logic_error(std::format(
                        "No implicit values specified for argument '{}'.", this->_name.str()
                    )));

                return this->_implicit_values;
            }
        }

        if (this->_default_values.empty())
            throw(std::logic_error(
                std::format("No default values specified for argument '{}'.", this->_name.str())
            ));

        return this->_default_values;
    }

    /// @return `true` if the argument accepts further values, `false` otherwise.
    [[nodiscard]] bool _accepts_further_values() const noexcept {
        return not std::is_gt(this->_values.size() + 1ull <=> this->_nargs_range);
    }

    /// @return `true` if the given value is a valid choice for the argument, `false` otherwise.
    /// @todo Use std::ranges::contains after the switch to C++23
    [[nodiscard]] bool _is_valid_choice(const value_type& value) const noexcept
    requires(not util::c_is_none<value_type>)
    {
        return this->_choices.empty()
            or std::ranges::find(this->_choices, value) != this->_choices.end();
    }

    /**
     * @brief The implementation of the `set_value` method for none-type arguments.
     * @param str_value The string value to set.
     * @throws ap::parsing_failure
     * @attention Always throws! (`set_value` should never be called for a none-type argument).
     */
    bool _set_value_impl(const std::string& str_value)
    requires(util::c_is_none<value_type>)
    {
        throw parsing_failure(std::format(
            "Cannot set values for a none-type argument '{}' (value: '{}')",
            this->_name.str(),
            str_value
        ));
    }

    /**
     * @brief The implementation of the `set_value` method for non-none-type arguments.
     * @return `true` if the argument accepts further values, `false` otherwise.
     * @param str_value The string value to set.
     * @throws ap::parsing_failure if:
     * @throws - the argument does not accept further values (nargs limit exceeded).
     * @throws - the value cannot be parsed to the argument's `value_type`.
     * @throws - the value is not a valid choice for the argument (if choices are defined).
     * @note The method is enabled only if `value_type` is not `none_type`.
     */
    bool _set_value_impl(const std::string& str_value)
    requires(not util::c_is_none<value_type>)
    {
        if (not this->_accepts_further_values())
            throw parsing_failure::invalid_nvalues(this->_name, std::weak_ordering::greater);

        value_type value;
        if constexpr (util::c_trivially_readable<value_type>) {
            value = value_type(str_value);
        }
        else {
            if (not (std::istringstream(str_value) >> value))
                throw parsing_failure(std::format(
                    "Cannot parse value `{}` for argument [{}].", str_value, this->_name.str()
                ));
        }

        if (not this->_is_valid_choice(value))
            throw parsing_failure(std::format(
                "Value `{}` is not a valid choice for argument [{}].", str_value, this->_name.str()
            ));

        const auto apply_visitor = action::util::apply_visitor<value_type>{value};
        for (const auto& action : this->_value_actions)
            std::visit(apply_visitor, action);

        this->_values.emplace_back(std::move(value));
        return this->_accepts_further_values();
    }

    // attributes
    const ap::detail::argument_name _name; ///< The argument's name.
    std::optional<std::string> _help_msg; ///< The argument's help message.
    nargs::range _nargs_range; ///< The argument's nargs range attribute value.
    [[no_unique_address]] value_arg_specific_type<std::vector<std::any>>
        _default_values; ///< The argument's default value list.
    [[no_unique_address]] value_arg_specific_type<optional_specific_type<std::vector<std::any>>>
        _implicit_values; ///< The optional argument's implicit value list.
    [[no_unique_address]] value_arg_specific_type<std::vector<value_type>>
        _choices; ///< The argument's valid choices collection.
    [[no_unique_address]] optional_specific_type<std::vector<flag_action_type>>
        _flag_actions; ///< The optional argument's flag actions collection.
    [[no_unique_address]] value_arg_specific_type<std::vector<value_action_type>>
        _value_actions; ///< The argument's value actions collection.

    bool _required : 1; ///< The argument's `required` attribute value.
    bool _suppress_arg_checks : 1 =
        false; ///< The argument's `suppress_arg_checks` attribute value.
    bool _suppress_group_checks : 1 =
        false; ///< The argument's `suppress_group_checks` attribute value.
    bool _greedy : 1 = false; ///< The argument's `greedy` attribute value.
    bool _hidden : 1 = false; ///< The argument's `hidden` attribute value.

    // parsing result
    [[no_unique_address]] optional_specific_type<std::size_t>
        _count; ///< The argument's value count.
    std::vector<std::any> _values; ///< The argument's parsed values.

    // default attribute values
    static constexpr bool _default_required = (type == argument_type::positional);
    static constexpr nargs::range _default_nargs_range =
        (type == argument_type::positional) ? nargs::range(1ull) : nargs::any();
    static constexpr nargs::range _default_nargs_range_actual =
        util::c_is_none<value_type> ? nargs::range(0ull) : _default_nargs_range;
};

/**
 * @brief Positional argument alias.
 * @tparam T The value type accepted by the argument (defaults to std::string).
 * @see ap::argument
 */
template <util::c_argument_value_type T = std::string>
using positional_argument = argument<argument_type::positional, T>;

/**
 * @brief Optional argument alias.
 * @tparam T The value type accepted by the argument (defaults to std::string).
 * @see ap::argument
 */
template <util::c_argument_value_type T = std::string>
using optional_argument = argument<argument_type::optional, T>;

} // namespace ap
