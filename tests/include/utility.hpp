#pragma once

#include "ap/detail/concepts.hpp"

namespace ap_testing {

template <ap::detail::c_writable T>
[[nodiscard]] std::string as_string(const T& value) noexcept {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

} // namespace ap_testing
