// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "ap/detail/argument_name.hpp"

namespace ap::detail {

argument_name::argument_name(std::string_view primary) : primary(primary) {}

argument_name::argument_name(std::string_view primary, std::string_view secondary)
: primary(primary), secondary(secondary) {}

bool argument_name::operator==(const argument_name& other) const noexcept {
    if (not (this->secondary and other.secondary) and (this->secondary or other.secondary))
        return false;

    if (this->primary != other.primary)
        return false;

    return this->secondary ? this->secondary.value() == other.secondary.value() : true;
}

bool argument_name::match(std::string_view arg_name) const noexcept {
    return arg_name == this->primary or (this->secondary and arg_name == this->secondary.value());
}

bool argument_name::match(const argument_name& arg_name) const noexcept {
    if (this->match(arg_name.primary))
        return true;

    if (arg_name.secondary)
        return this->match(arg_name.secondary.value());

    return false;
}

std::string argument_name::str() const noexcept {
    return this->secondary
             ? ("[" + this->primary + "," + this->secondary.value() + "]")
             : ("[" + this->primary + "]");
}

std::ostream& operator<<(std::ostream& os, const argument_name& arg_name) noexcept {
    os << arg_name.str();
    return os;
}

} // namespace ap::detail
