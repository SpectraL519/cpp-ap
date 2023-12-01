/*!
 * @file argument_parser.hpp
 * @brief Header file for the argument parser library in C++.
 */

#pragma once

#include <any>
#include <iostream>
#include <optional>
#include <string_view>

/**
 * @namespace ap
 * @brief Namespace for the argument parser library. 
 */
namespace ap {

namespace detail {

/**
 * @brief Concept for types that can be read from an input stream.
 * @tparam T The type to check for readability.
 */
template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };



/**
 * @brief Represents the name of a command-line argument.
 */
struct argument_name {
    /// @brief Default constructor (deleted).
    argument_name() = delete;

    /// @brief Copy constructor.
    argument_name(const argument_name&) = default;

    /// @brief Move constructor. 
    argument_name(argument_name&&) = default;

    /// @brief Default copy assignment operator for argument_name.
    /// @return Reference to the assigned argument_name.
    argument_name& operator= (const argument_name&) = default;

    /// @brief Explicit parameterized constructor with only a long name. 
    explicit argument_name(const std::string_view name) : name(name) {}

    /// @brief Explicit parameterized constructor with both long and short names.
    explicit argument_name(const std::string_view name, const std::string_view short_name)
        : name(name), short_name(short_name) {}

    /// @brief argument_name destructor.
    ~argument_name() = default;

    /**
     * @brief Equality comparison operator for argument names.
     * @param other The argument name struct to compare with.
     * @return Equality of argument names.
     */
    inline bool operator== (const argument_name& other) const {
        return this->name == other.name;
    }

    /**
     * @brief Equality comparison operator for long or short argument names.
     * @param name The name to compare with.
     * @return Equality of names comparison (either full or short name).
     */
    inline bool operator== (const std::string_view name) const {
        return name == this->name or
               (this->short_name and name == this->short_name.value());
    }

    /**
     * @brief Stream insertion operator for argument names.
     * @param os The output stream.
     * @param arg_name The argument name to be inserted into the stream.
     * @return The modified output stream.
     */
    friend std::ostream& operator<< (std::ostream& os, const argument_name& arg_name) {
        os << "[" << arg_name.name;
        if (arg_name.short_name)
            os << "," << arg_name.short_name.value();
        os << "]";
        return os;
    }

    /// @brief The long name of the argument.
    const std::string_view name;

    /// @brief The optional short name of the argument.
    const std::optional<std::string_view> short_name;
};



/**
 * @brief Interface for argument configuration.
 */
class argument_interface {
public:
    /**
     * @brief Sets the help message for the argument.
     * @param help Help message.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& help(std::string_view) = 0;

    /**
     * @brief Sets whether the argument is required.
     * @param required True if the argument is required.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& required(bool) = 0;

    /**
     * @brief Sets the default value for the argument.
     * @param value Default value.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& default_value(const std::any&) = 0;

    /// @brief Argument interface destructor.
    virtual ~argument_interface() = default;

protected:
    /**
     * @brief Sets the value for the argument.
     * @param value The value to set.
     * @return Reference to the argument_interface.
     */
    virtual argument_interface& value(const std::any&) = 0;

    /**
     * @brief Checks if the argument is positional.
     * @return True if the argument is positional.
     */
    virtual bool is_positional() const = 0;

    /**
     * @brief Checks if the argument is optional.
     * @return True if the argument is optional.
     */
    virtual bool is_optional() const = 0;

    /**
     * @brief Checks if the argument has a value.
     * @return True if the argument has a value.
     */
    virtual bool has_value() const = 0;

    /**
     * @brief Gets the value of the argument.
     * @return The value of the argument.
     */
    virtual const std::any& value() const = 0;

    /**
     * @brief Gets the name of the argument.
     * @return The argument_name struct reference.
     */
    virtual const argument_name& name() const = 0;

    /**
     * @brief Checks if the argument is required.
     * @return True if the argument is required.
     */
    virtual bool required() const = 0;

    /**
     * @brief Gets the help message for the argument.
     * @return The help message for the argument.
     */
    virtual const std::optional<std::string_view>& help() const = 0;

    /**
     * @brief Gets the default value for the argument.
     * @return The default value for the argument.
     */
    virtual const std::any& default_value() const = 0;

#ifdef AP_TESTING
    friend inline bool testing_argument_is_positional(const argument_interface&);
    friend inline bool testing_argument_is_optional(const argument_interface&);
    friend inline bool testing_argument_has_value(const argument_interface&);
    friend inline const std::any&
        testing_argument_get_value(const argument_interface&);
    friend inline const argument_name&
        testing_argument_get_name(const argument_interface&);
    friend inline bool testing_argument_is_required(const argument_interface&);
    friend inline const std::optional<std::string_view>&
        testing_argument_get_help(const argument_interface&);
    friend inline const std::any&
        testing_argument_get_default_value(const argument_interface&);
#endif
};

    
/**
 * @brief Represents a positional argument in the argument parser.
 */
class positional_argument : public argument_interface {
public:
    /// @brief Default constructor (deleted).
    positional_argument() = delete;

    /**
     * @brief Constructs a positional_argument with a given name.
     * @param name The primary name of the positional argument.
     */
    positional_argument(const std::string_view name) : _name(name) {}

    /// @brief Constructs a positional_argument with both primary and short names.
    positional_argument(const std::string_view name, const std::string_view short_name)
        : _name(name, short_name) {}

    /// @brief Positional argument destructor.
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
     * @brief Sets the help message for the positional argument.
     * @param help_msg Help message.
     * @return Reference to the positional_argument.
     */
    inline positional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Sets whether the positional argument is required.
     * @param required True if the positional argument is required.
     * @return Reference to the positional_argument class.
     */
    inline positional_argument& required(bool required) override {
        this->_required = required;
        return *this;
    }

    /**
     * @brief Sets the default value for the positional argument.
     * @param default_value Default value.
     * @return Reference to the positional_argument class.
     */
    inline positional_argument& default_value(const std::any& default_value) override {
        this->_default_value = default_value;
        return *this;
    }

private:
    /**
     * @brief Sets the value for the positional argument.
     * @param value The value to set.
     * @return Reference to the positional_argument.
     */
    inline positional_argument& value(const std::any& value) override {
        this->_value = value;
        return *this;
    }

    //! @copydoc argument_interface::is_positional
    [[nodiscard]] inline bool is_positional() const override {
        return not this->_optional;
    }

    //! @copydoc argument_interface::is_optional
    [[nodiscard]] inline bool is_optional() const override {
        return this->_optional;
    }

    //! @copydoc argument_interface::has_value
    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value();
    }

    //! @copydoc argument_interface::value
    [[nodiscard]] inline const std::any& value() const override {
        return this->_value;
    }

    //! @copydoc argument_interface::name
    [[nodiscard]] inline const argument_name& name() const override {
        return this->_name;
    }

    //! @copydoc argument_interface::required
    [[nodiscard]] inline bool required() const override {
        return this->_required;
    }

    //! @copydoc argument_interface::help
    [[nodiscard]] inline const std::optional<std::string_view>& help() const override {
        return this->_help_msg;
    }

    //! @copydoc argument_interface::default_value
    [[nodiscard]] inline const std::any& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = false;                 ///< Indicates if the positional argument is optional.
    const argument_name _name;                    ///< Name (and optional short name) of the positional argument.
    std::any _value;                              ///< Value of the positional argument.
    bool _required = true;                        ///< Indicates if the positional argument is required.
    std::optional<std::string_view> _help_msg;    ///< Optional help message for the positional argument.
    std::any _default_value;                      ///< Default value for the positional argument.

#ifdef AP_TESTING
    friend inline positional_argument&
        testing_argument_set_value(positional_argument&, const std::any&);
#endif
};


/**
 * @brief Represents an optional argument in the argument parser.
 */
class optional_argument : public argument_interface {
public:
    /// @brief Default constructor (deleted).
    optional_argument() = delete;

    /**
     * @brief Constructs an optional_argument with a given name.
     * @param name The primary name of the optional argument.
     */
    optional_argument(std::string_view name) : _name(name) {}

    /**
     * @brief Constructs an optional_argument with both primary and short names.
     * @param name The primary name of the optional argument.
     * @param short_name The short name of the optional argument (optional).
     */
    optional_argument(std::string_view name, std::string_view short_name)
        : _name(name, short_name) {}

    /// @brief Optional argument destructor.
    ~optional_argument() = default;


    /**
     * @brief Equality operator for optional_argument.
     * @param other Another optional_argument for comparison.
     * @return Result of equality
     */
    inline bool operator== (const optional_argument& other) const {
        return this->_name == other._name;
    }

    /**
     * @brief Sets the help message for the optional argument.
     * @param help_msg Help message.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    /**
     * @brief Sets whether the optional argument is required.
     * @param required True if the optional argument is required.
     * @return Reference to the optional_argument class.
     */
    inline optional_argument& required(bool required) override {
        this->_required = required;
        return *this;
    }

    /**
     * @brief Sets the default value for the optional argument.
     * @param default_value Default value.
     * @return Reference to the optional_argument class.
     */
    inline optional_argument& default_value(const std::any& default_value) override {
        this->_default_value = default_value;
        return *this;
    }

private:
    /**
     * @brief Sets the value for the optional argument.
     * @param value The value to set.
     * @return Reference to the optional_argument.
     */
    inline optional_argument& value(const std::any& value) override {
        this->_value = value;
        return *this;
    }

    //! @copydoc argument_interface::is_positional
    [[nodiscard]] inline bool is_positional() const override {
        return not this->_optional;
    }

    //! @copydoc argument_interface::is_optional
    [[nodiscard]] inline bool is_optional() const override {
        return this->_optional;
    }

    //! @copydoc argument_interface::has_value
    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value();
    }

    //! @copydoc argument_interface::value
    [[nodiscard]] inline const std::any& value() const override {
        return this->_value;
    }

    //! @copydoc argument_interface::name
    [[nodiscard]] inline const argument_name& name() const override {
        return this->_name;
    }

    //! @copydoc argument_interface::required
    [[nodiscard]] inline bool required() const override {
        return this->_required;
    }

    //! @copydoc argument_interface::help
    [[nodiscard]] inline const std::optional<std::string_view>& help() const override {
        return this->_help_msg;
    }

    //! @copydoc argument_interface::default_value
    [[nodiscard]] inline const std::any& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = true;                ///< Indicates if the optional argument is optional.
    const argument_name _name;                  ///< Name (and optional short name) of the optional argument.
    std::any _value;                            ///< Value of the optional argument.
    bool _required = false;                     ///< Indicates if the optional argument is required.
    std::optional<std::string_view> _help_msg;  ///< Optional help message for the optional argument.
    std::any _default_value;                    ///< Default value for the optional argument.

#ifdef AP_TESTING
    friend inline optional_argument&
        testing_argument_set_value(optional_argument&, const std::any&);
#endif
};

} // namespace detail


/**
 * @brief Main class for argument parsing.
 */
class argument_parser {
public:
    /// @brief Default constructor for argument_parser.
    argument_parser() = default;

    /// @brief Deleted copy constructor for argument_parser to prevent unintended copies.
    argument_parser(const argument_parser&) = delete;

    /// @brief Deleted move constructor for argument_parser to prevent unintended moves.
    argument_parser(argument_parser&&) = delete;

    /// @brief Deleted copy assignment operator for argument_parser to prevent unintended copies.
    /// @return Reference to the argument_parser class.
    argument_parser& operator= (const argument_parser&) = delete;

    /// @brief Default destructor for argument_parser.
    ~argument_parser() = default;

    /**
     * @brief Sets the program name for the parser.
     * @param name The program name.
     * @return Reference to the argument_parser.
     */
    inline argument_parser& program_name(const std::string_view name) {
        this->_program_name = name;
        return *this;
    }

    /**
     * @brief Sets the program description for the parser.
     * @param description The program description.
     * @return Reference to the argument_parser.
     */
    inline argument_parser& program_description(const std::string_view description) {
        this->_program_description = description;
        return *this;
    }

    /**
     * @brief Overloaded output stream operator for argument_parser.
     * @param os Output stream.
     * @param parser Argument parser to be output.
     * @return The modified output stream.
     */
    friend std::ostream& operator<< (std::ostream& os, const argument_parser& parser) {
        if (parser._program_name)
            os << parser._program_name.value() << std::endl;

        if (parser._program_description)
            os << parser._program_description.value() << std::endl;

        return os;
    }

private:
    std::optional<std::string_view> _program_name;         ///< Optional program name.
    std::optional<std::string_view> _program_description;  ///< Optional program description.
};

} // namespace ap
