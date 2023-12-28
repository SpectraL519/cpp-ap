#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::argument::detail::argument_name;
using ap::argument::positional_argument;
using ap::utility::readable;

namespace ap_testing {

struct positional_argument_test_fixture {
    positional_argument_test_fixture() = default;
    ~positional_argument_test_fixture() = default;

    template <readable T>
    using value_type = typename positional_argument<T>::value_type;

    template <readable T>
    inline positional_argument<T>& sut_set_value(
        positional_argument<T>& sut, const std::string& str_value
    ) const {
        return sut.set_value(str_value);
    }

    template <readable T>
    inline positional_argument<T>& sut_set_choices(
        positional_argument<T>& sut, const std::vector<value_type<T>>& choices
    ) const {
        return sut.choices(choices);
    }

    template <readable T>
    [[nodiscard]] inline bool sut_has_value(const positional_argument<T>& sut) const {
        return sut.has_value();
    }

    template <readable T>
    [[nodiscard]] inline bool sut_has_parsed_values(const positional_argument<T>& sut) const {
        return sut.has_parsed_values();
    }

    template <readable T>
    [[nodiscard]] inline std::weak_ordering sut_nvalues_in_range(
        const positional_argument<T>& sut
    ) const {
        return sut.nvalues_in_range();
    }

    template <readable T>
    [[nodiscard]] inline const std::any& sut_get_value(
        const positional_argument<T>& sut
    ) const {
        return sut.value();
    }

    template <readable T>
    inline const std::vector<std::any>& sut_get_values(const positional_argument<T>& sut) const {
        return sut.values();
    }

    template <readable T>
    [[nodiscard]] inline const argument_name& sut_get_name(
        const positional_argument<T>& sut
    ) const {
        return sut.name();
    }

    template <readable T>
    [[nodiscard]] inline bool sut_is_required(const positional_argument<T>& sut) const {
        return sut.is_required();
    }

    template <readable T>
    [[nodiscard]] inline const std::optional<std::string>& sut_get_help(
        const positional_argument<T>& sut
    ) const {
        return sut.help();
    }
};

} // namespace ap_testing
