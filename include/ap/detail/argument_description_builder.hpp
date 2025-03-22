#pragma once

#include "ap/detail/argument_name.hpp"

#include <format>
#include <sstream>
#include <vector>

namespace ap {

namespace nargs {

class range;
std::ostream& operator<<(std::ostream&, const range&) noexcept;

} // namespace nargs

namespace detail {

/*
Fill the string with spaces at the back:
str.resize(desired_length, ' ');
*/

class argument_description_builder {
public:
    argument_description_builder& argument(
        const detail::argument_name& arg_name, const std::optional<std::string>& help
    ) {
        this->_args.emplace_back(arg_name.str(), help.value_or("No help message provided"));
    }

    argument_description_builder& nargs(const nargs::range& r) noexcept {
        this->_ss << r;
        this->_args.back().params.emplace_back("nargs", this->_ss.str());
        this->_reset_ss();
        return *this;
    }

    argument_description_builder& choices() noexcept {
        return *this;
    }

    argument_description_builder& defaule_value() noexcept {
        return *this;
    }

    argument_description_builder& implicit_value() noexcept {
        return *this;
    }

    void reset() noexcept {
        this->_ss.clear();
        this->_args.clear();
        this->_depth = 0u;
    }

private:
    struct parameter_descriptor {
        std::string name;
        std::string value;
    };

    struct argument_descriptor {
        std::string name;
        std::string help;
        std::vector<parameter_descriptor> params;
    };

    void _reset_ss() noexcept {
        this->_ss.str("");
        this->_ss.clear();
    }

    std::stringstream _ss;
    std::vector<argument_descriptor> _args;
    std::uint16_t _depth = 0u;
};

} // namespace detail
} // namespace ap
