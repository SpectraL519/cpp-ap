#pragma once

#include <ap/argument.hpp>

namespace argon_testing {

template <typename T>
void discard_result(T&&) {
    // do nothing
}

template <argon::util::c_argument_value_type T = std::string>
bool is_positional(const argon::detail::argument_base& arg) {
    return dynamic_cast<const argon::positional_argument<T>*>(&arg);
}

template <argon::util::c_argument_value_type T = std::string>
bool is_optional(const argon::detail::argument_base& arg) {
    return dynamic_cast<const argon::optional_argument<T>*>(&arg);
}

} // namespace argon_testing
