#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::argument::argument_name;
using ap::argument::optional_argument;
using ap::utility::readable;

namespace ap_testing {

struct optional_argument_test_fixture {
    optional_argument_test_fixture() = default;
    ~optional_argument_test_fixture() = default;

    template <readable T>
    using value_type = typename optional_argument<T>::value_type;

    template <readable T>
    optional_argument<T>& sut_set_value(
        optional_argument<T>& sut, const std::string& str_value
    ) const {
        return sut.set_value(str_value);
    }

    template <readable T>
    optional_argument<T>& sut_set_choices(
        optional_argument<T>& sut, const std::vector<value_type<T>>& choices
    ) {
        return sut.choices(choices);
    }

    template <readable T>
    bool sut_has_value(const optional_argument<T>& sut) const {
        return sut.has_value();
    }

    template <readable T>
    const std::any& sut_get_value(const optional_argument<T>& sut) const {
        return sut.value();
    }

    template <readable T>
    const std::vector<std::any>& sut_get_values(const optional_argument<T>& sut) const {
        return sut.values();
    }

    template <readable T>
    const argument_name& sut_get_name(const optional_argument<T>& sut) const {
        return sut.name();
    }

    template <readable T>
    bool sut_is_required(const optional_argument<T>& sut) const {
        return sut.is_required();
    }

    template <readable T>
    const std::optional<std::string>& sut_get_help(
        const optional_argument<T>& sut
    ) const {
        return sut.help();
    }
};

} // namespace ap_testing
