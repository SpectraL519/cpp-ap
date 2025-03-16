#pragma once

#include "ap/detail/argument_name.hpp"

#include <sstream>

namespace ap::argument {

class description_generator {
public:
    description_generator(
        const detail::argument_name& arg_name,
        const std::string_view help
    ) {
        this->_ss << arg_name << " : " << help;
    }

    description_generator& nargs() noexcept {
        return *this;
    }

    description_generator& choices() noexcept {
        return *this;
    }

    description_generator& defaule_value() noexcept {
        return *this;
    }

    description_generator& implicit_value() noexcept {
        return *this;
    }

private:
    std::stringstream _ss;
};

} // namespace ap::argument
