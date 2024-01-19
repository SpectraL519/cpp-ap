/*
[cpp-ap] Command-line argument parser for C++20

MIT License

Copyright (c) 2023 Jakub Musiał

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
 * @brief Header file for the C++20 argument parser library.
 */

#pragma once

#include <algorithm>
#include <any>
#include <compare>
#include <concepts>
#include <filesystem>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <variant>
#include <vector>
#include <typeinfo>

#ifdef AP_TESTING

namespace ap_testing {
struct positional_argument_test_fixture;
struct optional_argument_test_fixture;
struct argument_parser_test_fixture;
} // namespace ap_testing

#endif

/// @brief Main argument parser library namespace. 
namespace ap {

class argument_parser;

/**
 * @namespace utility
 * @brief Namespace containing utility concepts and templates for the argument parser.
 */
namespace utility {

/**
 * @brief Concept for types that can be read from an input stream.
 * @tparam T The type to check for readability.
 */
template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };

/**
 * @brief Concept for types that are valid as argument values.
 * @tparam T The type to check for validity as an argument value.
 */
template <typename T>
concept valid_argument_value_type =
    readable<T> and
    std::copy_constructible<T> and std::assignable_from<T&, const T&>;

/**
 * @brief Concept for types that can be compared for equality.
 * @tparam T The type to check for equality comparability.
 */
template <typename T>
concept equality_comparable = requires(T lhs, T rhs) {
    { lhs == rhs } -> std::convertible_to<bool>;
};

/**
 * @brief Template to check if a type is one of the valid types.
 * @tparam T The type to check.
 * @tparam ValidTypes The valid types to compare against.
 */
template <typename T, typename... ValidTypes>
struct is_valid_type : std::disjunction<std::is_same<T, ValidTypes>...> {};

/**
 * @brief Constant expression indicating whether a type is one of the valid types.
 * @tparam T The type to check.
 * @tparam ValidTypes The valid types to compare against.
 */
template <typename T, typename... ValidTypes>
inline constexpr bool is_valid_type_v = is_valid_type<T, ValidTypes...>::value;

} // namespace utility


/**
 * @namespace nargs
 * @brief Namespace containing classes and functions related to specifying argument counts.
 */
namespace nargs {

/// @brief Class representing a range of valid argument counts.
class range {
public:
    using count_type = std::size_t;

    /// @brief Constructor for the range class without specified argument count.
    range() : _nlow(_ndefault), _nhigh(_ndefault) {}

    /**
     * @brief Constructor for the range class with a single argument count.
     * @param n The argument count.
     */
    range(const count_type n)
        : _nlow(n), _nhigh(n), _default(n == _ndefault) {}

    /**
     * @brief Constructor for the range class with a range of argument counts.
     * @param nlow The lower bound of the range.
     * @param nhigh The upper bound of the range.
     */
    range(const count_type nlow, const count_type nhigh)
        : _nlow(nlow), _nhigh(nhigh), _default(false) {}

    /**
     * @brief Copy assignment operator for the range class.
     * @param rhs The range to copy.
     * @return Reference to the assigned range.
     */
    range& operator= (const range&) = default;

    /// @brief Destructor for the range class.
    ~range() = default;

    /// @return True if the range is using default values, false otherwise.
    [[nodiscard]] inline bool is_default() const {
        return this->_default;
    }

    /**
     * @brief Check if a specific count is within the range.
     * @param n The count to check.
     * @return Ordering relationship between the count and the range.
     */
    [[nodiscard]] std::weak_ordering contains(const range::count_type n) const {
        if (not (this->_nlow.has_value() or this->_nhigh.has_value()))
            return std::weak_ordering::equivalent;

        if (this->_nlow.has_value() and this->_nhigh.has_value()) {
            if (n < this->_nlow.value())
                return std::weak_ordering::less;

            if (n > this->_nhigh.value())
                return std::weak_ordering::greater;

            return std::weak_ordering::equivalent;
        }

        if (this->_nlow.has_value())
            return (n < this->_nlow.value()) ? std::weak_ordering::less
                                             : std::weak_ordering::equivalent;

        return (n > this->_nhigh.value()) ? std::weak_ordering::greater
                                          : std::weak_ordering::equivalent;
    }

    /**
     * @brief Friend function to create a range representing at least a specified count.
     * @param n The minimum count.
     * @return Range representing at least the specified count.
     */
    friend range at_least(const count_type);

    /**
     * @brief Friend function to create a range representing more than a specified count.
     * @param n The count.
     * @return Range representing more than the specified count.
     */
    friend range more_than(const count_type);

    /**
     * @brief Friend function to create a range representing less than a specified count.
     * @param n The count.
     * @return Range representing less than the specified count.
     */
    friend range less_than(const count_type);

    /**
     * @brief Friend function to create a range representing up to a specified count.
     * @param n The maximum count.
     * @return Range representing up to the specified count.
     */
    friend range up_to(const count_type);

    /**
     * @brief Friend function to create a range representing any count.
     * @return Range representing any count.
     */
    friend range any();

private:
    /**
     * @brief Private constructor for the range class with optional lower and upper bounds.
     * @param nlow The optional lower bound of the range.
     * @param nhigh The optional upper bound of the range.
     */
    range(const std::optional<count_type> nlow, const std::optional<count_type> nhigh)
        : _nlow(nlow), _nhigh(nhigh) {}

    std::optional<count_type> _nlow;
    std::optional<count_type> _nhigh;
    bool _default = true;

    static constexpr count_type _ndefault = 1;
};

/**
 * @brief Create a range representing at least a specified count.
 * @param n The minimum count.
 * @return Range representing at least the specified count.
 */
[[nodiscard]] inline range at_least(const range::count_type n) {
    return range(n, std::nullopt);
}

/**
 * @brief Create a range representing more than a specified count.
 * @param n The count.
 * @return Range representing more than the specified count.
 */
[[nodiscard]] inline range more_than(const range::count_type n) {
    return range(n + 1, std::nullopt);
}

/**
 * @brief Create a range representing less than a specified count.
 * @param n The count.
 * @return Range representing less than the specified count.
 */
[[nodiscard]] inline range less_than(const range::count_type n) {
    return range(std::nullopt, n - 1);
}

/**
 * @brief Create a range representing up to a specified count.
 * @param n The maximum count.
 * @return Range representing up to the specified count.
 */
[[nodiscard]] inline range up_to(const range::count_type n) {
    return range(std::nullopt, n);
}

/**
 * @brief Create a range representing any count.
 * @return Range representing any count.
 */
[[nodiscard]] inline range any() {
    return range(std::nullopt, std::nullopt);
}

} // namespace nargs


/// @brief Struct providing a template type alias for a valued action function.
struct valued_action {
    template <ap::utility::valid_argument_value_type T>
    using type = std::function<T(const T&)>;
};

/// @brief Struct providing a template type alias for a void action function.
struct void_action {
    template <ap::utility::valid_argument_value_type T>
    using type = std::function<void(T&)>;
};

// TODO: on_read_action

/// @brief Namespace containing utilities for handling actions associated with argument parsing.
namespace action {

/// @brief Internal namespace containing details of the action utilities.
namespace detail {

/**
 * @brief Concept checking if a type is a valid action specifier.
 * @tparam AS The action specifier type.
 */
template <typename AS>
concept valid_action_specifier = ap::utility::is_valid_type_v<AS, ap::valued_action, ap::void_action>;

/// @brief Alias for the callable type of a specified action specifier and argument value type.
template <valid_action_specifier AS, ap::utility::valid_argument_value_type T>
using callable_type = typename AS::template type<T>;

/// @brief Alias for the variant type representing different action specifier types.
template <ap::utility::valid_argument_value_type T>
using action_variant_type =
    std::variant<callable_type<ap::valued_action, T>, callable_type<ap::void_action, T>>;

/**
 * @brief Check if an action variant is a void action.
 * @tparam T The argument value type.
 * @param action The action variant.
 * @return True if the action is a void action, false otherwise.
 */
template <ap::utility::valid_argument_value_type T>
[[nodiscard]] inline bool is_void_action(const action_variant_type<T>& action) {
    return std::holds_alternative<callable_type<ap::void_action, T>>(action);
}

} // namespace detail

/// @brief Default void action for argument values.
template <ap::utility::valid_argument_value_type T>
detail::callable_type<ap::void_action, T> default_action{ [](T&) {} };

/// @brief Action to check if a file exists for string argument values.
inline detail::callable_type<ap::void_action, std::string> check_file_exists_action{
    [](std::string& file_path) {
        if (not std::filesystem::exists(file_path)) {
            std::cerr << "[ERROR] : File " + file_path + " does not exists!";
            std::exit(EXIT_FAILURE);
        }
    }
};

} // namespace action


/// @brief Namespace containing classes and utilities for handling command-line arguments.
namespace argument::detail {

/// @brief Represents the name of a command-line argument.
struct argument_name {
    /// @brief Default constructor (deleted).
    argument_name() = delete;

    /// @brief Deleted copy assignment operator for argument_name.
    argument_name& operator= (const argument_name&) = delete;

    /// @brief Copy constructor
    argument_name(const argument_name&) = default;

    /// @brief Move constructor
    argument_name(argument_name&&) = default;

    /**
     * @brief Constructor for argument_name with only a long name.
     * @param name The primary name of the argument.
     */
    explicit argument_name(std::string_view name) : name(name) {}

    /**
     * @brief Constructor for argument_name with a long name and a short name.
     * @param name The primary name of the argument.
     * @param short_name The short name of the argument (optional).
     */
    explicit argument_name(std::string_view name, std::string_view short_name)
        : name(name), short_name(short_name) {}

    /// @brief Default destructor for argument_name.
    ~argument_name() = default;

    /**
     * @brief Equality comparison operator.
     * @param other The argument_name to compare with.
     * @return Equality of argument names.
     */
    inline bool operator== (const argument_name& other) const {
        return this->name == other.name;
    }

    /**
     * @brief Equality comparison operator for string variables representing argument names.
     * @param name The string view to compare with.
     * @return Equality of names comparison (either full or short name).
     */
    inline bool operator== (std::string_view name) const {
        return name == this->name or
               (this->short_name and name == this->short_name.value());
    }

    /// @brief Get a string representation of the argument_name.
    [[nodiscard]] inline std::string str() const {
        return this->short_name ? ("[" + this->name + "," + this->short_name.value() + "]")
                                : ("[" + this->name + "]");
    }

    /**
     * @brief Stream insertion operator for argument names.
     * @param os The output stream.
     * @param arg_name The argument name to be inserted into the stream.
     * @return The modified output stream.
     */
    friend std::ostream& operator<< (std::ostream& os, const argument_name& arg_name) {
        os << arg_name.str();
        return os;
    }

    const std::string name; ///< The primary name of the argument.
    const std::optional<std::string> short_name; ///< The optional short name of the argument.
};

/// @brief Argument class interface
class argument_interface {
public:
    /**
     * @brief Set the help message for the argument.
     * @param msg The help message to set.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& help(std::string_view) = 0;

    /// @return True if the argument is optional, false otherwise.
    virtual bool is_optional() const = 0;

    /// @brief Destructor for argument_interface.
    virtual ~argument_interface() = default;

    /**
     * @brief Overloaded stream insertion operator.
     * @param os The output stream.
     * @param argument The argument_interface to output.
     * @return The output stream.
     */
    friend std::ostream& operator<< (std::ostream& os, const argument_interface& argument) {
        os << argument.name() << " : ";
        const auto& argument_help_msg = argument.help();
        os << (argument_help_msg ? argument_help_msg.value() : "[ostream(argument)] TODO: msg");
        return os;
    }

    friend class ::ap::argument_parser;

protected:
    /// @return Reference to the name of the argument.
    virtual const argument_name& name() const = 0;

    /// @return True if the argument is required, false otherwise
    virtual bool is_required() const = 0;

    /// @return True if bypassing the required status is enabled for the argument, false otherwise.
    virtual bool bypass_required_enabled() const = 0;

    /// @return Optional help message for the argument.
    virtual const std::optional<std::string>& help() const = 0;

    /// @brief Mark the argument as used.
    virtual void set_used() = 0;

    /// @return True if the argument has been used, false otherwise.
    virtual bool is_used() const = 0;

    /// @return The number of times the positional argument is used.
    virtual std::size_t nused() const = 0;

    /**
     * @brief Set the value for the argument.
     * @param value The string representation of the value.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& set_value(const std::string&) = 0;

    /// @return True if the argument has a value, false otherwise.
    virtual bool has_value() const = 0;

    /// @return True if the argument has parsed values., false otherwise.
    virtual bool has_parsed_values() const = 0;

    /// @return The ordering relationship of argument range.
    virtual std::weak_ordering nvalues_in_range() const = 0;

    /// @return Reference to the stored value of the argument.
    virtual const std::any& value() const = 0;

    /// @return Reference to the vector of parsed values of the argument.
    virtual const std::vector<std::any>& values() const = 0;
};

} // namespace argument::detail


/**
 * @brief Base class for exceptions thrown by the argument parser.
 *
 * This class is derived from std::runtime_error and serves as the base class for all
 * exceptions related to the argument parser functionality.
 */
class argument_parser_error : public std::runtime_error {
public:
    /**
     * @brief Constructor for the argument_parser_error class.
     * @param message A descriptive error message providing information about the exception.
     */
    explicit argument_parser_error(const std::string& message)
    : std::runtime_error(message) {}
};


/// @brief Namespace containing custom exception classes for argument parser errors.
namespace error {

/// @brief Exception thrown when attempting to set a value for an argument that already has one.
class value_already_set_error : public argument_parser_error {
public:
    /**
     * @brief Constructor for the value_already_set_error class.
     * @param arg_name The name of the argument that already has a value set.
     */
    explicit value_already_set_error(const argument::detail::argument_name& arg_name)
    : argument_parser_error("Value for argument " + arg_name.str() + " has already been set") {}
};

/// @brief Exception thrown when the value provided for an argument cannot be parsed.
class invalid_value_error : public argument_parser_error{
public:
    /**
     * @brief Constructor for the invalid_value_error class.
     * @param arg_name The name of the argument for which the value parsing failed.
     * @param value The value that failed to parse.
     */
    explicit invalid_value_error(
        const argument::detail::argument_name& arg_name, const std::string& value
    ) : argument_parser_error(
        "Cannot parse value `" + value + "` for argument " + arg_name.str()) {}
};

/// @brief Exception thrown when the provided value is not in the choices for an argument.
class invalid_choice_error : public argument_parser_error {
public:
    /**
     * @brief Constructor for the invalid_choice_error class.
     * @param arg_name The name of the argument for which the value is not in choices.
     * @param value The value that is not in the allowed choices.
     */
    explicit invalid_choice_error(
        const argument::detail::argument_name& arg_name, const std::string& value
    ) : argument_parser_error(
        "Value `" + value + "` is not a valid choice for argument " + arg_name.str()) {}
};

/// @brief Exception thrown when there is a collision in argument names.
class argument_name_used_error : public argument_parser_error {
public:
    /**
     * @brief Constructor for the argument_name_used_error class.
     * @param given_arg_name The name of the argument causing the collision.
     */
    explicit argument_name_used_error(const std::string_view& given_arg_name)
    : argument_parser_error("Given name `" + std::string(given_arg_name) + "` already used") {}

    /**
     * @brief Constructor for the argument_name_used_error class with a short name.
     * @param given_arg_name The name of the argument causing the collision.
     * @param given_arg_name_short The short name of the argument causing the collision.
     */
    explicit argument_name_used_error(
        const std::string_view& given_arg_name, const std::string_view& given_arg_name_short
    ) : argument_parser_error(
        "Given name " +
        argument::detail::argument_name(given_arg_name, given_arg_name_short).str() + " already used") {}
};

/// @brief Exception thrown when an argument with a specific name is not found.
class argument_not_found_error : public argument_parser_error {
public:
    /**
     * @brief Constructor for the argument_name_not_found_error class.
     * @param arg_name The name of the argument that was not found.
     */
    explicit argument_not_found_error(const std::string_view& arg_name)
    : argument_parser_error("Argument with given name `" + std::string(arg_name) + "` not found.") {}
};

/// @brief Exception thrown when there is an attempt to cast to an invalid type.
class invalid_value_type_error : public argument_parser_error {
public:
    /**
     * @brief Constructor for the invalid_value_type_error class.
     * @param arg_name The name of the argument that had invalid value type.
     * @param given_arg_type The type information that failed to cast.
     */
    explicit invalid_value_type_error(
        const argument::detail::argument_name& arg_name, const std::type_info& given_arg_type
    ) : argument_parser_error(
        "Invalid value type specified for argument " + arg_name.str() + " - " + given_arg_type.name()) {}
};

/// @brief Exception thrown when a required argument is not parsed.
class required_argument_not_parsed_error : public argument_parser_error {
public:
    /**
     * @brief Constructor for the required_argument_not_parsed_error class.
     * @param arg_name The name of the required argument that was not parsed.
     */
    explicit required_argument_not_parsed_error(const argument::detail::argument_name& arg_name)
    : argument_parser_error("No values parsed for a required argument " + arg_name.str()) {}
};

/// @brief Exception thrown when there is an error deducing the argument for a given value.
class free_value_error : public argument_parser_error {
public:
    /**
     * @brief Constructor for the free_value_error class.
     * @param value The value for which the argument deduction failed.
     */
    explicit free_value_error(const std::string& value)
    : argument_parser_error("Failed to deduce the argument for the given value `" + value + "`") {}
};

/// @brief Exception thrown when an invalid number of values is provided for an argument.
class invalid_nvalues_error : public argument_parser_error {
public:
    /**
     * @brief Helper function to generate an error message based on the weak_ordering result.
     * @param ordering The result of the weak_ordering comparison.
     * @param arg_name The name of the argument for which the error occurred.
     * @return The error message.
     */
    [[nodiscard]] static std::string msg(
        const std::weak_ordering ordering, const argument::detail::argument_name& arg_name
    ) {
        if(std::is_lt(ordering))
            return "Too few values provided for optional argument " + arg_name.str();
        else
            return "Too many values provided for optional argument " + arg_name.str();
    }

    /**
     * @brief Constructor for the invalid_nvalues_error class.
     * @param ordering The result of the weak_ordering comparison.
     * @param arg_name The name of the argument for which the error occurred.
     */
    explicit invalid_nvalues_error(
        const std::weak_ordering ordering, const argument::detail::argument_name& arg_name
    ) : argument_parser_error(invalid_nvalues_error::msg(ordering, arg_name)) {}
};

} // namespace error



/// @brief Namespace containing classes and utilities for handling command-line arguments.
namespace argument {

/**
 * @brief "Positional argument class of type T.
 * @tparam T The type of the argument value.
 */
template <utility::valid_argument_value_type T>
class positional_argument : public detail::argument_interface {
public:
    using value_type = T; ///< Type of the argument value.

    /// @brief Deleted default constructor.
    positional_argument() = delete;

    /**
     * @brief Constructor for positional_argument with a name.
     * @param name The primary name of the positional argument.
     */
    positional_argument(std::string_view name) : _name(name) {}

    /**
     * @brief Constructor for positional_argument with a name and a short name.
     * @param name The primary name of the positional argument.
     * @param short_name The short name of the positional argument (optional).
     */
    positional_argument(std::string_view name, std::string_view short_name)
        : _name(name, short_name) {}

    /// @brief Destructor for positional argument.
    ~positional_argument() = default;

    /**
     * @brief Equality operator for positional_argument.
     * @param other Another positional_argument for comparison.
     * @return Result of equality
     */
    inline bool operator== (const positional_argument& other) const {
        return this->_name == other._name;
    }

    /**
     * @brief Set the help message for the positional argument.
     * @param help_msg The help message to set.
     * @return Reference to the positional_argument.
     */
    inline positional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Set the choices for the positional argument.
     * @param choices The vector of valid choices for the argument.
     * @return Reference to the positional_argument.
     * @note Requires T to be equality comparable.
     */
    inline positional_argument& choices(const std::vector<value_type>& choices)
    requires(utility::equality_comparable<value_type>) {
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
    template <ap::action::detail::valid_action_specifier AS, std::invocable<value_type&> F>
    inline positional_argument& action(F&& action) {
        using callable_type = ap::action::detail::callable_type<AS, value_type>;
        this->_action = std::forward<callable_type>(action);
        return *this;
    }

    /// @return True if the positional argument is optional., false if required.
    [[nodiscard]] inline bool is_optional() const override { return this->_optional; }

    
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
    [[nodiscard]] inline const detail::argument_name& name() const override {
        return this->_name;
    }

    /// @return Optional help message for the positional argument.
    [[nodiscard]] inline const std::optional<std::string>& help() const override {
        return this->_help_msg;
    }

    /// @return True if the positional argument is required, false otherwise
    [[nodiscard]] inline bool is_required() const override {
        return this->_required;
    }

    /// @return True if bypassing the required status is enabled for the positional argument, false otherwise.
    [[nodiscard]] inline bool bypass_required_enabled() const override {
        return this->_bypass_required;
    }

    /**
     * @brief Mark the positional argument as used.
     * @note Not required for positional arguments.
     */
    void set_used() override {}

    /// @return True if the positional argument is used, false otherwise.
    [[nodiscard]] inline bool is_used() const override {
        return this->_value.has_value();
    }

    /// @return The number of times the positional argument is used.
    [[nodiscard]] inline std::size_t nused() const override {
        return static_cast<std::size_t>(this->_value.has_value());
    }

    /**
     * @brief Set the value for the positional argument.
     * @param str_value The string representation of the value.
     * @return Reference to the positional_argument.
     */
    positional_argument& set_value(const std::string& str_value) override {
        if (this->_value.has_value())
            throw error::value_already_set_error(this->_name);

        this->_ss.clear();
        this->_ss.str(str_value);

        value_type value;
        if (not (this->_ss >> value))
            throw error::invalid_value_error(this->_name, this->_ss.str());

        if (not this->_is_valid_choice(value))
            throw error::invalid_choice_error(this->_name, str_value);

        this->_apply_action(value);

        this->_value = value;
        return *this;
    }

    /// @return True if the positional argument has a value, false otherwise.
    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value();
    }

    /// @return True if the positional argument has parsed values, false otherwise.
    [[nodiscard]] inline bool has_parsed_values() const override {
        return this->_value.has_value();
    }

    /// @return Ordering relationship of positional argument range.
    [[nodiscard]] inline std::weak_ordering nvalues_in_range() const override {
        return this->_value.has_value() ? std::weak_ordering::equivalent
                                        : std::weak_ordering::less;
    }

    /// @brief Get the stored value of the positional argument.
    [[nodiscard]] inline const std::any& value() const override {
        return this->_value;
    }

    /// @return Reference to the vector of parsed values for the positional argument.
    [[nodiscard]] inline const std::vector<std::any>& values() const override {
        throw std::logic_error("Positional argument " + this->_name.name + "has only 1 value.");
    }

    /**
     * @brief Check if the provided choice is valid for the positional argument.
     * @param choice The value to check against choices.
     * @return True if the choice valid, false otherwise.
     */
    [[nodiscard]] inline bool _is_valid_choice(const value_type& choice) const {
        return this->_choices.empty() or
               std::find(this->_choices.begin(), this->_choices.end(), choice) != this->_choices.end();
    }

    /**
     * @brief Apply the specified action to the value of the positional argument.
     * @param value The value to apply the action to.
     */
    void _apply_action(value_type& value) const {
        namespace action = ap::action::detail;
        if (action::is_void_action(this->_action))
            std::get<action::callable_type<ap::void_action, value_type>>(this->_action)(value);
        else
            value = std::get<action::callable_type<ap::valued_action, value_type>>(this->_action)(value);
    }

    using action_type = ap::action::detail::action_variant_type<T>;

    static constexpr bool _optional{false};
    const detail::argument_name _name;
    std::optional<std::string> _help_msg;

    static constexpr bool _required{true}; ///< Positional arguments are required by default.
    static constexpr bool _bypass_required{false}; ///< Bypassing required status is defaultly not allowed for positional arguments.
    std::vector<value_type> _choices; ///< Vector of valid choices for the positional argument.
    action_type _action{ap::action::default_action<value_type>}; ///< Action associated with the positional argument.

    std::any _value; ///< Stored value of the positional argument.

    std::stringstream _ss; ///< Stringstream used for parsing values.
};

/**
 * @brief Optional argument class of type T.
 * @tparam T The type of the argument value.
 */
template <utility::valid_argument_value_type T>
class optional_argument : public detail::argument_interface {
public:
    using value_type = T;
    using count_type = ap::nargs::range::count_type;

    /// @brief Deleted default constructor for optional_argument.
    optional_argument() = delete;

    /**
     * @brief Constructor for optional_argument with a name.
     * @param name The primary name of the optional argument.
     */
    optional_argument(std::string_view name) : _name(name) {}

    /**
     * @brief Constructor for optional_argument with a name and a short name.
     * @param name The primary name of the optional argument.
     * @param short_name The short name of the optional argument (optional).
     */
    optional_argument(std::string_view name, std::string_view short_name)
        : _name(name, short_name) {}

    /// @brief Destructor for optional_argument.
    ~optional_argument() = default;

    /**
     * @brief Equality comparison operator for optional_argument.
     * @param other The optional_argument to compare with.
     * @return Equality of comparison.
     */
    inline bool operator== (const optional_argument& other) const {
        return this->_name == other._name;
    }

    /**
     * @brief Set the help message for the optional argument.
     * @param help_msg The help message to set.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Mark the optional argument as required.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& required() {
        this->_required = true;
        return *this;
    }

    /**
     * @brief Enable bypassing the required status for the optional argument.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& bypass_required() {
        this->_bypass_required = true;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param range The nargs range to set.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& nargs(const ap::nargs::range& range) {
        this->_nargs_range = range;
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param count The count for nargs range.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& nargs(const count_type count) {
        this->_nargs_range = ap::nargs::range(count);
        return *this;
    }

    /**
     * @brief Set the nargs range for the optional argument.
     * @param nlow The lower bound for nargs range.
     * @param nhigh The upper bound for nargs range.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& nargs(const count_type nlow, const count_type nhigh) {
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
    template <ap::action::detail::valid_action_specifier AS, std::invocable<value_type&> F>
    inline optional_argument& action(F&& action) {
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
    inline optional_argument& choices(const std::vector<value_type>& choices)
    requires(utility::equality_comparable<value_type>) {
        this->_choices = choices;
        return *this;
    }

    /**
     * @brief Set the default value for the optional argument.
     * @param default_value The default value to set.
     * @return Reference to the optional_argument.
     */
    optional_argument& default_value(const value_type& default_value) {
        this->_default_value = default_value;
        return *this;
    }

    /**
     * @brief Set the implicit value for the optional argument.
     * @param implicit_value The implicit value to set.
     * @return Reference to the optional_argument.
     */
    optional_argument& implicit_value(const value_type& implicit_value) {
        this->_implicit_value = implicit_value;
        return *this;
    }

    /// @return True if argument is optional, false otherwise.
    [[nodiscard]] inline bool is_optional() const override { return this->_optional; }

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
    [[nodiscard]] inline const detail::argument_name& name() const override {
        return this->_name;
    }

    /// @return Reference to the optional help message for the optional argument.
    [[nodiscard]] inline const std::optional<std::string>& help() const override {
        return this->_help_msg;
    }

    /// @return True if the optional argument is required, false otherwise.
    [[nodiscard]] inline bool is_required() const override {
        return this->_required;
    }

    /// @return True if bypassing the required status is enabled for the optional argument, false otherwise.
    [[nodiscard]] inline bool bypass_required_enabled() const override {
        return this->_bypass_required;
    }

    /// @brief Mark the optional argument as used.
    void set_used() override {
        this->_nused++;
    }

    /// @return True if the optional argument is used, false otherwise.
    [[nodiscard]] inline bool is_used() const override {
        return this->_nused > 0;
    }

    /// @return The number of times the optional argument is used.
    [[nodiscard]] inline std::size_t nused() const override {
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
            throw error::invalid_value_error(this->_name, this->_ss.str());

        if (not this->_is_valid_choice(value))
            throw error::invalid_choice_error(this->_name, str_value);

        this->_apply_action(value);

        if (not (this->_nargs_range or this->_values.empty()))
            throw error::value_already_set_error(this->_name);

        this->_values.push_back(value);
        return *this;
    }

    /// @return True if the optional argument has a value, false otherwise.
    [[nodiscard]] inline bool has_value() const override {
        return this->has_parsed_values() or this->_has_predefined_value();
    }

    /// @return True if parsed values are available for the optional argument, false otherwise.
    [[nodiscard]] inline bool has_parsed_values() const override {
        return not this->_values.empty();
    }

    /// @return ordering relationship of optional argument range.
    [[nodiscard]] std::weak_ordering nvalues_in_range() const override {
        if (not this->_nargs_range)
            return std::weak_ordering::equivalent;

        if (this->_values.empty() and this->_has_predefined_value())
            return std::weak_ordering::equivalent;

        return this->_nargs_range->contains(this->_values.size());
    }

    /// @return Reference to the stored value of the optional argument.
    [[nodiscard]] inline const std::any& value() const override {
        return this->_values.empty() ? this->_predefined_value() : this->_values.front();
    }

    /// @return Reference to the vector of parsed values for the optional argument.
    [[nodiscard]] inline const std::vector<std::any>& values() const override {
        return this->_values;
    }

    /// @return True if the optional argument has a predefined value, false otherwise.
    [[nodiscard]] inline bool _has_predefined_value() const {
        return this->_default_value.has_value() or
               (this->is_used() and this->_implicit_value.has_value());
    }

    /// @return Reference to the predefined value of the optional argument.
    [[nodiscard]] inline const std::any& _predefined_value() const {
        return this->is_used() ? this->_implicit_value : this->_default_value;
    }

    /**
     * @brief Check if the provided choice is valid for the optional argument.
     * @param choice The value to check against choices.
     * @return True if choice is valid, false otherwise.
     */
    [[nodiscard]] inline bool _is_valid_choice(const value_type& choice) const {
        return this->_choices.empty() or
               std::find(this->_choices.begin(), this->_choices.end(), choice) != this->_choices.end();
    }

    /**
     * @brief Apply the specified action to the value of the optional argument.
     * @param value The value to apply the action to.
     */
    void _apply_action(value_type& value) const {
        namespace action = ap::action::detail;
        if (action::is_void_action(this->_action))
            std::get<action::callable_type<ap::void_action, value_type>>(this->_action)(value);
        else
            value = std::get<action::callable_type<ap::valued_action, value_type>>(this->_action)(value);
    }

    using action_type = ap::action::detail::action_variant_type<T>;

    static constexpr bool _optional{true};
    const detail::argument_name _name;
    std::optional<std::string> _help_msg;

    bool _required{false};
    bool _bypass_required{false}; 
    std::optional<ap::nargs::range> _nargs_range;
    action_type _action{ap::action::default_action<value_type>}; ///< Action associated with the opitonal argument.
    std::vector<value_type> _choices; ///< Vector of valid choices for the optional argument.
    std::any _default_value;
    std::any _implicit_value;

    std::size_t _nused{0u}; ///< Number of used optional arguments.
    std::vector<std::any> _values; ///< Vector holding parsed values for the optional argument.

    std::stringstream _ss; ///< Stringstream used for parsing values.
};

} // namespace argument


/// @brief Namespace containing default argument types.
namespace default_argument {

/// @brief Enum class representing positional arguments.
enum class positional : uint8_t {
    input,
    output
};

/// @brief Enum class representing optional arguments.
enum class optional : uint8_t {
    help,
    input,
    output,
    multi_input,
    multi_output
};

} // namespace default_argument


/// @brief Main argument parser class.
class argument_parser {
public:
    /// @brief Default constructor.
    argument_parser() = default;

    /// @brief Deleted copy constructor.
    argument_parser(const argument_parser&) = delete;

    /// @brief Deleted move constructor.
    argument_parser(argument_parser&&) = delete;

    /// @brief Deleted copy assignment operator.
    argument_parser& operator= (const argument_parser&) = delete;

    /// @brief Destructor for the argument parser.
    ~argument_parser() = default;

    /**
     * @brief Set the program name.
     * @param name The name of the program.
     * @return Reference to the argument parser.
     */
    inline argument_parser& program_name(std::string_view name) {
        this->_program_name = name;
        return *this;
    }

    /**
     * @brief Set the program description.
     * @param description The description of the program.
     * @return Reference to the argument parser.
     */
    inline argument_parser& program_description(std::string_view description) {
        this->_program_description = description;
        return *this;
    }

    /**
     * @brief Set default positional arguments.
     * @param args Vector of default positional argument categories.
     * @return Reference to the argument parser.
     */
    inline argument_parser& default_positional_arguments(
        const std::vector<default_argument::positional>& args
    ) {
        for (const auto arg : args)
            this->_add_default_positional_argument(arg);
        return *this;
    }

    /**
     * @brief Set default optional arguments.
     * @param args Vector of default optional argument categories.
     * @return Reference to the argument parser.
     */
    inline argument_parser& default_optional_arguments(
        const std::vector<default_argument::optional>& args
    ) {
        for (const auto arg : args)
            this->_add_default_optional_argument(arg);
        return *this;
    }

    /**
     * @brief Add a positional argument to the parser.
     * @tparam T Type of the argument value.
     * @param name The name of the argument.
     * @return Reference to the added positional argument.
     */
    template <utility::valid_argument_value_type T = std::string>
    argument::positional_argument<T>& add_positional_argument(std::string_view name) {
        // TODO: check forbidden characters

        if (this->_is_arg_name_used(name))
            throw error::argument_name_used_error(name);

        this->_positional_args.push_back(
            std::make_unique<argument::positional_argument<T>>(name));
        return static_cast<argument::positional_argument<T>&>(
            *this->_positional_args.back());
    }

    /**
     * @brief Add a positional argument with a short name to the parser.
     * @tparam T Type of the argument value.
     * @param name The name of the argument.
     * @param short_name The short name of the argument.
     * @return Reference to the added positional argument.
     */
    template <utility::valid_argument_value_type T = std::string>
    argument::positional_argument<T>& add_positional_argument(
        std::string_view name, std::string_view short_name
    ) {
        // TODO: check forbidden characters

        if (this->_is_arg_name_used(name, short_name))
            throw error::argument_name_used_error(name, short_name);

        this->_positional_args.push_back(
            std::make_unique<argument::positional_argument<T>>(name, short_name));
        return static_cast<argument::positional_argument<T>&>(
            *this->_positional_args.back());
    }

    /**
     * @brief Add an optional argument to the parser.
     * @tparam T Type of the argument value.
     * @param name The name of the argument.
     * @return Reference to the added optional argument.
     */
    template <utility::valid_argument_value_type T = std::string>
    argument::optional_argument<T>& add_optional_argument(std::string_view name) {
        // TODO: check forbidden characters

        if (this->_is_arg_name_used(name))
            throw error::argument_name_used_error(name);

        this->_optional_args.push_back(std::make_unique<argument::optional_argument<T>>(name));
        return static_cast<argument::optional_argument<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Add an optional argument with a short name to the parser.
     * @tparam T Type of the argument value.
     * @param name The name of the argument.
     * @param short_name The short name of the argument.
     * @return Reference to the added optional argument.
     */
    template <utility::valid_argument_value_type T = std::string>
    argument::optional_argument<T>& add_optional_argument(std::string_view name, std::string_view short_name) {
        // TODO: check forbidden characters

        if (this->_is_arg_name_used(name, short_name))
            throw error::argument_name_used_error(name, short_name);

        this->_optional_args.push_back(
            std::make_unique<argument::optional_argument<T>>(name, short_name));
        return static_cast<argument::optional_argument<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Add a boolean flag to the parser.
     * @tparam StoreImplicitly Flag indicating whether to store implicitly.
     * @param name The name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    argument::optional_argument<bool>& add_flag(std::string_view name) {
        return this->add_optional_argument<bool>(name)
                    .default_value(not StoreImplicitly)
                    .implicit_value(StoreImplicitly)
                    .nargs(0);
    }

    /**
     * @brief Add a boolean flag with a short name to the parser.
     * @tparam StoreImplicitly Flag indicating whether to store implicitly.
     * @param name The name of the flag.
     * @param short_name The short name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    argument::optional_argument<bool>& add_flag(
        std::string_view name, std::string_view short_name
    ) {
        return this->add_optional_argument<bool>(name, short_name)
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
        this->_parse_args_impl(this->_process_input(argc, argv));

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
    bool has_value(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().has_value() : false; // TODO: throw
    }

    /**
     * @brief Get the count of times a positional or optional argument has been used.
     * @param arg_name The name of the argument.
     * @return The count of times the argument has been used.
     */
    std::size_t count(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().nused() : 0u; // TODO: throw
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
            throw error::argument_not_found_error(arg_name);

        try {
            T value{std::any_cast<T>(arg_opt->get().value())};
            return value;
        }
        catch (const std::bad_any_cast& err) {
            throw error::invalid_value_type_error(arg_opt->get().name(), typeid(T));
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
            throw error::argument_not_found_error(arg_name);

        const auto& arg = arg_opt->get();

        try {
            if (not arg.has_parsed_values() and arg.has_value())
                return std::vector<T>{std::any_cast<T>(arg.value())};

            const auto& arg_values = arg.values();

            std::vector<T> values;
            std::transform(
                std::begin(arg_values),
                std::end(arg_values),
                std::back_inserter(values),
                [] (const std::any& value) { return std::any_cast<T>(value); }
            );
            return values;
        }
        catch (const std::bad_any_cast& err) {
            throw error::invalid_value_type_error(arg.name(), typeid(T));
        }
    }

    /**
     * @brief Overloaded stream insertion operator for printing the argument parser details.
     * @param os Output stream.
     * @param parser The argument parser to print.
     * @return The modified output stream.
     */
    friend std::ostream& operator<< (std::ostream& os, const argument_parser& parser) {
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
     * @brief Add default positional argument based on the specified category.
     * @param arg The default positional argument category.
     */
    void _add_default_positional_argument(const default_argument::positional arg) {
        switch (arg) {
            case default_argument::positional::input:
                this->add_positional_argument("input")
                     .action<ap::void_action>(ap::action::check_file_exists_action)
                     .help("Input file path");
                break;

            case default_argument::positional::output:
                this->add_positional_argument("output")
                     .help("Output file path");
                break;
        }
    }

    /**
     * @brief Add default optional argument based on the specified category.
     * @param arg The default optional argument category.
     */
    void _add_default_optional_argument(const default_argument::optional arg) {
        switch (arg) {
            case default_argument::optional::help:
                this->add_flag("help", "h")
                     .bypass_required()
                     .help("Display help message");
                break;

            case default_argument::optional::input:
                this->add_optional_argument("input", "i")
                     .required()
                     .nargs(1)
                     .action<ap::void_action>(ap::action::check_file_exists_action)
                     .help("Input file path");
                break;

            case default_argument::optional::output:
                this->add_optional_argument("output", "o")
                     .required()
                     .nargs(1)
                     .help("Output file path");
                break;

            case default_argument::optional::multi_input:
                this->add_optional_argument("input", "i")
                     .required()
                     .nargs(ap::nargs::at_least(1))
                     .action<ap::void_action>(ap::action::check_file_exists_action)
                     .help("Input file path");
                break;

            case default_argument::optional::multi_output:
                this->add_optional_argument("output", "o")
                     .required()
                     .nargs(ap::nargs::at_least(1))
                     .help("Input file path");
                break;
        }
    }

    /// @brief Structure representing a command-line argument.
    struct cmd_argument {
        enum class type_discriminator : bool { flag, value };

        cmd_argument() = default;
        cmd_argument(const cmd_argument&) = default;
        cmd_argument(cmd_argument&&) = default;
        cmd_argument& operator= (const cmd_argument&) = default;

        /**
         * @brief Constructor of a command-line argument.
         * @param discriminator Type discriminator (flag or value).
         * @param value The value of the argument.
         */
        cmd_argument(
            const type_discriminator discriminator, const std::string& value
        ) : discriminator(discriminator), value(value) {}

        ~cmd_argument() = default;

        /**
         * @brief Equality operator for comparing cmd_argument instances.
         * @param other Another cmd_argument to compare with.
         * @return Boolean statement of equality comparison.
         */
        inline bool operator== (const cmd_argument& other) const {
            return this->discriminator == other.discriminator and
                   this->value == other.value;
        }

        type_discriminator discriminator;
        std::string value;
    };

    using cmd_argument_list = std::vector<cmd_argument>;
    using cmd_argument_list_iterator = typename cmd_argument_list::const_iterator;

    using argument_ptr_type = std::unique_ptr<argument::detail::argument_interface>;
    using argument_opt_type =
        std::optional<std::reference_wrapper<argument::detail::argument_interface>>;
    using argument_list_type = std::vector<argument_ptr_type>;
    using argument_list_iterator_type = typename argument_list_type::iterator;
    using argument_list_const_iterator_type = typename argument_list_type::const_iterator;
    using argument_predicate_type = std::function<bool(const argument_ptr_type&)>;

    /**
     * @brief Function to create a predicate for finding arguments by name.
     * @param name The name of the argument.
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] inline argument_predicate_type _name_eq_predicate(
        const std::string_view& name
    ) const {
        return [&name](const argument_ptr_type& arg) {
            return name == arg->name();
        };
    }

    /**
     * @brief Function to create a predicate for finding arguments by name and short name.
     * @param name The name of the argument.
     * @param short_name The short name of the argument.
     * @return Argument predicate based on the provided name and short name.
     */
    [[nodiscard]] inline argument_predicate_type _name_eq_predicate(
        const std::string_view& name, const std::string_view& short_name
    ) const {
        return [&name, &short_name](const argument_ptr_type& arg) {
            return name == arg->name() or short_name == arg->name();
        };
    }

    /**
     * @brief Check if an argument name is already used.
     * @param name The name of the argument.
     * @return True if the argument name is already used, false otherwise.
     */
    [[nodiscard]] bool _is_arg_name_used(const std::string_view& name) const {
        const auto predicate = this->_name_eq_predicate(name);

        if (this->_const_find_positional(predicate) != this->_positional_args.end())
            return true;

        if (this->_const_find_optional(predicate) != this->_optional_args.end())
            return true;

        return false;
    }

    /**
     * @brief Check if an argument name and short name pair is already used.
     * @param name The name of the argument.
     * @param short_name The short name of the argument.
     * @return True if the argument name or short name is already used, false otherwise.
     */
    [[nodiscard]] bool _is_arg_name_used(
        const std::string_view& name, const std::string_view& short_name
    ) const {
        const auto predicate = this->_name_eq_predicate(name, short_name);

        if (this->_const_find_positional(predicate) != this->_positional_args.end())
            return true;

        if (this->_const_find_optional(predicate) != this->_optional_args.end())
            return true;

        return false;
    }

    /**
     * @brief Process command-line input arguments.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return List of processed command-line arguments.
     */
    [[nodiscard]] cmd_argument_list _process_input(int argc, char* argv[]) const {
        if (argc < 2)
            return cmd_argument_list{};

        cmd_argument_list args;
        args.reserve(argc - 1);

        for (int i = 1; i < argc; i++) {
            std::string value = argv[i];
            if (this->_is_flag(value)) {
                this->_strip_flag_prefix(value);
                args.push_back(cmd_argument{cmd_argument::type_discriminator::flag, value});
            }
            else {
                args.push_back(cmd_argument{cmd_argument::type_discriminator::value, value});
            }
        }

        return args;
    }

    /**
     * @brief Check if an argument is a flag based on its value.
     * @param arg The argument value.
     * @return True if the argument is a flag, false otherwise.
     */
    [[nodiscard]] bool _is_flag(const std::string& arg) const {
        if (arg.starts_with(this->_flag_prefix))
            return this->_is_arg_name_used(arg.substr(this->_flag_prefix_length));

        if (arg.starts_with(this->_flag_prefix_char))
            return this->_is_arg_name_used(arg.substr(this->_flag_prefix_char_length));

        return false;
    }

    /**
     * @brief Remove the flag prefix from the argument.
     * @param arg The argument to strip the prefix from.
     */
    void _strip_flag_prefix(std::string& arg) const {
        if (arg.starts_with(this->_flag_prefix))
            arg.erase(0, this->_flag_prefix_length);
        else
            arg.erase(0, this->_flag_prefix_char_length);
    }

    /**
     * @brief Implementation of parsing command-line arguments.
     * @param cmd_args The list of command-line arguments.
     */
    void _parse_args_impl(const cmd_argument_list& cmd_args) {
        cmd_argument_list_iterator cmd_it = cmd_args.begin();
        this->_parse_positional_args(cmd_args, cmd_it);
        this->_parse_optional_args(cmd_args, cmd_it);
    }

    /**
     * @brief Parse positional arguments based on command-line input.
     * @param cmd_args The list of command-line arguments.
     * @param cmd_it Iterator for iterating through command-line arguments.
     */
    void _parse_positional_args(
        const cmd_argument_list& cmd_args, cmd_argument_list_iterator& cmd_it
    ) {
        // TODO: align tests
        for (const auto& pos_arg : this->_positional_args) {
            if (cmd_it == cmd_args.end())
                return;

            if (cmd_it->discriminator == cmd_argument::type_discriminator::flag)
                return;

            pos_arg->set_value(cmd_it->value);
            cmd_it++;
        }
    }

    /**
     * @brief Parse optional arguments based on command-line input.
     * @param cmd_args The list of command-line arguments.
     * @param cmd_it Iterator for iterating through command-line arguments.
     */
    void _parse_optional_args(
        const cmd_argument_list& cmd_args, cmd_argument_list_iterator& cmd_it
    ) {
        std::optional<std::reference_wrapper<argument_ptr_type>> curr_opt_arg;

        while (cmd_it != cmd_args.end()) {
            if (cmd_it->discriminator == cmd_argument::type_discriminator::flag) {
                auto opt_arg_it =
                    this->_find_optional(this->_name_eq_predicate(cmd_it->value));

                if (opt_arg_it == this->_optional_args.end())
                    throw error::argument_not_found_error(cmd_it->value);

                curr_opt_arg = std::ref(*opt_arg_it);
                curr_opt_arg->get()->set_used();
            }
            else {
                if (not curr_opt_arg)
                    throw error::free_value_error(cmd_it->value);

                curr_opt_arg->get()->set_value(cmd_it->value);
            }

            cmd_it++;
        }
    }

    /**
     * @brief Check if optional arguments can bypass the required arguments.
     * @return True if optional arguments can bypass required arguments, false otherwise.
     */
    [[nodiscard]] inline bool _bypass_required_args() const {
        return std::any_of(
            std::cbegin(this->_optional_args), std::cend(this->_optional_args),
            [](const argument_ptr_type& arg) {
                return arg->is_used() and arg->bypass_required_enabled();
            }
        );
    }

    /// @brief Check if all required positional and optional arguments are used.
    void _check_required_args() const {
        for (const auto& arg : this->_positional_args)
            if (not arg->is_used())
                throw error::required_argument_not_parsed_error(arg->name());

        for (const auto& arg : this->_optional_args)
            if (arg->is_required() and not arg->has_value())
                throw error::required_argument_not_parsed_error(arg->name());
    }

    /// @brief Check if the number of argument values is within the specified range.
    void _check_nvalues_in_range() const {
        for (const auto& arg : this->_positional_args) {
            const auto nvalues_ordering = arg->nvalues_in_range();
            if (not std::is_eq(nvalues_ordering))
                throw error::invalid_nvalues_error(nvalues_ordering, arg->name());
        }

        for (const auto& arg : this->_optional_args) {
            const auto nvalues_ordering = arg->nvalues_in_range();
            if (not std::is_eq(nvalues_ordering))
                throw error::invalid_nvalues_error(nvalues_ordering, arg->name());
        }
    }

    /**
     * @brief Get the argument with the specified name.
     * @param name The name of the argument.
     * @return The argument with the specified name, if found; otherwise, std::nullopt.
     */
    argument_opt_type _get_argument(const std::string_view& name) const {
        const auto predicate = this->_name_eq_predicate(name);

        if (auto pos_arg_it = this->_const_find_positional(predicate);
            pos_arg_it != this->_positional_args.end()) {
            return std::ref(**pos_arg_it);
        }

        if (auto opt_arg_it = this->_const_find_optional(predicate);
            opt_arg_it != this->_optional_args.end()) {
            return std::ref(**opt_arg_it);
        }

        return std::nullopt;
    }

    /**
     * @brief Find a positional argument based on the provided predicate.
     * @param predicate The predicate for finding the argument.
     * @return Iterator to the found positional argument.
     */
    [[nodiscard]] inline argument_list_iterator_type _find_positional(
        const argument_predicate_type& predicate
    ) {
        return std::find_if(
            std::begin(this->_positional_args), std::end(this->_positional_args), predicate);
    }

    /**
     * @brief Find an optional argument based on the provided predicate.
     * @param predicate The predicate for finding the argument.
     * @return Iterator to the found optional argument.
     */
    [[nodiscard]] inline argument_list_iterator_type _find_optional(
        const argument_predicate_type& predicate
    ) {
        return std::find_if(
            std::begin(this->_optional_args), std::end(this->_optional_args), predicate);
    }

    /**
     * @brief Find a positional argument based on the provided predicate (const version).
     * @param predicate The predicate for finding the argument.
     * @return Iterator to the found positional argument.
     */
    [[nodiscard]] inline argument_list_const_iterator_type _const_find_positional(
        const argument_predicate_type& predicate
    ) const {
        return std::find_if(
            std::cbegin(this->_positional_args), std::cend(this->_positional_args), predicate);
    }

    /**
     * @brief Find an optional argument based on the provided predicate (const version).
     * @param predicate The predicate for finding the argument.
     * @return Iterator to the found optional argument.
     */
    [[nodiscard]] inline argument_list_const_iterator_type _const_find_optional(
        const argument_predicate_type& predicate
    ) const {
        return std::find_if(
            std::cbegin(this->_optional_args), std::cend(this->_optional_args), predicate);
    }

    std::optional<std::string> _program_name;
    std::optional<std::string> _program_description;

    argument_list_type _positional_args;
    argument_list_type _optional_args;

    static constexpr uint8_t _flag_prefix_char_length{1u};
    static constexpr uint8_t _flag_prefix_length{2u};
    static constexpr char _flag_prefix_char{'-'};
    const std::string _flag_prefix{"--"}; // not static constexpr because of ubuntu :(
};

} // namespace ap
