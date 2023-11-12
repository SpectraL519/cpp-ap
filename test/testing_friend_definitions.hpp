#pragma once

#include <ap/argument_parser.hpp>

#ifdef AP_TESTING

namespace ap::detail {

bool testing_is_positional(const argument_interface& argument) {
    return argument.is_positional();
}

bool testing_is_optional(const argument_interface& argument) {
    return argument.is_optional();
}

bool testing_has_value(const argument_interface& argument) {
    return argument.has_value();
}

const std::any& testing_get_value(const argument_interface& argument) {
    return argument.value();
}

const std::string_view testing_get_name(const argument_interface& argument) {
    return argument.name();
}

const std::optional<std::string_view> testing_get_short_name(const optional_argument& argument) {
    return argument.short_name();
}

bool testing_is_required(const argument_interface& argument) {
    return argument.required();
}

const std::optional<std::string_view>&
    testing_get_help(const argument_interface& argument) {
    return argument.help();
}

const std::any& testing_get_default_value(const argument_interface& argument) {
    return argument.default_value();
}


positional_argument& testing_set_value(
    positional_argument& argument, const std::any& value
) {
    return argument.value(value);
}

optional_argument& testing_set_value(
    optional_argument& argument, const std::any& value
) {
    return argument.value(value);
}

} // namespace ap::detail

#endif
