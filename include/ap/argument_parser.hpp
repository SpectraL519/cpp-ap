#pragma once

#include <iostream>
#include <optional>
#include <string_view>


namespace ap {

namespace detail {

template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };



template <readable T>
class argument_interface {
public:
    using value_type = std::remove_reference_t<T>;

    virtual argument_interface& help(std::string_view) = 0;
    virtual argument_interface& required(bool) = 0;
    virtual argument_interface& default_value(const value_type&) = 0;

    virtual ~argument_interface() = default;

protected:
    virtual argument_interface& value(const value_type&) = 0;

    virtual bool is_positional() const = 0;
    virtual bool is_optional() const = 0;

    virtual bool has_value() const = 0;
    virtual const value_type& value() const = 0;

    virtual const std::string_view name() const = 0;
    virtual const std::optional<std::string_view> short_name() const = 0;
    virtual bool required() const = 0;
    virtual const std::optional<std::string_view>& help() const = 0;
    virtual const std::optional<value_type>& default_value() const = 0;

#ifdef AP_TESTING
    template <readable U>
    friend inline bool testing_is_positional(const argument_interface<U>&);

    template <readable U>
    friend inline bool testing_is_optional(const argument_interface<U>&);

    template <readable U>
    friend inline bool testing_has_value(const argument_interface<U>&);

    template <readable U>
    friend inline const typename argument_interface<U>::value_type&
        testing_get_value(const argument_interface<U>&);

    template <readable U>
    friend inline const std::string_view
        testing_get_name(const argument_interface<U>&);

    template <readable U>
    friend inline const std::optional<std::string_view>
        testing_get_short_name(const argument_interface<U>&);

    template <readable U>
    friend inline bool testing_is_required(const argument_interface<U>&);

    template <readable U>
    friend inline const std::optional<std::string_view>&
        testing_get_help(const argument_interface<U>&);

    template <readable U>
    friend inline const std::optional<typename argument_interface<U>::value_type>&
        testing_get_default_value(const argument_interface<U>&);
#endif
};


template <readable T>
class positional_argument : public argument_interface<T> {
public:
    using value_type = typename argument_interface<T>::value_type;

    positional_argument() = delete;

    explicit positional_argument(std::string_view name) : _name(name) {}

    ~positional_argument() = default;

    bool operator== (const positional_argument& other) const {
        return this->_name == other._name;
    }

    inline positional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    inline positional_argument& required(bool required) override {
        this->_required = required;
        return *this;
    }

    inline positional_argument& default_value(const value_type& default_value) override {
        this->_default_value = default_value;
        return *this;
    }

private:
    inline positional_argument& value(const value_type& value) override {
        this->_value = value;
        return *this;
    }

    [[nodiscard]] inline bool is_positional() const override {
        return not this->_optional;
    }

    [[nodiscard]] inline bool is_optional() const override {
        return this->_optional;
    }

    [[nodiscard]] bool has_value() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] const value_type& value() const override {
        return this->_value.value();
    }

    [[nodiscard]] const std::string_view name() const override {
        return this->_name;
    }

    [[nodiscard]] bool required() const override { return this->_required; }

    [[nodiscard]] const std::optional<std::string_view>& help() const override {
        return this->_help_msg;
    }

    [[nodiscard]] const std::optional<value_type>& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = false;
    const std::string_view _name;

    std::optional<value_type> _value;

    bool _required = true;
    std::optional<std::string_view> _help_msg;
    std::optional<value_type> _default_value;

#ifdef AP_TESTING
    template <readable U>
    friend inline positional_argument<U>&
        testing_set_value(positional_argument<U>&, const typename positional_argument<U>::value_type&);
#endif
};


template <readable T>
class optional_argument : public argument_interface<T> {
public:
    using value_type = typename argument_interface<T>::value_type;

    optional_argument() = delete;

    optional_argument(std::string_view long_name)
        : _long_name(long_name) {}

    optional_argument(std::string_view long_name, std::string_view short_name)
        : _long_name(long_name), _short_name(short_name) {} 

    ~optional_argument() = default;

    bool operator==(const optional_argument& other) const {
        return this->_long_name == other._long_name;
    }

    inline optional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    inline optional_argument& required(bool required) override {
        this->_required = required;
        return *this;
    }

    inline optional_argument& default_value(const value_type& default_value) override {
        this->_default_value = default_value;
        return *this;
    }

private:
    inline optional_argument& value(const value_type& value) override {
        this->_value = value;
        return *this;
    }

    [[nodiscard]] inline bool is_positional() const override {
        return not this->_optional;
    }

    [[nodiscard]] inline bool is_optional() const override {
        return this->_optional;
    }

    [[nodiscard]] bool has_value() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] const value_type& value() const override {
        return this->_value.value();
    }

    [[nodiscard]] const std::string_view name() const {
        return this->_long_name;
    }

    [[nodiscard]] const std::optional<std::string_view> short_name() const {
        return this->_short_name;
    }

    [[nodiscard]] bool required() const override { 
        return this->_required; 
    }

    [[nodiscard]] const std::optional<std::string_view>& help() const override {
        return this->_help_msg;
    }

    [[nodiscard]] const std::optional<value_type>& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = true;
    const std::string_view _long_name;
    const std::optional<std::string_view> _short_name;

    std::optional<value_type> _value;

    bool _required = false;
    std::optional<std::string_view> _help_msg;
    std::optional<value_type> _default_value;

#ifdef AP_TESTING
    template <readable U>
    friend inline optional_argument<U>&
        testing_set_value(optional_argument<U>&, const typename optional_argument<U>::value_type&);
#endif
};

} // namespace detail


class argument_parser {
public:
    argument_parser() = default;

    argument_parser(const argument_parser&) = delete;
    argument_parser(argument_parser&&) = delete;
    argument_parser& operator= (const argument_parser&) = delete;

    ~argument_parser() = default;

    argument_parser& program_name(const std::string_view name) {
        this->_program_name = name;
        return *this;
    }

    argument_parser& program_description(const std::string_view description) {
        this->_program_description = description;
        return *this;
    }

    friend std::ostream& operator<< (std::ostream& os, const argument_parser& parser) {
        if (parser._program_name)
            os << parser._program_name.value() << std::endl;

        if (parser._program_description)
            os << parser._program_description.value() << std::endl;

        return os;
    }

private:
    std::optional<std::string_view> _program_name;
    std::optional<std::string_view> _program_description;
};

} // namespace ap
