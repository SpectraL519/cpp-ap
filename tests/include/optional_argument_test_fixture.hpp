#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

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
    void sut_set_used(optional<T>& sut) const {
        return sut.mark_used();
    }

    template <c_argument_value_type T>
    bool sut_is_used(const optional<T>& sut) const {
        return sut.is_used();
    }

    template <c_argument_value_type T>
    std::size_t sut_get_nused(const optional<T>& sut) const {
        return sut.nused();
    }

    template <c_argument_value_type T>
    optional<T>& sut_set_value(optional<T>& sut, const std::string& str_value) const {
        sut.mark_used();
        return sut.set_value(str_value);
    }

    template <c_argument_value_type T>
    optional<T>& sut_set_choices(optional<T>& sut, const std::vector<value_type<T>>& choices)
        const {
        return sut.choices(choices);
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_has_value(const optional<T>& sut) const {
        return sut.has_value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_has_parsed_values(const optional<T>& sut) const {
        return sut.has_parsed_values();
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::weak_ordering sut_nvalues_in_range(const optional<T>& sut) const {
        return sut.nvalues_in_range();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::any& sut_get_value(const optional<T>& sut) const {
        return sut.value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::vector<std::any>& sut_get_values(const optional<T>& sut) const {
        return sut.values();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const argument_name& sut_get_name(const optional<T>& sut) const {
        return sut.name();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_is_required(const optional<T>& sut) const {
        return sut.is_required();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& sut_get_help(const optional<T>& sut) const {
        return sut.help();
    }
};

} // namespace ap_testing
