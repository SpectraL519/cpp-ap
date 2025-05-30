#pragma once

#define AP_TESTING

#include <ap/argument/positional.hpp>

using ap::argument::positional;
using ap::detail::argument_descriptor;
using ap::detail::argument_name;
using ap::detail::c_argument_value_type;

namespace ap_testing {

struct positional_argument_test_fixture {
    positional_argument_test_fixture() = default;
    ~positional_argument_test_fixture() = default;

    template <c_argument_value_type T>
    using value_type = typename positional<T>::value_type;

    template <c_argument_value_type T>
    [[nodiscard]] const argument_name& get_name(const positional<T>& arg) const {
        return arg.name();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& get_help(const positional<T>& arg) const {
        return arg._help_msg;
    }

    template <c_argument_value_type T>
    [[nodiscard]] argument_descriptor get_desc(const positional<T>& arg, const bool verbose) const {
        return arg.desc(verbose, flag_char);
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool is_required(const positional<T>& arg) const {
        return arg.is_required();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool is_used(const positional<T>& arg) const {
        return arg.is_used();
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::size_t get_count(const positional<T>& arg) const {
        return arg.count();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool has_value(const positional<T>& arg) const {
        return arg.has_value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool has_parsed_values(const positional<T>& arg) const {
        return arg.has_parsed_values();
    }

    template <c_argument_value_type T>
    bool set_value(positional<T>& arg, const T& value) const {
        return arg.set_value(std::to_string(value));
    }

    template <c_argument_value_type T>
    bool set_value(positional<T>& arg, const std::string& str_value) const {
        return arg.set_value(str_value);
    }

    template <c_argument_value_type T>
    void reset_value(positional<T>& arg) const {
        arg._value.reset();
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::weak_ordering nvalues_ordering(const positional<T>& arg) const {
        return arg.nvalues_ordering();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::any& get_value(const positional<T>& arg) const {
        return arg.value();
    }

    template <c_argument_value_type T>
    const std::vector<std::any>& get_values(const positional<T>& arg) const {
        return arg.values();
    }

    static constexpr char flag_char = '-';
};

} // namespace ap_testing
