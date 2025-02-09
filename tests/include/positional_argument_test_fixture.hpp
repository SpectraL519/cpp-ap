#pragma once

#define AP_TESTING

#include <ap/argument/positional.hpp>

using ap::argument::positional;
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
    [[nodiscard]] bool is_required(const positional<T>& arg) const {
        return arg.is_required();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& get_help(const positional<T>& arg) const {
        return arg.help();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool is_used(const positional<T>& arg) const {
        return arg.is_used();
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::size_t get_nused(const positional<T>& arg) const {
        return arg.nused();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool has_value(const positional<T>& arg) const {
        return arg.has_value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool has_parsed_values(const positional<T>& arg) const {
        return arg.has_parsed_values();
    }

    // TODO: const T& instead of const std::string&
    template <c_argument_value_type T>
    positional<T>& set_value(positional<T>& arg, const std::string& str_value) const {
        return arg.set_value(str_value);
    }

    template <c_argument_value_type T>
    positional<T>& set_choices(positional<T>& arg, const std::vector<value_type<T>>& choices)
        const {
        return arg.choices(choices);
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::weak_ordering nvalues_in_range(const positional<T>& arg) const {
        return arg.nvalues_in_range();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::any& get_value(const positional<T>& arg) const {
        return arg.value();
    }

    template <c_argument_value_type T>
    const std::vector<std::any>& get_values(const positional<T>& arg) const {
        return arg.values();
    }
};

} // namespace ap_testing
