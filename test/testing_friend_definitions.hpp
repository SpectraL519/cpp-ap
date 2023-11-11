#pragma once

#include <ap/argument_parser.hpp>

#ifdef AP_TESTING

namespace ap::detail {

template <readable T>
using arg_value_type = typename argument_interface<T>::value_type;

template <readable T>
bool testing_is_positional(const argument_interface<T>& argument) {
    return argument.is_positional();
}

template <readable T>
bool testing_is_optional(const argument_interface<T>& argument) {
    return argument.is_optional();
}

template <readable T>
bool testing_has_value(const argument_interface<T>& argument) {
    return argument.has_value();
}

template <readable T>
const arg_value_type<T>& testing_get_value(const argument_interface<T>& argument) {
    return argument.value();
}

template <readable T>
const std::string_view testing_get_name(const argument_interface<T>& argument) {
    return argument.name();
}

template <readable T>
const std::optional<std::string_view> testing_get_short_name(const optional_argument<T>& argument) {
    return argument.short_name();
}

template <readable T>
bool testing_is_required(const argument_interface<T>& argument) {
    return argument.required();
}

template <readable T>
const std::optional<std::string_view>&
    testing_get_help(const argument_interface<T>& argument) {
    return argument.help();
}

template <readable T>
const std::optional<typename argument_interface<T>::value_type>&
    testing_get_default_value(const argument_interface<T>& argument) {
    return argument.default_value();
}


template <readable T>
using positional_arg_value_type = typename positional_argument<T>::value_type;

template <readable T>
positional_argument<T>& testing_set_value(
    positional_argument<T>& argument, const positional_arg_value_type<T>& value
) {
    return argument.value(value);
}

template <readable T>
using optional_arg_value_type = typename optional_argument<T>::value_type;

template <readable T>
optional_argument<T>& testing_set_value(
    optional_argument<T>& argument, const optional_arg_value_type<T>& value
) {
    return argument.value(value);
}

} // namespace ap::detail

#endif
