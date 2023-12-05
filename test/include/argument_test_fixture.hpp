#pragma once

#define AP_TESTING

#include <ap/argument_parser.hpp>

#include <cstring>

using ap::detail::argument_interface;
using ap::detail::argument_name;

namespace ap_testing {

struct argument_test_fixture {
    argument_test_fixture() = default;
    ~argument_test_fixture() = default;

    using argument_ptr_type = std::unique_ptr<argument_interface>;

    argument_interface& sut_set_value(
        const argument_ptr_type& sut, const std::string& str_value
    ) const {
        return sut->value(str_value);
    }

    bool sut_has_value(const argument_ptr_type& sut) const {
        return sut->has_value();
    }

    const std::any& sut_get_value(const argument_ptr_type& sut) const {
        return sut->value();
    }

    const argument_name& sut_get_name(const argument_ptr_type& sut) const {
        return sut->name();
    }

    bool sut_is_required(const argument_ptr_type& sut) const {
        return sut->is_required();
    }

    const std::optional<std::string_view>& sut_get_help(const argument_ptr_type& sut) const {
        return sut->help();
    }

    const std::any& sut_get_default_value(const argument_ptr_type& sut) const {
        return sut->default_value();
    }
};

} // namespace ap_testing
