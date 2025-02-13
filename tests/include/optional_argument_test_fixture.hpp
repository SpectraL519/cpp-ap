#pragma once

#define AP_TESTING

#include <ap/argument/optional.hpp>

using ap::argument::optional;
using ap::detail::argument_name;
using ap::detail::c_argument_value_type;

namespace ap_testing {

struct optional_argument_test_fixture {
    optional_argument_test_fixture() = default;
    ~optional_argument_test_fixture() = default;

    template <c_argument_value_type T>
    using value_type = typename optional<T>::value_type;

    template <c_argument_value_type T>
    void mark_used(optional<T>& arg) const {
        return arg.mark_used();
    }

    template <c_argument_value_type T>
    bool is_used(const optional<T>& arg) const {
        return arg.is_used();
    }

    template <c_argument_value_type T>
    std::size_t get_nused(const optional<T>& arg) const {
        return arg.nused();
    }

    template <c_argument_value_type T>
    optional<T>& set_value(optional<T>& arg, const T& value) const {
        return arg.set_value(std::to_string(value));
    }

    template <c_argument_value_type T>
    optional<T>& set_value(optional<T>& arg, const std::string& str_value) const {
        return arg.set_value(str_value);
    }

    template <c_argument_value_type T>
    void reset_value(optional<T>& arg) const {
        arg._values.clear();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool has_value(const optional<T>& arg) const {
        return arg.has_value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool has_parsed_values(const optional<T>& arg) const {
        return arg.has_parsed_values();
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::weak_ordering nvalues_in_range(const optional<T>& arg) const {
        return arg.nvalues_in_range();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::any& get_value(const optional<T>& arg) const {
        return arg.value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::vector<std::any>& get_values(const optional<T>& arg) const {
        return arg.values();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const argument_name& get_name(const optional<T>& arg) const {
        return arg.name();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool is_required(const optional<T>& arg) const {
        return arg.is_required();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& get_help(const optional<T>& arg) const {
        return arg.help();
    }
};

} // namespace ap_testing
