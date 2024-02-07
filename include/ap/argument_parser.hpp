/*
[cpp-ap] Command-line argument parser for C++20

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



namespace ap {

class argument_parser;



namespace utility {

template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };

template <typename T>
concept valid_argument_value_type =
    readable<T> and
    std::copy_constructible<T> and std::assignable_from<T&, const T&>;

template <typename T>
concept equality_comparable = requires(T lhs, T rhs) {
    { lhs == rhs } -> std::convertible_to<bool>;
};

template <typename T, typename... ValidTypes>
inline constexpr bool is_valid_type_v = std::disjunction_v<std::is_same<T, ValidTypes>...>;

} // namespace utility



namespace nargs {

class range {
public:
    using count_type = std::size_t;

    range() : _nlow(_ndefault), _nhigh(_ndefault) {}

    range(const count_type n)
    : _nlow(n), _nhigh(n), _default(n == _ndefault) {}

    range(const count_type nlow, const count_type nhigh)
    : _nlow(nlow), _nhigh(nhigh), _default(nlow == _ndefault and nhigh == _ndefault) {}

    range& operator=(const range&) = default;

    ~range() = default;

    [[nodiscard]] inline bool is_default() const {
        return this->_default;
    }

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

    friend range at_least(const count_type);
    friend range more_than(const count_type);
    friend range less_than(const count_type);
    friend range up_to(const count_type);
    friend range any();

private:
    range(const std::optional<count_type> nlow, const std::optional<count_type> nhigh)
        : _nlow(nlow), _nhigh(nhigh) {}

    std::optional<count_type> _nlow;
    std::optional<count_type> _nhigh;
    bool _default = true;

    static constexpr count_type _ndefault = 1;
};

[[nodiscard]] inline range at_least(const range::count_type n) {
    return range(n, std::nullopt);
}

[[nodiscard]] inline range more_than(const range::count_type n) {
    return range(n + 1, std::nullopt);
}

[[nodiscard]] inline range less_than(const range::count_type n) {
    return range(std::nullopt, n - 1);
}

[[nodiscard]] inline range up_to(const range::count_type n) {
    return range(std::nullopt, n);
}

[[nodiscard]] inline range any() {
    return range(std::nullopt, std::nullopt);
}

} // namespace nargs



struct valued_action {
    template <ap::utility::valid_argument_value_type T>
    using type = std::function<T(const T&)>;
};

struct void_action {
    template <ap::utility::valid_argument_value_type T>
    using type = std::function<void(T&)>;
};

namespace action {

namespace detail {

template <typename AS>
concept valid_action_specifier = ap::utility::is_valid_type_v<AS, ap::valued_action, ap::void_action>;

template <valid_action_specifier AS, ap::utility::valid_argument_value_type T>
using callable_type = typename AS::template type<T>;

template <ap::utility::valid_argument_value_type T>
using action_variant_type =
    std::variant<callable_type<ap::valued_action, T>, callable_type<ap::void_action, T>>;

template <ap::utility::valid_argument_value_type T>
[[nodiscard]] inline bool is_void_action(const action_variant_type<T>& action) {
    return std::holds_alternative<callable_type<ap::void_action, T>>(action);
}

} // namespace detail

template <ap::utility::valid_argument_value_type T>
detail::callable_type<ap::void_action, T> default_action{ [](T&) {} };

inline detail::callable_type<ap::void_action, std::string> check_file_exists_action{
    [](std::string& file_path) {
        if (not std::filesystem::exists(file_path)) {
            std::cerr << "[ERROR] : File " + file_path + " does not exists!";
            std::exit(EXIT_FAILURE);
        }
    }
};

} // namespace action



namespace argument::detail {

// TODO: allow for a variadic number of names
struct argument_name {
    argument_name() = delete;

    argument_name& operator=(const argument_name&) = delete;

    argument_name(const argument_name&) = default;

    argument_name(argument_name&&) = default;

    explicit argument_name(std::string_view name) : name(name) {}

    explicit argument_name(std::string_view name, std::string_view short_name)
    : name(name), short_name(short_name) {}

    ~argument_name() = default;

    inline bool operator==(const argument_name& other) const {
        return this->name == other.name;
    }

    inline bool operator==(std::string_view name) const {
        return name == this->name or
               (this->short_name and name == this->short_name.value());
    }

    [[nodiscard]] inline std::string str() const {
        return this->short_name ? ("[" + this->name + "," + this->short_name.value() + "]")
                                : ("[" + this->name + "]");
    }

    friend std::ostream& operator<<(std::ostream& os, const argument_name& arg_name) {
        os << arg_name.str();
        return os;
    }

    // TODO: rename
    // * name -> primary
    // * short_name -> secondary
    const std::string name;
    const std::optional<std::string> short_name;
};

class argument_interface {
public:
    virtual ~argument_interface() = default;

    virtual bool is_optional() const = 0;
    virtual argument_interface& help(std::string_view) = 0;


    friend std::ostream& operator<<(std::ostream& os, const argument_interface& argument) {
        os << argument.name() << " : ";
        const auto& argument_help_msg = argument.help();
        os << (argument_help_msg ? argument_help_msg.value() : "[ostream(argument)] TODO: msg");
        return os;
    }

    friend class ::ap::argument_parser;

protected:
    virtual const argument_name& name() const = 0;
    virtual bool is_required() const = 0;

    virtual bool bypass_required_enabled() const = 0;
    virtual const std::optional<std::string>& help() const = 0;

    virtual void set_used() = 0;
    virtual bool is_used() const = 0;
    virtual std::size_t nused() const = 0;

    virtual argument_interface& set_value(const std::string&) = 0;
    virtual bool has_value() const = 0;
    virtual bool has_parsed_values() const = 0;
    virtual std::weak_ordering nvalues_in_range() const = 0;

    virtual const std::any& value() const = 0;
    virtual const std::vector<std::any>& values() const = 0;
};

} // namespace argument::detail



class argument_parser_error : public std::runtime_error {
public:
    explicit argument_parser_error(const std::string& message)
    : std::runtime_error(message) {}
};

namespace error {

class value_already_set_error : public argument_parser_error {
public:
    explicit value_already_set_error(const argument::detail::argument_name& arg_name)
    : argument_parser_error("Value for argument " + arg_name.str() + " has already been set") {}
};

class invalid_value_error : public argument_parser_error{
public:
    explicit invalid_value_error(
        const argument::detail::argument_name& arg_name, const std::string& value
    ) : argument_parser_error(
        "Cannot parse value `" + value + "` for argument " + arg_name.str()) {}
};

class invalid_choice_error : public argument_parser_error {
public:
    explicit invalid_choice_error(
        const argument::detail::argument_name& arg_name, const std::string& value
    ) : argument_parser_error(
        "Value `" + value + "` is not a valid choice for argument " + arg_name.str()) {}
};

class argument_name_used_error : public argument_parser_error {
public:
    explicit argument_name_used_error(const std::string_view& given_arg_name)
    : argument_parser_error("Given name `" + std::string(given_arg_name) + "` already used") {}

    explicit argument_name_used_error(
        const std::string_view& given_arg_name, const std::string_view& given_arg_name_short
    ) : argument_parser_error(
        "Given name " +
        argument::detail::argument_name(given_arg_name, given_arg_name_short).str() + " already used") {}
};

class argument_not_found_error : public argument_parser_error {
public:
    explicit argument_not_found_error(const std::string_view& arg_name)
    : argument_parser_error("Argument with given name `" + std::string(arg_name) + "` not found.") {}
};

class invalid_value_type_error : public argument_parser_error {
public:
    explicit invalid_value_type_error(
        const argument::detail::argument_name& arg_name, const std::type_info& given_arg_type
    ) : argument_parser_error(
        "Invalid value type specified for argument " + arg_name.str() + " - " + given_arg_type.name()) {}
};

class required_argument_not_parsed_error : public argument_parser_error {
public:
    explicit required_argument_not_parsed_error(const argument::detail::argument_name& arg_name)
    : argument_parser_error("No values parsed for a required argument " + arg_name.str()) {}
};

class free_value_error : public argument_parser_error {
public:
    explicit free_value_error(const std::string& value)
    : argument_parser_error("Failed to deduce the argument for the given value `" + value + "`") {}
};

class invalid_nvalues_error : public argument_parser_error {
public:
    [[nodiscard]] static std::string msg(
        const std::weak_ordering ordering, const argument::detail::argument_name& arg_name
    ) {
        if(std::is_lt(ordering))
            return "Too few values provided for optional argument " + arg_name.str();
        else
            return "Too many values provided for optional argument " + arg_name.str();
    }

    explicit invalid_nvalues_error(
        const std::weak_ordering ordering, const argument::detail::argument_name& arg_name
    ) : argument_parser_error(invalid_nvalues_error::msg(ordering, arg_name)) {}
};

} // namespace error



namespace argument {

template <utility::valid_argument_value_type T>
class positional_argument : public detail::argument_interface {
public:
    using value_type = T;

    positional_argument() = delete;

    positional_argument(std::string_view name) : _name(name) {}

    positional_argument(std::string_view name, std::string_view short_name)
        : _name(name, short_name) {}

    ~positional_argument() = default;

    inline bool operator==(const positional_argument& other) const {
        return this->_name == other._name;
    }

    inline positional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    inline positional_argument& choices(const std::vector<value_type>& choices)
    requires(utility::equality_comparable<value_type>) {
        this->_choices = choices;
        return *this;
    }

    template <ap::action::detail::valid_action_specifier AS, std::invocable<value_type&> F>
    inline positional_argument& action(F&& action) {
        using callable_type = ap::action::detail::callable_type<AS, value_type>;
        this->_action = std::forward<callable_type>(action);
        return *this;
    }

    [[nodiscard]] inline bool is_optional() const override { return this->_optional; }

    friend class ::ap::argument_parser;

#ifdef AP_TESTING
    friend struct ::ap_testing::positional_argument_test_fixture;
#endif

private:
    [[nodiscard]] inline const detail::argument_name& name() const override {
        return this->_name;
    }

    [[nodiscard]] inline const std::optional<std::string>& help() const override {
        return this->_help_msg;
    }

    [[nodiscard]] inline bool is_required() const override {
        return this->_required;
    }

    [[nodiscard]] inline bool bypass_required_enabled() const override {
        return this->_bypass_required;
    }

    void set_used() override {}

    [[nodiscard]] inline bool is_used() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] inline std::size_t nused() const override {
        return static_cast<std::size_t>(this->_value.has_value());
    }

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

    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] inline bool has_parsed_values() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] inline std::weak_ordering nvalues_in_range() const override {
        return this->_value.has_value() ? std::weak_ordering::equivalent
                                        : std::weak_ordering::less;
    }

    [[nodiscard]] inline const std::any& value() const override {
        return this->_value;
    }

    [[nodiscard]] inline const std::vector<std::any>& values() const override {
        throw std::logic_error("Positional argument " + this->_name.name + "has only 1 value.");
    }

    [[nodiscard]] inline bool _is_valid_choice(const value_type& choice) const {
        return this->_choices.empty() or
               std::find(this->_choices.begin(), this->_choices.end(), choice) != this->_choices.end();
    }

    void _apply_action(value_type& value) const {
        namespace action = ap::action::detail;
        if (action::is_void_action(this->_action))
            std::get<action::callable_type<ap::void_action, value_type>>(this->_action)(value);
        else
            value = std::get<action::callable_type<ap::valued_action, value_type>>(this->_action)(value);
    }

    using action_type = ap::action::detail::action_variant_type<T>;

    static constexpr bool _optional = false;
    const detail::argument_name _name;
    std::optional<std::string> _help_msg;

    static constexpr bool _required = true;
    static constexpr bool _bypass_required = false;
    std::vector<value_type> _choices;
    action_type _action = ap::action::default_action<value_type>;

    std::any _value;

    std::stringstream _ss;
};

template <utility::valid_argument_value_type T>
class optional_argument : public detail::argument_interface {
public:
    using value_type = T;
    using count_type = ap::nargs::range::count_type;

    optional_argument() = delete;

    optional_argument(std::string_view name) : _name(name) {}

    optional_argument(std::string_view name, std::string_view short_name)
        : _name(name, short_name) {}

    ~optional_argument() = default;

    inline bool operator==(const optional_argument& other) const {
        return this->_name == other._name;
    }

    inline optional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    inline optional_argument& required() {
        this->_required = true;
        return *this;
    }

    inline optional_argument& bypass_required() {
        this->_bypass_required = true;
        return *this;
    }

    inline optional_argument& nargs(const ap::nargs::range& range) {
        this->_nargs_range = range;
        return *this;
    }

    inline optional_argument& nargs(const count_type count) {
        this->_nargs_range = ap::nargs::range(count);
        return *this;
    }

    inline optional_argument& nargs(const count_type nlow, const count_type nhigh) {
        this->_nargs_range = ap::nargs::range(nlow, nhigh);
        return *this;
    }

    template <ap::action::detail::valid_action_specifier AS, std::invocable<value_type&> F>
    inline optional_argument& action(F&& action) {
        using callable_type = ap::action::detail::callable_type<AS, value_type>;
        this->_action = std::forward<callable_type>(action);
        return *this;
    }

    inline optional_argument& choices(const std::vector<value_type>& choices)
    requires(utility::equality_comparable<value_type>) {
        this->_choices = choices;
        return *this;
    }

    optional_argument& default_value(const value_type& default_value) {
        this->_default_value = default_value;
        return *this;
    }

    optional_argument& implicit_value(const value_type& implicit_value) {
        this->_implicit_value = implicit_value;
        return *this;
    }

    [[nodiscard]] inline bool is_optional() const override { return this->_optional; }

    friend class ::ap::argument_parser;

#ifdef AP_TESTING
    friend struct ::ap_testing::optional_argument_test_fixture;
#endif

private:
    [[nodiscard]] inline const detail::argument_name& name() const override {
        return this->_name;
    }

    [[nodiscard]] inline const std::optional<std::string>& help() const override {
        return this->_help_msg;
    }

    [[nodiscard]] inline bool is_required() const override {
        return this->_required;
    }

    [[nodiscard]] inline bool bypass_required_enabled() const override {
        return this->_bypass_required;
    }

    void set_used() override {
        this->_nused++;
    }

    [[nodiscard]] inline bool is_used() const override {
        return this->_nused > 0;
    }

    [[nodiscard]] inline std::size_t nused() const override {
        return this->_nused;
    }

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

    [[nodiscard]] inline bool has_value() const override {
        return this->has_parsed_values() or this->_has_predefined_value();
    }

    [[nodiscard]] inline bool has_parsed_values() const override {
        return not this->_values.empty();
    }

    [[nodiscard]] std::weak_ordering nvalues_in_range() const override {
        if (not this->_nargs_range)
            return std::weak_ordering::equivalent;

        if (this->_values.empty() and this->_has_predefined_value())
            return std::weak_ordering::equivalent;

        return this->_nargs_range->contains(this->_values.size());
    }

    [[nodiscard]] inline const std::any& value() const override {
        return this->_values.empty() ? this->_predefined_value() : this->_values.front();
    }

    [[nodiscard]] inline const std::vector<std::any>& values() const override {
        return this->_values;
    }

    [[nodiscard]] inline bool _has_predefined_value() const {
        return this->_default_value.has_value() or
               (this->is_used() and this->_implicit_value.has_value());
    }

    [[nodiscard]] inline const std::any& _predefined_value() const {
        return this->is_used() ? this->_implicit_value : this->_default_value;
    }

    [[nodiscard]] inline bool _is_valid_choice(const value_type& choice) const {
        return this->_choices.empty() or
               std::find(this->_choices.begin(), this->_choices.end(), choice) != this->_choices.end();
    }

    void _apply_action(value_type& value) const {
        namespace action = ap::action::detail;
        if (action::is_void_action(this->_action))
            std::get<action::callable_type<ap::void_action, value_type>>(this->_action)(value);
        else
            value = std::get<action::callable_type<ap::valued_action, value_type>>(this->_action)(value);
    }

    using action_type = ap::action::detail::action_variant_type<T>;

    static constexpr bool _optional = true;
    const detail::argument_name _name;
    std::optional<std::string> _help_msg;

    bool _required = false;
    bool _bypass_required = false;
    std::optional<ap::nargs::range> _nargs_range;
    action_type _action = ap::action::default_action<value_type>;
    std::vector<value_type> _choices;
    std::any _default_value;
    std::any _implicit_value;

    std::size_t _nused = 0u;
    std::vector<std::any> _values;

    std::stringstream _ss;
};

} // namespace argument



namespace default_argument {

enum class positional : uint8_t {
    input,
    output
};

enum class optional : uint8_t {
    help,
    input,
    output,
    multi_input,
    multi_output
};

} // namespace default_argument



class argument_parser {
public:
    argument_parser() = default;

    argument_parser(const argument_parser&) = delete;
    argument_parser(argument_parser&&) = delete;
    argument_parser& operator=(const argument_parser&) = delete;

    ~argument_parser() = default;

    inline argument_parser& program_name(std::string_view name) {
        this->_program_name = name;
        return *this;
    }

    inline argument_parser& program_description(std::string_view description) {
        this->_program_description = description;
        return *this;
    }

    inline argument_parser& default_positional_arguments(
        const std::vector<default_argument::positional>& args
    ) {
        for (const auto arg : args)
            this->_add_default_positional_argument(arg);
        return *this;
    }

    inline argument_parser& default_optional_arguments(
        const std::vector<default_argument::optional>& args
    ) {
        for (const auto arg : args)
            this->_add_default_optional_argument(arg);
        return *this;
    }

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

    template <utility::valid_argument_value_type T = std::string>
    argument::optional_argument<T>& add_optional_argument(std::string_view name) {
        // TODO: check forbidden characters

        if (this->_is_arg_name_used(name))
            throw error::argument_name_used_error(name);

        this->_optional_args.push_back(std::make_unique<argument::optional_argument<T>>(name));
        return static_cast<argument::optional_argument<T>&>(*this->_optional_args.back());
    }

    template <utility::valid_argument_value_type T = std::string>
    argument::optional_argument<T>& add_optional_argument(std::string_view name, std::string_view short_name) {
        // TODO: check forbidden characters

        if (this->_is_arg_name_used(name, short_name))
            throw error::argument_name_used_error(name, short_name);

        this->_optional_args.push_back(
            std::make_unique<argument::optional_argument<T>>(name, short_name));
        return static_cast<argument::optional_argument<T>&>(*this->_optional_args.back());
    }

    template <bool StoreImplicitly = true>
    argument::optional_argument<bool>& add_flag(std::string_view name) {
        return this->add_optional_argument<bool>(name)
                    .default_value(not StoreImplicitly)
                    .implicit_value(StoreImplicitly)
                    .nargs(0);
    }

    template <bool StoreImplicitly = true>
    argument::optional_argument<bool>& add_flag(
        std::string_view name, std::string_view short_name
    ) {
        return this->add_optional_argument<bool>(name, short_name)
                    .default_value(not StoreImplicitly)
                    .implicit_value(StoreImplicitly)
                    .nargs(0);
    }

    void parse_args(int argc, char* argv[]) {
        // TODO: check for nullptr
        this->_parse_args_impl(this->_preprocess_input(argc, argv));

        if (this->_bypass_required_args())
            return;

        this->_check_required_args();
        this->_check_nvalues_in_range();
    }

    bool has_value(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().has_value() : false;
    }

    std::size_t count(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().nused() : 0u;
    }

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

    friend std::ostream& operator<<(std::ostream& os, const argument_parser& parser) {
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
    friend struct ::ap_testing::argument_parser_test_fixture;
#endif

private:
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

    struct cmd_argument {
        enum class type_discriminator : bool { flag, value };

        cmd_argument() = default;
        cmd_argument(const cmd_argument&) = default;
        cmd_argument(cmd_argument&&) = default;
        cmd_argument& operator=(const cmd_argument&) = default;

        cmd_argument(
            const type_discriminator discriminator, const std::string& value
        ) : discriminator(discriminator), value(value) {}

        ~cmd_argument() = default;

        inline bool operator==(const cmd_argument& other) const {
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

    [[nodiscard]] inline argument_predicate_type _name_eq_predicate(
        const std::string_view& name
    ) const {
        return [&name](const argument_ptr_type& arg) {
            return name == arg->name();
        };
    }

    [[nodiscard]] inline argument_predicate_type _name_eq_predicate(
        const std::string_view& name, const std::string_view& short_name
    ) const {
        return [&name, &short_name](const argument_ptr_type& arg) {
            return name == arg->name() or short_name == arg->name();
        };
    }

    [[nodiscard]] bool _is_arg_name_used(const std::string_view& name) const {
        const auto predicate = this->_name_eq_predicate(name);

        if (this->_const_find_positional(predicate) != this->_positional_args.end())
            return true;

        if (this->_const_find_optional(predicate) != this->_optional_args.end())
            return true;

        return false;
    }

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

    [[nodiscard]] cmd_argument_list _preprocess_input(int argc, char* argv[]) const {
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

    [[nodiscard]] bool _is_flag(const std::string& arg) const {
        if (arg.starts_with(this->_flag_prefix))
            return this->_is_arg_name_used(arg.substr(this->_flag_prefix_length));

        if (arg.starts_with(this->_flag_prefix_char))
            return this->_is_arg_name_used(arg.substr(this->_flag_prefix_char_length));

        return false;
    }

    void _strip_flag_prefix(std::string& arg) const {
        if (arg.starts_with(this->_flag_prefix))
            arg.erase(0, this->_flag_prefix_length);
        else
            arg.erase(0, this->_flag_prefix_char_length);
    }

    void _parse_args_impl(const cmd_argument_list& cmd_args) {
        cmd_argument_list_iterator cmd_it = cmd_args.begin();
        this->_parse_positional_args(cmd_args, cmd_it);
        this->_parse_optional_args(cmd_args, cmd_it);
    }

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

    [[nodiscard]] inline bool _bypass_required_args() const {
        return std::any_of(
            std::cbegin(this->_optional_args), std::cend(this->_optional_args),
            [](const argument_ptr_type& arg) {
                return arg->is_used() and arg->bypass_required_enabled();
            }
        );
    }

    void _check_required_args() const {
        for (const auto& arg : this->_positional_args)
            if (not arg->is_used())
                throw error::required_argument_not_parsed_error(arg->name());

        for (const auto& arg : this->_optional_args)
            if (arg->is_required() and not arg->has_value())
                throw error::required_argument_not_parsed_error(arg->name());
    }

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

    [[nodiscard]] inline argument_list_iterator_type _find_positional(
        const argument_predicate_type& predicate
    ) {
        return std::find_if(
            std::begin(this->_positional_args), std::end(this->_positional_args), predicate);
    }

    [[nodiscard]] inline argument_list_iterator_type _find_optional(
        const argument_predicate_type& predicate
    ) {
        return std::find_if(
            std::begin(this->_optional_args), std::end(this->_optional_args), predicate);
    }

    [[nodiscard]] inline argument_list_const_iterator_type _const_find_positional(
        const argument_predicate_type& predicate
    ) const {
        return std::find_if(
            std::cbegin(this->_positional_args), std::cend(this->_positional_args), predicate);
    }

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

    static constexpr uint8_t _flag_prefix_char_length = 1u;
    static constexpr uint8_t _flag_prefix_length = 2u;
    static constexpr char _flag_prefix_char = '-';
    const std::string _flag_prefix = "--"; // not static constexpr because of g++-12 on ubuntu :(
};

} // namespace ap
