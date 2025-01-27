#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::argument::positional_argument;
using ap::detail::argument_name;
using ap::detail::c_argument_value_type;

namespace ap_testing {

struct positional_argument_test_fixture {
    positional_argument_test_fixture() = default;
    ~positional_argument_test_fixture() = default;

    template <c_argument_value_type T>
    using value_type = typename positional_argument<T>::value_type;

    template <c_argument_value_type T>
    [[nodiscard]] const argument_name& sut_get_name(const positional_argument<T>& sut) const {
        return sut.name();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_is_required(const positional_argument<T>& sut) const {
        return sut.is_required();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& sut_get_help(const positional_argument<T>& sut
    ) const {
        return sut.help();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_is_used(const positional_argument<T>& sut) const {
        return sut.is_used();
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::size_t sut_get_nused(const positional_argument<T>& sut) const {
        return sut.nused();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_has_value(const positional_argument<T>& sut) const {
        return sut.has_value();
    }

    template <c_argument_value_type T>
    [[nodiscard]] bool sut_has_parsed_values(const positional_argument<T>& sut) const {
        return sut.has_parsed_values();
    }

    template <c_argument_value_type T>
    positional_argument<T>& sut_set_value(positional_argument<T>& sut, const std::string& str_value)
        const {
        return sut.set_value(str_value);
    }

    template <c_argument_value_type T>
    positional_argument<T>& sut_set_choices(
        positional_argument<T>& sut, const std::vector<value_type<T>>& choices
    ) const {
        return sut.choices(choices);
    }

    template <c_argument_value_type T>
    [[nodiscard]] std::weak_ordering sut_nvalues_in_range(const positional_argument<T>& sut) const {
        return sut.nvalues_in_range();
    }

    template <c_argument_value_type T>
    [[nodiscard]] const std::any& sut_get_value(const positional_argument<T>& sut) const {
        return sut.value();
    }

    template <c_argument_value_type T>
    const std::vector<std::any>& sut_get_values(const positional_argument<T>& sut) const {
        return sut.values();
    }
};

} // namespace ap_testing
