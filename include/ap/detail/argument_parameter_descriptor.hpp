#pragma once

#include "concepts.hpp"

#include <cstdint>
#include <functional>
#include <format>
#include <sstream>

namespace ap::detail {

enum class argument_parameter_descriptor_mode : std::uint8_t {
    m_none  = 0b00,
    m_named = 0b01,
    m_range = 0b10,
    m_named_range = m_named | m_range,
};

template <c_writable V, argument_parameter_descriptor_mode M>
class argument_parameter_descriptor {
public:
    using enum argument_parameter_descriptor_mode;

    using value_type = V;

    friend std::ostream& operator<<(std::ostream& os, argument_parameter_descriptor& desc) noexcept {

    }

    static constexpr argument_parameter_descriptor_mode mode = M;

private:
    [[nodiscard]] /* alwayt inline */ void _handle_name(std::ostream& os) const noexcept {
        if constexpr (mode & m_named)
            os << this->_name << ": ";
    }

    [[nodiscard]] /* alwayt inline */ void _handle_value(std::ostream& os) const noexcept {
        if constexpr (mode & m_range) {
            std::stringstream ss;
            // TODO: print the values to ss
        }
        else {
            os << this->_value;
        }
    }

    std::reference_wrapper<const value_type> _value;
    std::enable_if_t<mode & m_named, std::string_view> _name;
    std::enable_if_t<mode & m_range, std::string_view> _sep;
};

} // namespace ap::detail
