#pragma once

#include <algorithm>
#include <any>
#include <concepts>
#include <iostream>
#include <memory>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <vector>

#ifdef AP_TESTING

namespace ap_testing {
struct argument_test_fixture;
struct argument_parser_test_fixture;
} // namespace ap_testing

#endif

namespace ap {

class argument_parser;

namespace detail {

template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };

struct argument_name {
    argument_name() = delete;

    argument_name(const argument_name&) = default;
    argument_name(argument_name&&) = default;
    argument_name& operator= (const argument_name&) = default;

    explicit argument_name(std::string_view name) : name(name) {}
    explicit argument_name(std::string_view name, std::string_view short_name)
        : name(name), short_name(short_name) {}

    ~argument_name() = default;

    inline bool operator== (const argument_name& other) const {
        return this->name == other.name;
    }

    inline bool operator== (std::string_view name) const {
        return name == this->name
            or (this->short_name and name == this->short_name.value());
    }

    [[nodiscard]] inline std::string str() const {
        return this->short_name
                 ? "[" + std::string(this->name) + "]"
                 : "[" + std::string(this->name) + ","
                       + std::string(this->short_name.value()) + "]";
    }

    friend std::ostream& operator<< (std::ostream& os, const argument_name& arg_name) {
        // TODO: use str()
        os << "[" << arg_name.name;
        if (arg_name.short_name)
            os << "," << arg_name.short_name.value();
        os << "]";
        return os;
    }

    const std::string name;
    const std::optional<std::string> short_name;
};

class argument_interface {
public:
    using value_type = void;

    virtual argument_interface& help(std::string_view) = 0;
    virtual argument_interface& required() = 0;
    virtual argument_interface& default_value(const std::any&) = 0;

    virtual bool is_optional() const = 0;

    virtual ~argument_interface() = default;

    friend std::ostream& operator<< (std::ostream& os, const argument_interface& argument) {
        os << argument.name() << " : ";

        const auto& argument_help_msg = argument.help();
        os << (argument_help_msg ? argument_help_msg.value() : "[ostream(argument)] TODO: msg");

        return os;
    }

    friend class ::ap::argument_parser;

#ifdef AP_TESTING
    friend struct ::ap_testing::argument_test_fixture;
#endif

protected:
    virtual argument_interface& value(const std::string&) = 0;
    virtual bool has_value() const = 0;
    virtual const std::any& value() const = 0;

    virtual const argument_name& name() const = 0;
    virtual bool is_required() const = 0;
    virtual const std::optional<std::string_view>& help() const = 0;
    virtual const std::any& default_value() const = 0;
};

template <readable T>
class positional_argument : public argument_interface {
public:
    using value_type = T;

    positional_argument() = delete;

    positional_argument(std::string_view name) : _name(name) {}

    positional_argument(std::string_view name, std::string_view short_name)
        : _name(name, short_name) {}

    ~positional_argument() = default;

    inline bool operator== (const positional_argument& other) const {
        return this->_name == other._name;
    }

    inline positional_argument& help(std::string_view help_msg) override {
        this->_help_msg = help_msg;
        return *this;
    }

    inline positional_argument& required() override {
        // TODO: log a warning + add warning tests
        return *this;
    }

    positional_argument& default_value(const std::any&) override {
        // TODO: log a warning + add warning tests
        return *this;
    }

    [[nodiscard]] bool is_optional() const { return this->_optional; }

    friend class ::ap::argument_parser;

private:
    // TODO: add tests for value throwing in test_positional_argument
    positional_argument& value(const std::string& str_value) override {
        this->_ss.clear();
        this->_ss.str(str_value);

        value_type value;
        if (not (this->_ss >> value))
            throw std::invalid_argument("[value] TODO: msg");

        this->_value = value;
        return *this;
    }

    // TODO: add test cases checking has_value() with default_value set
    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value() or this->_default_value.has_value();
    }

    // TODO: add test cases checking value() with default_value set
    [[nodiscard]] inline const std::any& value() const override {
        return this->_value;
    }

    [[nodiscard]] inline const argument_name& name() const override {
        return this->_name;
    }

    [[nodiscard]] inline bool is_required() const override {
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

    const bool _required = true;
    std::optional<std::string_view> _help_msg;
    const std::any _default_value;

    std::stringstream _ss;
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

    inline optional_argument& required() {
        this->_required = true;
        return *this;
    }

    [[nodiscard]] bool is_optional() const { return this->_optional; }

    // TODO: add tests for default_value throwing in test_optional_argument
    optional_argument& default_value(const std::any& default_value) {
        try {
            this->_default_value = std::any_cast<value_type>(default_value);
        }
        catch (const std::bad_any_cast& err) {
            throw std::invalid_argument("[default_value] TODO: msg");
        }

        return *this;
    }

    friend class ::ap::argument_parser;

private:
    // TODO: add tests for value throwing in test_optional_argument
    optional_argument& value(const std::string& str_value) override {
        this->_ss.clear();
        this->_ss.str(str_value);

        value_type value;
        if (not (this->_ss >> value))
            throw std::invalid_argument("[value] TODO: msg");

        this->_value = value;
        return *this;
    }
    [[nodiscard]] inline bool has_value() const override {
        return this->_value.has_value();
    }

    [[nodiscard]] inline const std::any& value() const override {
        return this->_value.has_value() ? this->_value : this->_default_value;
    }

    [[nodiscard]] inline const argument_name& name() const override {
        return this->_name;
    }

    [[nodiscard]] inline bool is_required() const override {
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

    std::stringstream _ss;
};

} // namespace detail

class argument_parser {
public:
    argument_parser() = default;

    argument_parser(const argument_parser&) = delete;
    argument_parser(argument_parser&&) = delete;
    argument_parser& operator= (const argument_parser&) = delete;

    ~argument_parser() = default;

    inline argument_parser& program_name(std::string_view name) {
        this->_program_name = name;
        return *this;
    }

    inline argument_parser& program_description(std::string_view description) {
        this->_program_description = description;
        return *this;
    }

    template <detail::readable T = std::string>
    detail::argument_interface& add_positional_argument(std::string_view name) {
        // TODO: check forbidden characters
        this->_check_arg_name_present(name);
        this->_positional_args.push_back(
            std::make_unique<detail::positional_argument<T>>(name)
        );
        return *this->_positional_args.back();
    }

    template <detail::readable T = std::string>
    detail::argument_interface&
    add_positional_argument(std::string_view name, std::string_view short_name) {
        // TODO: check forbidden characters
        this->_check_arg_name_present(name);
        this->_check_arg_name_present(short_name);
        this->_positional_args.push_back(
            std::make_unique<detail::positional_argument<T>>(name, short_name)
        );
        return *this->_positional_args.back();
    }

    template <detail::readable T = std::string>
    detail::argument_interface& add_optional_argument(std::string_view name) {
        // TODO: check forbidden characters
        this->_check_arg_name_present(name);
        this->_optional_args.push_back(std::make_unique<detail::optional_argument<T>>(name));
        return *this->_optional_args.back();
    }

    template <detail::readable T = std::string>
    detail::argument_interface&
    add_optional_argument(std::string_view name, std::string_view short_name) {
        // TODO: check forbidden/allowed characters
        this->_check_arg_name_present(name);
        this->_check_arg_name_present(short_name);
        this->_optional_args.push_back(
            std::make_unique<detail::optional_argument<T>>(name, short_name)
        );
        return *this->_optional_args.back();
    }

    void parse_args(int argc, char* argv[]) {
        this->_parse_args_impl(this->_process_input(argc, argv));
        this->_check_required_args();
    }

    bool has_value(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt.value().get().has_value() : false;
    }

    template <std::copy_constructible T = std::string>
    T value(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);

        if (not arg_opt)
            throw std::invalid_argument("[value#1] TODO: msg (no arg found)");

        try {
            T value{ std::any_cast<T>(arg_opt.value().get().value()) };
            return value;
        }
        catch (const std::bad_any_cast& err) {
            throw std::invalid_argument("[value#2] TODO: msg (invalid type)");
        }
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

#ifdef AP_TESTING
    friend struct ::ap_testing::argument_parser_test_fixture;
#endif

private:
    using cmd_argument_list = std::vector<std::string>;
    using cmd_argument_list_iterator = typename cmd_argument_list::const_iterator;

    using argument_ptr_type = std::unique_ptr<detail::argument_interface>;
    using argument_opt_type =
        std::optional<std::reference_wrapper<detail::argument_interface>>;
    using argument_list_type = std::vector<argument_ptr_type>;
    using argument_list_iterator = typename argument_list_type::iterator;

    auto _name_eq_predicate(std::string_view name) const {
        return [name](const argument_ptr_type& arg) {
            return name == arg->name();
        };
    }

    void _check_arg_name_present(std::string_view name) const {
        const auto predicate = this->_name_eq_predicate(name);

        if (std::find_if(this->_positional_args.begin(), this->_positional_args.end(), predicate)
            != this->_positional_args.end()) {
            throw std::invalid_argument("[_check_arg_name_present(n)] TODO: "
                                        "msg");
        }

        if (std::find_if(this->_optional_args.begin(), this->_optional_args.end(), predicate)
            != this->_optional_args.end()) {
            throw std::invalid_argument("[_check_arg_name_present(n, s)] TODO: "
                                        "msg");
        }
    }

    [[nodiscard]] cmd_argument_list _process_input(int argc, char* argv[]) const {
        /*
        TODO:
        * assignment character detection
        * quotes detection
        * negative number detection
        * split into smaller functions
        */

        if (argc < 2)
            return cmd_argument_list{};

        cmd_argument_list args;
        args.reserve(argc - 1);

        for (int i = 1; i < argc; i++) {
            std::string arg = argv[i];

            if (arg.length() > this->_flag_prefix_length
                and arg.starts_with(this->_flag_prefix)) {
                arg.erase(0, this->_flag_prefix_length);
            }
            else if (arg.length() > this->_flag_prefix_char_length and arg.front() == this->_flag_prefix_char) {
                arg.erase(0, this->_flag_prefix_char_length);
            }

            args.emplace_back(arg);
        }

        return args;
    }

    void _parse_args_impl(const cmd_argument_list& cmd_args) {
        cmd_argument_list_iterator cmd_it = cmd_args.begin();

        for (const auto& pos_arg : this->_positional_args) {
            if (cmd_it == cmd_args.end())
                throw std::runtime_error("[_parse_args_impl#1] TODO: msg (not "
                                         "enough values)");

            // TODO: add argument name parsing for positional args
            pos_arg->value(*cmd_it++);
        }

        while (cmd_it != cmd_args.end()) {
            auto opt_arg_it = std::find_if(
                this->_optional_args.begin(),
                this->_optional_args.end(),
                this->_name_eq_predicate(*cmd_it)
            );

            if (opt_arg_it == this->_optional_args.end())
                throw std::runtime_error("[_parse_args_impl#2] TODO: msg "
                                         "(opt_arg not found)");

            if (++cmd_it == cmd_args.end())
                throw std::runtime_error("[_parse_args_impl#3] TODO: msg "
                                         "(can't read opt_arg's value)");

            opt_arg_it->get()->value(*cmd_it++);
        }
    }

    void _check_required_args() const {
        for (const auto& arg : this->_optional_args)
            if (arg->is_required() and not arg->has_value())
                throw std::runtime_error("[_check_required_args] TODO: msg "
                                         "(optional)");
    }

    argument_opt_type _get_argument(std::string_view name) const {
        const auto predicate = this->_name_eq_predicate(name);

        if (auto pos_arg_it = std::find_if(
                this->_positional_args.begin(), this->_positional_args.end(), predicate
            );
            pos_arg_it != this->_positional_args.end()) {
            return std::ref(**pos_arg_it);
        }

        if (auto opt_arg_it = std::find_if(
                this->_optional_args.begin(), this->_optional_args.end(), predicate
            );
            opt_arg_it != this->_optional_args.end()) {
            return std::ref(**opt_arg_it);
        }

        return std::nullopt;
    }

    std::optional<std::string> _program_name;
    std::optional<std::string> _program_description;

    argument_list_type _positional_args;
    argument_list_type _optional_args;

    // TODO: make it modifiable
    static constexpr uint8_t _flag_prefix_char_length = 1;
    static constexpr uint8_t _flag_prefix_length = 2;
    static constexpr char _flag_prefix_char = '-';
    const std::string _flag_prefix = "--";
};

} // namespace ap
