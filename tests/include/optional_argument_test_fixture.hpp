#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::argument::optional_argument;
using ap::argument::detail::argument_name;
using ap::utility::valid_argument_value_type;

namespace ap_testing {

struct optional_argument_test_fixture {
    optional_argument_test_fixture() = default;
    ~optional_argument_test_fixture() = default;

    template <valid_argument_value_type T>
    using value_type = typename optional_argument<T>::value_type;

    template <valid_argument_value_type T>
    void sut_set_used(optional_argument<T>& sut) const {
        return sut.mark_used();
    }

    template <valid_argument_value_type T>
    bool sut_is_used(const optional_argument<T>& sut) const {
        return sut.is_used();
    }

    template <valid_argument_value_type T>
    std::size_t sut_get_nused(const optional_argument<T>& sut) const {
        return sut.nused();
    }

    template <valid_argument_value_type T>
    optional_argument<T>& sut_set_value(optional_argument<T>& sut, const std::string& str_value)
        const {
        sut.mark_used();
        return sut.set_value(str_value);
    }

    template <valid_argument_value_type T>
    optional_argument<T>& sut_set_choices(
        optional_argument<T>& sut, const std::vector<value_type<T>>& choices
    ) const {
        return sut.choices(choices);
    }

    template <valid_argument_value_type T>
    [[nodiscard]] bool sut_has_value(const optional_argument<T>& sut) const {
        return sut.has_value();
    }

    template <valid_argument_value_type T>
    [[nodiscard]] bool sut_has_parsed_values(const optional_argument<T>& sut) const {
        return sut.has_parsed_values();
    }

    template <valid_argument_value_type T>
    [[nodiscard]] std::weak_ordering sut_nvalues_in_range(const optional_argument<T>& sut) const {
        return sut.nvalues_in_range();
    }

    template <valid_argument_value_type T>
    [[nodiscard]] const std::any& sut_get_value(const optional_argument<T>& sut) const {
        return sut.value();
    }

    template <valid_argument_value_type T>
    [[nodiscard]] const std::vector<std::any>& sut_get_values(const optional_argument<T>& sut
    ) const {
        return sut.values();
    }

    template <valid_argument_value_type T>
    [[nodiscard]] const argument_name& sut_get_name(const optional_argument<T>& sut) const {
        return sut.name();
    }

    template <valid_argument_value_type T>
    [[nodiscard]] bool sut_is_required(const optional_argument<T>& sut) const {
        return sut.is_required();
    }

    template <valid_argument_value_type T>
    [[nodiscard]] const std::optional<std::string>& sut_get_help(const optional_argument<T>& sut
    ) const {
        return sut.help();
    }
};

} // namespace ap_testing
