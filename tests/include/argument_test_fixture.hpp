#pragma once

#include <ap/argument.hpp>
#include <ap/util/string.hpp>

using ap::argument;
using ap::argument_type;
using ap::detail::argument_descriptor;
using ap::detail::argument_name;
using ap::util::as_string;
using ap::util::c_argument_value_type;

namespace ap_testing {

struct argument_test_fixture {
    template <argument_type ArgT, c_argument_value_type T>
    using value_type = typename argument<ArgT, T>::value_type;

    template <argument_type ArgT, c_argument_value_type T>
    bool mark_used(argument<ArgT, T>& arg) const {
        return arg.mark_used();
    }

    template <argument_type ArgT, c_argument_value_type T>
    bool is_used(const argument<ArgT, T>& arg) const {
        return arg.is_used();
    }

    template <argument_type ArgT, c_argument_value_type T>
    std::size_t get_count(const argument<ArgT, T>& arg) const {
        return arg.count();
    }

    template <argument_type ArgT, c_argument_value_type T>
    bool set_required(argument<ArgT, T>& arg, const bool r) const {
        return arg._required = r;
    }

    template <argument_type ArgT, c_argument_value_type T>
    bool set_bypass_required(argument<ArgT, T>& arg, const bool br) const {
        return arg._bypass_required = br;
    }

    template <argument_type ArgT, c_argument_value_type T>
    bool set_value(argument<ArgT, T>& arg, const T& value) const {
        return set_value(arg, as_string(value));
    }

    template <argument_type ArgT, c_argument_value_type T>
    bool set_value(argument<ArgT, T>& arg, const std::string& str_value) const {
        return arg.set_value(str_value);
    }

    template <argument_type ArgT, c_argument_value_type T>
    void set_value_force(argument<ArgT, T>& arg, const T& value) const {
        set_value_force(arg, as_string(value));
    }

    template <argument_type ArgT, c_argument_value_type T>
    void set_value_force(argument<ArgT, T>& arg, const std::string& str_value) const {
        arg._values.emplace_back(str_value);
    }

    template <argument_type ArgT, c_argument_value_type T>
    void reset_values(argument<ArgT, T>& arg) const {
        arg._values.clear();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] bool has_value(const argument<ArgT, T>& arg) const {
        return arg.has_value();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] bool has_parsed_values(const argument<ArgT, T>& arg) const {
        return arg.has_parsed_values();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] bool has_predefined_values(const argument<ArgT, T>& arg) const {
        return arg.has_predefined_values();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] std::weak_ordering nvalues_ordering(const argument<ArgT, T>& arg) const {
        return arg.nvalues_ordering();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] const std::any& get_value(const argument<ArgT, T>& arg) const {
        return arg.value();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] const std::vector<std::any>& get_values(const argument<ArgT, T>& arg) const {
        return arg.values();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] const argument_name& get_name(const argument<ArgT, T>& arg) const {
        return arg.name();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& get_help(const argument<ArgT, T>& arg) const {
        return arg._help_msg;
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] argument_descriptor get_desc(const argument<ArgT, T>& arg, const bool verbose)
        const {
        return arg.desc(verbose);
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] bool is_hidden(const argument<ArgT, T>& arg) const {
        return arg.is_hidden();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] bool is_required(const argument<ArgT, T>& arg) const {
        return arg.is_required();
    }

    template <argument_type ArgT, c_argument_value_type T>
    [[nodiscard]] bool is_bypass_required_enabled(const argument<ArgT, T>& arg) const {
        return arg.is_bypass_required_enabled();
    }
};

} // namespace ap_testing
