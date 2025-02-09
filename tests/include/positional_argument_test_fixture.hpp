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
    [[nodiscard]] const argument_name& sut_get_name(const positional<T>& sut) const {
        return sut.name();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_is_required(const positional<T>& sut) const {
        return sut.is_required();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& sut_get_help(const positional<T>& sut) const {
        return sut.help();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_is_used(const positional<T>& sut) const {
        return sut.is_used();
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::size_t sut_get_nused(const positional<T>& sut) const {
        return sut.nused();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_has_value(const positional<T>& sut) const {
        return sut.has_value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_has_parsed_values(const positional<T>& sut) const {
        return sut.has_parsed_values();
    }

    template <c_argument_value_type T>
    positional<T>& sut_set_value(positional<T>& sut, const std::string& str_value) const {
        return sut.set_value(str_value);
    }

    template <c_argument_value_type T>
    positional<T>& sut_set_choices(positional<T>& sut, const std::vector<value_type<T>>& choices)
        const {
        return sut.choices(choices);
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::weak_ordering sut_nvalues_in_range(const positional<T>& sut) const {
        return sut.nvalues_in_range();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::any& sut_get_value(const positional<T>& sut) const {
        return sut.value();
    }

    template <c_argument_value_type T>
    const std::vector<std::any>& sut_get_values(const positional<T>& sut) const {
        return sut.values();
    }
};

} // namespace ap_testing
