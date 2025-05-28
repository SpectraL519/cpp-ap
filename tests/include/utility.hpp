#pragma once

#include <ap/argument/optional.hpp>
#include <ap/argument/positional.hpp>

namespace ap_testing {

template <typename T>
void discard_result(T&&) {
    // do nothing
}

template <ap::detail::c_argument_value_type T = std::string>
bool is_positional(const ap::detail::argument_base& arg) {
    return dynamic_cast<const ap::argument::positional<T>*>(&arg);
}

template <ap::detail::c_argument_value_type T = std::string>
bool is_optional(const ap::detail::argument_base& arg) {
    return dynamic_cast<const ap::argument::optional<T>*>(&arg);
}

} // namespace ap_testing
