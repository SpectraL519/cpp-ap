#pragma once

#include <iostream>
#include <optional>
#include <string_view>
#include <any>


namespace ap {

namespace detail {

template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };



struct argument_name {
    argument_name() = delete;
    argument_name(const argument_name&) = delete;
    argument_name(argument_name&&) = delete;
    argument_name& operator=(const argument_name&) = delete;

    explicit argument_name(const std::string_view name) : name(name) {}
    explicit argument_name(const std::string_view name, const std::string_view short_name)
        : name(name), short_name(short_name)
    {}

    ~argument_name() = default;

    inline bool operator==(const argument_name& other) const {
        return this->name == other.name;
    }

    inline bool operator==(const std::string_view name) const {
        return name == this->name or (this->short_name and name == this->short_name.value());
    }

    friend std::ostream& operator<<(std::ostream& os, const argument_name& arg_name) {
        os << "[" << arg_name.name;
        if (arg_name.short_name)
            os << "," << arg_name.short_name.value();
        os << "]";
        return os;
    }

    const std::string_view name;
    const std::optional<std::string_view> short_name;
};



class argument_interface {
public:
    virtual argument_interface& help(std::string_view) = 0;
    virtual argument_interface& required(bool) = 0;
    virtual argument_interface& default_value(const std::any&) = 0;

    virtual ~argument_interface() = default;

protected:
    virtual argument_interface& value(const std::any&) = 0;

    virtual bool is_positional() const = 0;
    virtual bool is_optional() const = 0;

    virtual bool has_value() const = 0;

    virtual const std::any& value() const = 0;

    virtual const std::string_view name() const = 0;
    virtual bool required() const = 0;
    virtual const std::optional<std::string_view>& help() const = 0;
    virtual const std::any& default_value() const = 0;

#ifdef AP_TESTING
    friend inline bool testing_argument_is_positional(const argument_interface&);
    friend inline bool testing_argument_is_optional(const argument_interface&);
    friend inline bool testing_argument_has_value(const argument_interface&);
    friend inline const std::any& testing_argument_get_value(const argument_interface&);
    friend inline const std::string_view testing_argument_get_name(const argument_interface&);
    friend inline bool testing_argument_is_required(const argument_interface&);
    friend inline const std::optional<std::string_view>&
        testing_argument_get_help(const argument_interface&);
    friend inline const std::any&
        testing_argument_get_default_value(const argument_interface&);
#endif
};


class positional_argument : public argument_interface {
public:
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

    inline positional_argument& default_value(const std::any& default_value) override {
        this->_default_value = default_value;
        return *this;
    }

private:
    inline positional_argument& value(const std::any& value) override {
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

    [[nodiscard]] const std::any& value() const override {
        return this->_value;
    }

    [[nodiscard]] const std::string_view name() const override {
        return this->_name;
    }

    [[nodiscard]] bool required() const override { return this->_required; }

    [[nodiscard]] const std::optional<std::string_view>& help() const override {
        return this->_help_msg;
    }

    [[nodiscard]] const std::any& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = false;
    const std::string_view _name;

    std::any _value;

    bool _required = true;
    std::optional<std::string_view> _help_msg;
    std::any _default_value;

#ifdef AP_TESTING
    friend inline positional_argument&
        testing_argument_set_value(positional_argument&, const std::any&);
#endif
};


class optional_argument : public argument_interface {
public:
    optional_argument() = delete;

    optional_argument(std::string_view name)
        : _name(name) {}

    optional_argument(std::string_view name, std::string_view short_name)
        : _name(name), _short_name(short_name) {}

    ~optional_argument() = default;

    bool operator==(const optional_argument& other) const {
        return this->_name == other._name;
    }

    inline optional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    inline optional_argument& required(bool required) override {
        this->_required = required;
        return *this;
    }

    inline optional_argument& default_value(const std::any& default_value) override {
        this->_default_value = default_value;
        return *this;
    }

private:
    inline optional_argument& value(const std::any& value) override {
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

    [[nodiscard]] const std::any& value() const override {
        return this->_value;
    }

    [[nodiscard]] const std::string_view name() const override {
        return this->_name;
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

    [[nodiscard]] const std::any& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = true;
    const std::string_view _name;
    const std::optional<std::string_view> _short_name;

    std::any _value;

    bool _required = false;
    std::optional<std::string_view> _help_msg;
    std::any _default_value;

#ifdef AP_TESTING
    friend inline optional_argument&
        testing_argument_set_value(optional_argument&, const std::any&);
    friend inline const std::optional<std::string_view>
        testing_optional_argument_get_short_name(const optional_argument&);
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
