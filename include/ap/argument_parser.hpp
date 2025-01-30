/*
CPP-AP: Command-line argument parser for C++20

MIT License

Copyright (c) 2023-2024 Jakub Musiał and other contributors
https://github.com/SpectraL519/cpp-ap

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

/*!
 * @file argument_parser.hpp
 * @brief CPP-AP library source file.
 *
 * This header file contians the entire CPP-AP library implementation.
 *
 * @version 2.0.0
 */

#pragma once

#include "detail/argument_interface.hpp"
#include "detail/argument_name.hpp"
#include "detail/concepts.hpp"
#include "error/exceptions.hpp"
#include "nargs/range.hpp"

#include <algorithm>
#include <any>
#include <compare>
#include <concepts>
#include <filesystem>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <ranges>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <typeinfo>
#include <variant>
#include <vector>

#ifdef AP_TESTING

namespace ap_testing {
struct positional_argument_test_fixture;
struct optional_argument_test_fixture;
struct argument_parser_test_fixture;
} // namespace ap_testing

#endif

namespace ap {

/// @brief Defines valued argument action traits.
struct valued_action {
    template <ap::detail::c_argument_value_type T>
    using type = std::function<T(const T&)>;
};

/// @brief Defines void argument action traits.
struct void_action {
    template <ap::detail::c_argument_value_type T>
    using type = std::function<void(T&)>;
};

// TODO:
// * on_read_action
// * on_flag_action

/// @brief Argument action handling utility.
namespace action {

/// @brief Internal argument action handling utility
namespace detail {

/**
 * @brief The concept is satisfied when `AS` is either a valued or void argument action
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept c_action_specifier = ap::detail::c_one_of<AS, ap::valued_action, ap::void_action>;

/// @brief Template argument action callable type alias.
template <c_action_specifier AS, ap::detail::c_argument_value_type T>
using callable_type = typename AS::template type<T>;

/// @brief Template argument action callabla variant type alias.
template <ap::detail::c_argument_value_type T>
using action_variant_type =
    std::variant<callable_type<ap::valued_action, T>, callable_type<ap::void_action, T>>;

/**
 * @brief Checks if an argument action variant holds a void action.
 * @tparam T The argument value type.
 * @param action The action variant.
 * @return True if the held action is a void action.
 */
template <ap::detail::c_argument_value_type T>
[[nodiscard]] constexpr inline bool is_void_action(const action_variant_type<T>& action) noexcept {
    return std::holds_alternative<callable_type<ap::void_action, T>>(action);
}

} // namespace detail

/// @brief Returns a default argument action.
template <ap::detail::c_argument_value_type T>
detail::callable_type<ap::void_action, T> default_action() noexcept {
    return [](T&) {};
}

/// @brief Returns a predefined action for file name handling arguments. Checks whether a file with the given name exists.
inline detail::callable_type<ap::void_action, std::string> check_file_exists() noexcept {
    return [](std::string& file_path) {
        if (not std::filesystem::exists(file_path))
            throw argument_parser_exception(std::format("File `{}` does not exists!", file_path));
    };
}

} // namespace action

/// @brief Namespace containing classes and utilities for handling command-line arguments.
namespace argument {

/**
 * @brief "Positional argument class of type T.
 * @tparam T The type of the argument value.
 */
template <ap::detail::c_argument_value_type T = std::string>
class positional_argument : public ap::detail::argument_interface {
public:
    using value_type = T; ///< Type of the argument value.

    positional_argument() = delete;

    /**
     * @brief Constructor for positional_argument with the `name` identifier.
     * @param name The `name` identifier of the positional argument.
     */
    positional_argument(const ap::detail::argument_name& name) : _name(name) {}

    ~positional_argument() = default;

    /**
     * @brief Equality operator for positional_argument.
     * @param other Another positional_argument for comparison.
     * @return Result of equality
     */
    bool operator==(const positional_argument& other) const noexcept {
        return this->_name == other._name;
    }

    /**
     * @brief Set the help message for the positional argument.
     * @param help_msg The help message to set.
     * @return Reference to the positional_argument.
     */
    positional_argument& help(std::string_view help_msg) noexcept override {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Set the choices for the positional argument.
     * @param choices The vector of valid choices for the argument.
     * @return Reference to the positional_argument.
     * @note Requires T to be equality comparable.
     */
    positional_argument& choices(const std::vector<value_type>& choices) noexcept
    requires(std::equality_comparable<value_type>)
    {
        this->_choices = choices;
        return *this;
    }

    /**
     * @brief Set the action for the positional argument.
     * @tparam AS The action specifier type (valued_action or void_action).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the positional_argument.
     */
    template <ap::action::detail::c_action_specifier AS, std::invocable<value_type&> F>
    positional_argument& action(F&& action) noexcept {
        using callable_type = ap::action::detail::callable_type<AS, value_type>;
        this->_action = std::forward<callable_type>(action);
        return *this;
    }

    /// @return True if the positional argument is optional., false if required.
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
     * @note Not required for positional arguments.
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
     * @return Reference to the positional_argument.
     */
    positional_argument& set_value(const std::string& str_value) override {
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

    /// @brief Get the stored value of the positional argument.
    [[nodiscard]] const std::any& value() const override {
        if (not this->_value.has_value())
            throw std::logic_error(
                std::format("No value parsed for the `{}` positional argument.", this->_name.str())
            );
        return this->_value;
    }

    /// @return Reference to the vector of parsed values for the positional argument.
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
        return this->_choices.empty()
            or std::ranges::find(this->_choices, choice) != this->_choices.end();
    }

    /**
     * @brief Apply the specified action to the value of the positional argument.
     * @param value The value to apply the action to.
     */
    void _apply_action(value_type& value) const noexcept {
        namespace action = ap::action::detail;
        if (action::is_void_action(this->_action))
            std::get<action::callable_type<ap::void_action, value_type>>(this->_action)(value);
        else
            value =
                std::get<action::callable_type<ap::valued_action, value_type>>(this->_action)(value
                );
    }

    using action_type = ap::action::detail::action_variant_type<T>;

    static constexpr bool _optional = false;
    const ap::detail::argument_name _name;
    std::optional<std::string> _help_msg;

    static constexpr bool _required = true; ///< Positional arguments are required by default.
    static constexpr bool _bypass_required =
        false; ///< Bypassing required status is defaultly not allowed for positional arguments.
    std::vector<value_type> _choices; ///< Vector of valid choices for the positional argument.
    action_type _action = ap::action::default_action<value_type>(
    ); ///< Action associated with the positional argument.

    std::any _value; ///< Stored value of the positional argument.

    std::stringstream _ss; ///< Stringstream used for parsing values.
};

/**
 * @brief Optional argument class of type T.
 * @tparam T The type of the argument value.
 */
template <ap::detail::c_argument_value_type T = std::string>
class optional_argument : public ap::detail::argument_interface {
public:
    using value_type = T;
    using count_type = ap::nargs::range::count_type;

    optional_argument() = delete;

    /**
     * @brief Constructor for optional_argument with the `name` identifier.
     * @param name The `name` identifier of the optional argument.
     */
    optional_argument(const ap::detail::argument_name& name) : _name(name) {}

    ~optional_argument() = default;

    /**
     * @brief Equality comparison operator for optional_argument.
     * @param other The optional_argument to compare with.
     * @return Equality of comparison.
     */
    bool operator==(const optional_argument& other) const noexcept {
        return this->_name == other._name;
    }

    /**
     * @brief Set the help message for the optional argument.
     * @param help_msg The help message to set.
     * @return Reference to the optional_argument.
     */
    optional_argument& help(std::string_view help_msg) noexcept override {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Mark the optional argument as required.
     * @return Reference to the optional_argument.
     */
    optional_argument& required() noexcept {
        this->_required = true;
        return *this;
    }

    /**
     * @brief Enable bypassing the required status for the optional argument.
     * @return Reference to the optional_argument.
     */
    optional_argument& bypass_required() noexcept {
        this->_bypass_required = true;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param range The nargs range to set.
     * @return Reference to the optional_argument.
     */
    optional_argument& nargs(const ap::nargs::range& range) noexcept {
        this->_nargs_range = range;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param count The count for nargs range.
     * @return Reference to the optional_argument.
     */
    optional_argument& nargs(const count_type count) noexcept {
        this->_nargs_range = ap::nargs::range(count);
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param nlow The lower bound for nargs range.
     * @param nhigh The upper bound for nargs range.
     * @return Reference to the optional_argument.
     */
    optional_argument& nargs(const count_type nlow, const count_type nhigh) noexcept {
        this->_nargs_range = ap::nargs::range(nlow, nhigh);
        return *this;
    }

    /**
     * @brief Set the action for the optional argument.
     * @tparam AS The action specifier type (valued_action or void_action).
     * @tparam F The type of the action function.
     * @param action The action function to set.
     * @return Reference to the optional_argument.
     */
    template <ap::action::detail::c_action_specifier AS, std::invocable<value_type&> F>
    optional_argument& action(F&& action) noexcept {
        using callable_type = ap::action::detail::callable_type<AS, value_type>;
        this->_action = std::forward<callable_type>(action);
        return *this;
    }

    /**
     * @brief Set the choices for the optional argument.
     * @param choices The vector of valid choices for the argument.
     * @return Reference to the optional_argument.
     * @note Requires T to be equality comparable.
     */
    optional_argument& choices(const std::vector<value_type>& choices) noexcept
    requires(std::equality_comparable<value_type>)
    {
        this->_choices = choices;
        return *this;
    }

    /**
     * @brief Set the default value for the optional argument.
     * @param default_value The default value to set.
     * @return Reference to the optional_argument.
     */
    optional_argument& default_value(const value_type& default_value) noexcept {
        this->_default_value = default_value;
        return *this;
    }

    /**
     * @brief Set the implicit value for the optional argument.
     * @param implicit_value The implicit value to set.
     * @return Reference to the optional_argument.
     */
    optional_argument& implicit_value(const value_type& implicit_value) noexcept {
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
     * @return Reference to the optional_argument.
     */
    optional_argument& set_value(const std::string& str_value) override {
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
        namespace action = ap::action::detail;
        if (action::is_void_action(this->_action))
            std::get<action::callable_type<ap::void_action, value_type>>(this->_action)(value);
        else
            value =
                std::get<action::callable_type<ap::valued_action, value_type>>(this->_action)(value
                );
    }

    using action_type = ap::action::detail::action_variant_type<T>;

    static constexpr bool _optional = true;
    const ap::detail::argument_name _name;
    std::optional<std::string> _help_msg;

    bool _required = false;
    bool _bypass_required = false;
    std::optional<ap::nargs::range> _nargs_range;
    action_type _action =
        ap::action::default_action<value_type>(); ///< Action associated with the opitonal argument.
    std::vector<value_type> _choices; ///< Vector of valid choices for the optional argument.
    std::any _default_value;
    std::any _implicit_value;

    std::size_t _nused = 0u; ///< Number of used optional arguments.
    std::vector<std::any> _values; ///< Vector holding parsed values for the optional argument.

    std::stringstream _ss; ///< Stringstream used for parsing values.
};

} // namespace argument

/// @brief Namespace containing default argument types.
namespace default_argument {

/// @brief Enum class representing positional arguments.
enum class positional : uint8_t { input, output };

/// @brief Enum class representing optional arguments.
enum class optional : uint8_t { help, input, output, multi_input, multi_output };

} // namespace default_argument

using default_posarg = default_argument::positional;
using default_optarg = default_argument::optional;

/// @brief Main argument parser class.
class argument_parser {
public:
    argument_parser(const argument_parser&) = delete;
    argument_parser& operator=(const argument_parser&) = delete;

    argument_parser() = default;

    argument_parser(argument_parser&&) = default;
    argument_parser& operator=(argument_parser&&) = default;

    ~argument_parser() = default;

    /**
     * @brief Set the program name.
     * @param name The name of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_name(std::string_view name) noexcept {
        this->_program_name = name;
        return *this;
    }

    /**
     * @brief Set the program description.
     * @param description The description of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_description(std::string_view description) noexcept {
        this->_program_description = description;
        return *this;
    }

    /**
     * @brief Set default positional arguments.
     * @param arg_discriminator_list Vector of default positional argument categories.
     * @return Reference to the argument parser.
     */
    argument_parser& default_positional_arguments(
        const std::vector<default_posarg>& arg_discriminator_list
    ) noexcept {
        for (const auto arg_discriminator : arg_discriminator_list)
            this->_add_default_positional_argument(arg_discriminator);
        return *this;
    }

    /**
     * @brief Set default optional arguments.
     * @param arg_discriminator_list Vector of default optional argument categories.
     * @return Reference to the argument parser.
     */
    argument_parser& default_optional_arguments(
        const std::vector<default_optarg>& arg_discriminator_list
    ) noexcept {
        for (const auto arg_discriminator : arg_discriminator_list)
            this->_add_default_optional_argument(arg_discriminator);
        return *this;
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @return Reference to the added positional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::positional_argument<T>& add_positional_argument(std::string_view primary_name) {
        // TODO: check forbidden characters

        const ap::detail::argument_name arg_name = {primary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_positional_args.emplace_back(
            std::make_unique<argument::positional_argument<T>>(arg_name)
        );
        return static_cast<argument::positional_argument<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added positional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::positional_argument<T>& add_positional_argument(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        // TODO: check forbidden characters

        const ap::detail::argument_name arg_name = {primary_name, secondary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_positional_args.emplace_back(
            std::make_unique<argument::positional_argument<T>>(arg_name)
        );
        return static_cast<argument::positional_argument<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @return Reference to the added optional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::optional_argument<T>& add_optional_argument(std::string_view primary_name) {
        // TODO: check forbidden characters

        const ap::detail::argument_name arg_name = {primary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_optional_args.push_back(std::make_unique<argument::optional_argument<T>>(arg_name));
        return static_cast<argument::optional_argument<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added optional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::optional_argument<T>& add_optional_argument(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        // TODO: check forbidden characters

        const ap::detail::argument_name arg_name = {primary_name, secondary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_optional_args.emplace_back(std::make_unique<argument::optional_argument<T>>(arg_name)
        );
        return static_cast<argument::optional_argument<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a boolean flag argument to the parser's configuration.
     * @tparam StoreImplicitly Flag indicating whether to store implicitly.
     * @param primary_name The primary name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    argument::optional_argument<bool>& add_flag(std::string_view primary_name) {
        return this->add_optional_argument<bool>(primary_name)
            .default_value(not StoreImplicitly)
            .implicit_value(StoreImplicitly)
            .nargs(0);
    }

    /**
     * @brief Adds a boolean flag argument to the parser's configuration.
     * @tparam StoreImplicitly Flag indicating whether to store implicitly.
     * @param primary_name The primary name of the flag.
     * @param secondary_name The secondary name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    argument::optional_argument<bool>& add_flag(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        return this->add_optional_argument<bool>(primary_name, secondary_name)
            .default_value(not StoreImplicitly)
            .implicit_value(StoreImplicitly)
            .nargs(0);
    }

    /**
     * @brief Parse command-line arguments.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     */
    void parse_args(int argc, char* argv[]) {
        this->_parse_args_impl(this->_tokenize(argc, argv));

        if (this->_bypass_required_args())
            return;

        this->_check_required_args();
        this->_check_nvalues_in_range();
    }

    /**
     * @brief Check if an argument has a value.
     * @param arg_name The name of the argument.
     * @return True if the argument has a value, false otherwise.
     */
    bool has_value(std::string_view arg_name) const noexcept {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().has_value() : false;
    }

    /**
     * @brief Get the count of times a positional or optional argument has been used.
     * @param arg_name The name of the argument.
     * @return The count of times the argument has been used.
     */
    std::size_t count(std::string_view arg_name) const noexcept {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().nused() : 0ull;
    }

    /**
     * @brief Get the value of a typed argument.
     * @tparam T Type of the argument value.
     * @param arg_name The name of the argument.
     * @return The value of the argument.
     */
    template <std::copy_constructible T = std::string>
    T value(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        if (not arg_opt)
            throw error::argument_not_found(arg_name);

        const auto& arg_value = arg_opt->get().value();
        try {
            return std::any_cast<T>(arg_value);
        }
        catch (const std::bad_any_cast& err) {
            throw error::invalid_value_type(arg_opt->get().name(), typeid(T));
        }
    }

    /**
     * @brief Get the values of a typed argument.
     * @tparam T Type of the argument values.
     * @param arg_name The name of the argument.
     * @return The values of the argument as a vector.
     */
    template <std::copy_constructible T = std::string>
    std::vector<T> values(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        if (not arg_opt)
            throw error::argument_not_found(arg_name);

        const auto& arg = arg_opt->get();

        try {
            if (not arg.has_parsed_values() and arg.has_value())
                return std::vector<T>{std::any_cast<T>(arg.value())};

            std::vector<T> values;
            // TODO: use std::ranges::to after transition to C++23
            std::ranges::copy(
                std::views::transform(
                    arg.values(), [](const std::any& value) { return std::any_cast<T>(value); }
                ),
                std::back_inserter(values)
            );
            return values;
        }
        catch (const std::bad_any_cast& err) {
            throw error::invalid_value_type(arg.name(), typeid(T));
        }
    }

    /**
     * @brief Overloaded stream insertion operator for printing the argument parser details.
     * @param os Output stream.
     * @param parser The argument parser to print.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const argument_parser& parser) noexcept {
        if (parser._program_name)
            os << parser._program_name.value() << std::endl;

        if (parser._program_description)
            os << parser._program_description.value() << std::endl;

        for (const auto& argument : parser._positional_args)
            os << "\t" << *argument << std::endl;

        for (const auto& argument : parser._optional_args)
            os << "\t" << *argument << std::endl;

        return os;
    }

#ifdef AP_TESTING
    /// @brief Friend struct for testing purposes.
    friend struct ::ap_testing::argument_parser_test_fixture;
#endif

private:
    /**
     * @brief Add default positional argument based on the specified discriminator.
     * @param arg_discriminator The default positional argument discriminator.
     */
    void _add_default_positional_argument(const default_posarg arg_discriminator) noexcept {
        switch (arg_discriminator) {
        case default_posarg::input:
            this->add_positional_argument("input")
                .action<ap::void_action>(ap::action::check_file_exists())
                .help("Input file path");
            break;

        case default_posarg::output:
            this->add_positional_argument("output").help("Output file path");
            break;
        }
    }

    /**
     * @brief Add default optional argument based on the specified discriminator.
     * @param arg_discriminator The default optional argument discriminator.
     */
    void _add_default_optional_argument(const default_optarg arg_discriminator) noexcept {
        switch (arg_discriminator) {
        case default_optarg::help:
            this->add_flag("help", "h").bypass_required().help("Display help message");
            break;

        case default_optarg::input:
            this->add_optional_argument("input", "i")
                .required()
                .nargs(1)
                .action<ap::void_action>(ap::action::check_file_exists())
                .help("Input file path");
            break;

        case default_optarg::output:
            this->add_optional_argument("output", "o").required().nargs(1).help("Output file path");
            break;

        case default_optarg::multi_input:
            this->add_optional_argument("input", "i")
                .required()
                .nargs(ap::nargs::at_least(1))
                .action<ap::void_action>(ap::action::check_file_exists())
                .help("Input files paths");
            break;

        case default_optarg::multi_output:
            this->add_optional_argument("output", "o")
                .required()
                .nargs(ap::nargs::at_least(1))
                .help("Output files paths");
            break;
        }
    }

    /// @brief Structure representing a single command-line argument token.
    struct arg_token {
        enum class token_type : bool { flag, value };

        arg_token() = default;

        arg_token(const arg_token&) = default;
        arg_token(arg_token&&) = default;

        arg_token& operator=(const arg_token&) = default;
        arg_token& operator=(arg_token&&) = default;

        /**
         * @brief Constructor of a command-line argument.
         * @param type Type type of the token (flag or value).
         * @param value The value of the argument.
         */
        arg_token(const token_type type, const std::string& value) : type(type), value(value) {}

        ~arg_token() = default;

        /**
         * @brief Equality operator for comparing arg_token instances.
         * @param other An arg_token instance to compare with.
         * @return Boolean statement of equality comparison.
         */
        bool operator==(const arg_token& other) const noexcept {
            return this->type == other.type and this->value == other.value;
        }

        token_type type;
        std::string value;
    };

    using arg_token_list = std::vector<arg_token>;
    using arg_token_list_iterator = typename arg_token_list::const_iterator;

    using argument_ptr_type = std::unique_ptr<ap::detail::argument_interface>;
    using argument_opt_type = std::optional<std::reference_wrapper<ap::detail::argument_interface>>;
    using argument_list_type = std::vector<argument_ptr_type>;
    using argument_list_iterator_type = typename argument_list_type::iterator;
    using argument_list_const_iterator_type = typename argument_list_type::const_iterator;
    using argument_predicate_type = std::function<bool(const argument_ptr_type&)>;

    /**
     * @brief Returns an unary predicate function which checks if the given name matches the argument's name
     * @param arg_name The name of the argument.
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] argument_predicate_type _name_match_predicate(std::string_view arg_name
    ) const noexcept {
        return [arg_name](const argument_ptr_type& arg) { return arg->name().match(arg_name); };
    }

    /**
     * @brief Returns an unary predicate function which checks if the given name matches the argument's name
     * @param arg_name The name of the argument.
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] argument_predicate_type _name_match_predicate(
        const ap::detail::argument_name& arg_name
    ) const noexcept {
        return [&arg_name](const argument_ptr_type& arg) { return arg->name().match(arg_name); };
    }

    /**
     * @brief Check if an argument name is already used.
     * @param arg_name The name of the argument.
     * @return True if the argument name is already used, false otherwise.
     */
    [[nodiscard]] bool _is_arg_name_used(const ap::detail::argument_name& arg_name) const noexcept {
        const auto predicate = this->_name_match_predicate(arg_name);

        if (std::ranges::find_if(this->_positional_args, predicate) != this->_positional_args.end())
            return true;

        if (std::ranges::find_if(this->_optional_args, predicate) != this->_optional_args.end())
            return true;

        return false;
    }

    /**
     * @brief Processes the command-line arguments by converting them into tokens.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return List of preprocessed command-line argument tokens.
     */
    [[nodiscard]] arg_token_list _tokenize(int argc, char* argv[]) const noexcept {
        if (argc < 2)
            return arg_token_list{};

        arg_token_list args;
        args.reserve(argc - 1);

        for (int i = 1; i < argc; ++i) {
            std::string value = argv[i];
            if (this->_is_flag(value)) {
                this->_strip_flag_prefix(value);
                args.emplace_back(arg_token::token_type::flag, std::move(value));
            }
            else {
                args.emplace_back(arg_token::token_type::value, std::move(value));
            }
        }

        return args;
    }

    /**
     * @brief Check if an argument is a flag based on its value.
     * @param arg The cmd argument's value.
     * @return True if the argument is a flag, false otherwise.
     */
    [[nodiscard]] bool _is_flag(const std::string& arg) const noexcept {
        if (arg.starts_with(this->_flag_prefix))
            return this->_is_arg_name_used({arg.substr(this->_flag_prefix_length)});

        if (arg.starts_with(this->_flag_prefix_char))
            return this->_is_arg_name_used({arg.substr(this->_flag_prefix_char_length)});

        return false;
    }

    /**
     * @brief Remove the flag prefix from the argument.
     * @param arg_flag The argument flag to strip the prefix from.
     */
    void _strip_flag_prefix(std::string& arg_flag) const noexcept {
        if (arg_flag.starts_with(this->_flag_prefix))
            arg_flag.erase(0, this->_flag_prefix_length);
        else
            arg_flag.erase(0, this->_flag_prefix_char_length);
    }

    /**
     * @brief Implementation of parsing command-line arguments.
     * @param arg_tokens The list of command-line argument tokens.
     */
    void _parse_args_impl(const arg_token_list& arg_tokens) {
        arg_token_list_iterator token_it = arg_tokens.begin();
        this->_parse_positional_args(arg_tokens, token_it);
        this->_parse_optional_args(arg_tokens, token_it);
    }

    /**
     * @brief Parse positional arguments based on command-line input.
     * @param arg_tokens The list of command-line argument tokens.
     * @param token_it Iterator for iterating through command-line argument tokens.
     */
    void _parse_positional_args(
        const arg_token_list& arg_tokens, arg_token_list_iterator& token_it
    ) noexcept {
        for (const auto& pos_arg : this->_positional_args) {
            if (token_it == arg_tokens.end())
                return;

            if (token_it->type == arg_token::token_type::flag)
                return;

            pos_arg->set_value(token_it->value);
            ++token_it;
        }
    }

    /**
     * @brief Parse optional arguments based on command-line input.
     * @param arg_tokens The list of command-line argument tokens.
     * @param token_it Iterator for iterating through command-line argument tokens.
     */
    void _parse_optional_args(const arg_token_list& arg_tokens, arg_token_list_iterator& token_it) {
        std::optional<std::reference_wrapper<argument_ptr_type>> curr_opt_arg;

        while (token_it != arg_tokens.end()) {
            if (token_it->type == arg_token::token_type::flag) {
                auto opt_arg_it = std::ranges::find_if(
                    this->_optional_args, this->_name_match_predicate(token_it->value)
                );

                if (opt_arg_it == this->_optional_args.end())
                    throw error::argument_not_found(token_it->value);

                curr_opt_arg = std::ref(*opt_arg_it);
                curr_opt_arg->get()->mark_used();
            }
            else {
                if (not curr_opt_arg)
                    throw error::free_value(token_it->value);

                curr_opt_arg->get()->set_value(token_it->value);
            }

            ++token_it;
        }
    }

    /**
     * @brief Check if optional arguments can bypass the required arguments.
     * @return True if optional arguments can bypass required arguments, false otherwise.
     */
    [[nodiscard]] bool _bypass_required_args() const noexcept {
        return std::ranges::any_of(this->_optional_args, [](const argument_ptr_type& arg) {
            return arg->is_used() and arg->bypass_required_enabled();
        });
    }

    /// @brief Check if all required positional and optional arguments are used.
    void _check_required_args() const {
        for (const auto& arg : this->_positional_args)
            if (not arg->is_used())
                throw error::required_argument_not_parsed(arg->name());

        for (const auto& arg : this->_optional_args)
            if (arg->is_required() and not arg->has_value())
                throw error::required_argument_not_parsed(arg->name());
    }

    /// @brief Check if the number of argument values is within the specified range.
    void _check_nvalues_in_range() const {
        for (const auto& arg : this->_positional_args) {
            const auto nvalues_ordering = arg->nvalues_in_range();
            if (not std::is_eq(nvalues_ordering))
                throw error::invalid_nvalues(nvalues_ordering, arg->name());
        }

        for (const auto& arg : this->_optional_args) {
            const auto nvalues_ordering = arg->nvalues_in_range();
            if (not std::is_eq(nvalues_ordering))
                throw error::invalid_nvalues(nvalues_ordering, arg->name());
        }
    }

    /**
     * @brief Get the argument with the specified name.
     * @param arg_name The name of the argument.
     * @return The argument with the specified name, if found; otherwise, std::nullopt.
     */
    argument_opt_type _get_argument(std::string_view arg_name) const noexcept {
        const auto predicate = this->_name_match_predicate(arg_name);

        if (auto pos_arg_it = std::ranges::find_if(this->_positional_args, predicate);
            pos_arg_it != this->_positional_args.end()) {
            return std::ref(**pos_arg_it);
        }

        if (auto opt_arg_it = std::ranges::find_if(this->_optional_args, predicate);
            opt_arg_it != this->_optional_args.end()) {
            return std::ref(**opt_arg_it);
        }

        return std::nullopt;
    }

    std::optional<std::string> _program_name;
    std::optional<std::string> _program_description;

    argument_list_type _positional_args;
    argument_list_type _optional_args;

    static constexpr uint8_t _flag_prefix_char_length = 1u;
    static constexpr uint8_t _flag_prefix_length = 2u;
    static constexpr char _flag_prefix_char = '-';
    const std::string _flag_prefix =
        "--"; // not static constexpr because of ubuntu gcc implementation :(
};

} // namespace ap
