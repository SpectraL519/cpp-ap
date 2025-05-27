#pragma once

#define AP_TESTING

#include <ap/argument/optional.hpp>
#include <ap/detail/str_utility.hpp>

using ap::argument::optional;
using ap::detail::argument_descriptor;
using ap::detail::argument_name;
using ap::detail::as_string;
using ap::detail::c_argument_value_type;

namespace ap_testing {

struct optional_argument_test_fixture {
    optional_argument_test_fixture() = default;
    ~optional_argument_test_fixture() = default;

    template <c_argument_value_type T>
    using value_type = typename optional<T>::value_type;

    template <c_argument_value_type T>
    bool mark_used(optional<T>& arg) const {
        return arg.mark_used();
    }

    template <c_argument_value_type T>
    bool is_used(const optional<T>& arg) const {
        return arg.is_used();
    }

    template <c_argument_value_type T>
    std::size_t get_count(const optional<T>& arg) const {
        return arg.count();
    }

    template <c_argument_value_type T>
    bool set_value(optional<T>& arg, const T& value) const {
        return set_value(arg, as_string(value));
    }

    template <c_argument_value_type T>
    bool set_value(optional<T>& arg, const std::string& str_value) const {
        return arg.set_value(str_value);
    }

    template <c_argument_value_type T>
    void set_value_force(optional<T>& arg, const T& value) const {
        set_value_force(arg, as_string(value));
    }

    template <c_argument_value_type T>
    void set_value_force(optional<T>& arg, const std::string& str_value) const {
        arg._values.emplace_back(str_value);
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
    [[nodiscard]] std::weak_ordering nvalues_ordering(const optional<T>& arg) const {
        return arg.nvalues_ordering();
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
    [[nodiscard]] const std::optional<std::string>& get_help(const optional<T>& arg) const {
        return arg.help();
    }

    template <c_argument_value_type T>
    [[nodiscard]] argument_descriptor get_desc(const optional<T>& arg, const bool verbose) const {
        return arg.desc(verbose);
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool is_required(const optional<T>& arg) const {
        return arg.is_required();
    }
};

} // namespace ap_testing
