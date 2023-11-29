#pragma once

#include <algorithm>
#include <any>
#include <concepts>
#include <iostream>
#include <memory>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>


namespace ap {

class argument_parser;

namespace detail {

template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };

template <typename T, typename... ValidTypes>
struct is_valid_type : std::disjunction<std::is_same<T, ValidTypes>...> {};

template <typename T, typename... ValidTypes>
inline constexpr bool is_valid_type_v = is_valid_type<T, ValidTypes...>::value;


template <readable T>
bool holds_type(const std::any& any) {
    return any.type() == typeid(T);
}



struct argument_name {
    argument_name() = delete;

    argument_name(const argument_name&) = default;
    argument_name(argument_name&&) = default;
    argument_name& operator= (const argument_name&) = default;

    explicit argument_name(const std::string_view name) : name(name) {}
    explicit argument_name(const std::string_view name, const std::string_view short_name)
        : name(name), short_name(short_name) {}

    ~argument_name() = default;

    inline bool operator== (const argument_name& other) const {
        return this->name == other.name;
    }

    inline bool operator== (const std::string_view name) const {
        return name == this->name or
               (this->short_name and name == this->short_name.value());
    }

    [[nodiscard]] inline std::string str() const {
        return this->short_name ? "[" + std::string(this->name) + "]"
                                : "[" + std::string(this->name) + "," +
                                      std::string(this->short_name.value()) + "]";
    }

    friend std::ostream& operator<< (std::ostream& os, const argument_name& arg_name) {
        // TODO: use str()
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
    using value_type = void;

    virtual argument_interface& help(std::string_view) = 0;
    virtual argument_interface& required(bool) = 0;
    virtual argument_interface& default_value(const std::any&) = 0;

    virtual ~argument_interface() = default;

    friend class ::ap::argument_parser;
    friend std::ostream& operator<< (std::ostream& os, const argument_interface& argument) {
        os << argument.name() << " : ";

        const auto& argument_help_msg = argument.help();
        os << (argument_help_msg ? argument_help_msg.value() : "[ostream(argument)] TODO: msg");

        return os;
    }

protected:
    virtual bool is_optional() const = 0;

    virtual argument_interface& value(const std::any&) = 0;
    virtual bool has_value() const = 0;
    virtual const std::any& value() const = 0;

    virtual const argument_name& name() const = 0;
    virtual bool required() const = 0;
    virtual const std::optional<std::string_view>& help() const = 0;
    virtual const std::any& default_value() const = 0;

// TODO PWRS5PZ-24: replace with a friend testing fixuture class
#ifdef AP_TESTING
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


template <readable T>
class positional_argument : public argument_interface {
public:
    using value_type = T;

    positional_argument() = delete;

    positional_argument(const std::string_view name) : _name(name) {}

    positional_argument(const std::string_view name, const std::string_view short_name)
        : _name(name, short_name) {}

    ~positional_argument() = default;

    inline bool operator== (const positional_argument& other) const {
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
        if (not holds_type<value_type>(default_value))
            throw std::invalid_argument("[default_value] TODO: msg");

        this->_default_value = default_value;
        return *this;
    }

    friend class ::ap::argument_parser;

private:
    [[nodiscard]] bool is_optional() const {
        return false;
    }

    inline positional_argument& value(const std::any& value) override {
        if (not holds_type<value_type>(value))
            throw std::invalid_argument("[value] TODO: msg");

        this->_value = value;
        return *this;
    }

    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] inline const std::any& value() const override {
        return this->_value;
    }

    [[nodiscard]] inline const argument_name& name() const override {
        return this->_name;
    }

    [[nodiscard]] inline bool required() const override {
        return this->_required;
    }

    [[nodiscard]] inline const std::optional<std::string_view>& help() const override {
        return this->_help_msg;
    }

    [[nodiscard]] inline const std::any& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = false;
    const argument_name _name;

    std::any _value;

    bool _required = true;
    std::optional<std::string_view> _help_msg;
    std::any _default_value;

// TODO PWRS5PZ-24: replace with a friend testing fixuture class
// #ifdef AP_TESTING
//     friend inline positional_argument&
//         testing_argument_set_value(positional_argument&, const std::any&);
// #endif
};


template <readable T>
class optional_argument : public argument_interface {
public:
    using value_type = T;

    optional_argument() = delete;

    optional_argument(std::string_view name) : _name(name) {}

    optional_argument(std::string_view name, std::string_view short_name)
        : _name(name, short_name) {}

    ~optional_argument() = default;

    inline bool operator== (const optional_argument& other) const {
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
        if (not holds_type<value_type>(default_value))
            throw std::invalid_argument("[default_value] TODO: msg");

        this->_default_value = default_value;
        return *this;
    }

    friend class ::ap::argument_parser;

private:
    [[nodiscard]] bool is_optional() const {
        return true;
    }

    inline optional_argument& value(const std::any& value) override {
        if (not holds_type<value_type>(value))
            throw std::invalid_argument("[value] TODO: msg");

        this->_value = value;
        return *this;
    }

    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] inline const std::any& value() const override {
        return this->_value;
    }

    [[nodiscard]] inline const argument_name& name() const override {
        return this->_name;
    }

    [[nodiscard]] inline bool required() const override {
        return this->_required;
    }

    [[nodiscard]] inline const std::optional<std::string_view>& help() const override {
        return this->_help_msg;
    }

    [[nodiscard]] inline const std::any& default_value() const override {
        return this->_default_value;
    }

    const bool _optional = true;
    const argument_name _name;

    std::any _value;

    bool _required = false;
    std::optional<std::string_view> _help_msg;
    std::any _default_value;

// TODO PWRS5PZ-24: replace with a friend testing fixuture class
// #ifdef AP_TESTING
//     friend inline optional_argument&
//         testing_argument_set_value(optional_argument&, const std::any&);
// #endif
};


template <typename A>
concept derived_from_argument_interface =
    std::derived_from<std::remove_cvref_t<A>, argument_interface>;

template <derived_from_argument_interface A, readable T>
inline constexpr bool is_positional() {
    return std::is_same_v<std::remove_cvref_t<A>, positional_argument<T>>;
}

template <derived_from_argument_interface A, readable T>
inline constexpr bool is_optional() {
    return std::is_same_v<std::remove_cvref_t<A>, optional_argument<T>>;
}

} // namespace detail



class argument_parser {
public:
    argument_parser() = default;

    argument_parser(const argument_parser&) = delete;
    argument_parser(argument_parser&&) = delete;
    argument_parser& operator= (const argument_parser&) = delete;

    ~argument_parser() = default;

    inline argument_parser& program_name(const std::string_view name) {
        this->_program_name = name;
        return *this;
    }

    inline argument_parser& program_description(const std::string_view description) {
        this->_program_description = description;
        return *this;
    }

    template <detail::readable T = std::string>
    detail::argument_interface& add_positional_argument(const std::string_view name) {
        this->_check_arg_name_present(name);
        this->_positional_args.push_back(std::make_unique<detail::positional_argument<T>>(name));
        return *this->_positional_args.back();
    }

    template <detail::readable T = std::string>
    detail::argument_interface& add_positional_argument(
        const std::string_view name, const std::string_view short_name
    ) {
        this->_check_arg_name_present(name);
        this->_check_arg_name_present(short_name);
        this->_positional_args.push_back(std::make_unique<detail::positional_argument<T>>(name, short_name));
        return *this->_positional_args.back();
    }

    template <detail::readable T = std::string>
    detail::argument_interface& add_optional_argument(const std::string_view name) {
        this->_check_arg_name_present(name);
        this->_optional_args.push_back(std::make_unique<detail::optional_argument<T>>(name));
        return *this->_optional_args.back();
    }

    template <detail::readable T = std::string>
    detail::argument_interface& add_optional_argument(
        const std::string_view name, const std::string_view short_name
    ) {
        this->_check_arg_name_present(name);
        this->_check_arg_name_present(short_name);
        this->_optional_args.push_back(std::make_unique<detail::optional_argument<T>>(name, short_name));
        return *this->_optional_args.back();
    }

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

private:
    void _check_arg_name_present(const std::string_view name) const {
        const auto name_eq_predicate = [name](const std::unique_ptr<detail::argument_interface>& arg) {
            return name == arg->name();
        };

        if (std::find_if(
                this->_positional_args.begin(), this->_positional_args.end(), name_eq_predicate
            ) != this->_positional_args.end()) {
            throw std::invalid_argument("[_check_arg_name_present(n)] TODO: msg");
        }

        if (std::find_if(
                this->_optional_args.begin(), this->_optional_args.end(), name_eq_predicate
            ) != this->_optional_args.end()) {
            throw std::invalid_argument("[_check_arg_name_present(n, s)] TODO: msg");
        }
    }

    std::optional<std::string_view> _program_name;
    std::optional<std::string_view> _program_description;

    std::vector<std::unique_ptr<detail::argument_interface>> _positional_args;
    std::vector<std::unique_ptr<detail::argument_interface>> _optional_args;
};

} // namespace ap
