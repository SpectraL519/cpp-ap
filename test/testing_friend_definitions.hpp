#pragma once

#include <ap/argument_parser.hpp>

#ifdef AP_TESTING

namespace ap::detail {

bool testing_argument_is_positional(const argument_interface& argument) {
    return argument.is_positional();
}

bool testing_argument_is_optional(const argument_interface& argument) {
    return argument.is_optional();
}

bool testing_argument_has_value(const argument_interface& argument) {
    return argument.has_value();
}

const std::any& testing_argument_get_value(const argument_interface& argument) {
    return argument.value();
}

const argument_name& testing_argument_get_name(const argument_interface& argument) {
    return argument.name();
}

bool testing_argument_is_required(const argument_interface& argument) {
    return argument.required();
}

const std::optional<std::string_view>&
    testing_argument_get_help(const argument_interface& argument) {
    return argument.help();
}

const std::any&
    testing_argument_get_default_value(const argument_interface& argument) {
    return argument.default_value();
}

positional_argument&
    testing_argument_set_value(positional_argument& argument, const std::any& value) {
    return argument.value(value);
}

optional_argument&
    testing_argument_set_value(optional_argument& argument, const std::any& value) {
    return argument.value(value);
}

} // namespace ap::detail

#endif
