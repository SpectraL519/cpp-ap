#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::argument::argument_name;
using ap::argument::positional_argument;
using ap::utility::readable;

namespace ap_testing {

struct positional_argument_test_fixture {
    positional_argument_test_fixture() = default;
    ~positional_argument_test_fixture() = default;

    template <readable T>
    positional_argument<T>& sut_set_value(
        positional_argument<T>& sut, const std::string& str_value
    ) const {
        return sut.value(str_value);
    }

    template <readable T>
    positional_argument<T>& sut_set_choices(
        positional_argument<T>& sut, const std::vector<std::any>& choices
    ) const {
        return sut.choices(choices);
    }

    template <readable T>
    bool sut_has_value(const positional_argument<T>& sut) const {
        return sut.has_value();
    }

    template <readable T>
    const std::any& sut_get_value(const positional_argument<T>& sut) const {
        return sut.value();
    }

    template <readable T>
    const argument_name& sut_get_name(const positional_argument<T>& sut) const {
        return sut.name();
    }

    template <readable T>
    bool sut_is_required(const positional_argument<T>& sut) const {
        return sut.is_required();
    }

    template <readable T>
    const std::optional<std::string_view>& sut_get_help(
        const positional_argument<T>& sut
    ) const {
        return sut.help();
    }
};

} // namespace ap_testing
