#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::detail::argument_name;
using ap::detail::optional_argument;
using ap::detail::readable;

namespace ap_testing {

struct optional_argument_test_fixture {
    optional_argument_test_fixture() = default;
    ~optional_argument_test_fixture() = default;

    template <readable T>
    optional_argument<T>& sut_set_value(
        optional_argument<T>& sut, const std::string& str_value
    ) const {
        return sut.value(str_value);
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
    const argument_name& sut_get_name(const optional_argument<T>& sut) const {
        return sut.name();
    }

    template <readable T>
    bool sut_is_required(const optional_argument<T>& sut) const {
        return sut.is_required();
    }

    template <readable T>
    const std::optional<std::string_view>& sut_get_help(
        const optional_argument<T>& sut
    ) const {
        return sut.help();
    }
};

} // namespace ap_testing
