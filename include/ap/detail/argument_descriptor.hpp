#pragma once

#include "ap/detail/argument_name.hpp"
#include "concepts.hpp"
#include "str_utility.hpp"

#include <format>
#include <iomanip>
#include <optional>
#include <sstream>
#include <vector>

namespace ap::detail {

/*
Fill the string with spaces at the back:
str.resize(desired_length, ' ');
*/

struct parameter_descriptor {
    std::string name;
    std::string value;
};

class argument_descriptor {
public:
    argument_descriptor(const std::string& name, const std::optional<std::string>& help)
    : _name(name), _help(help) {}

    void add_param(const std::string& name, const std::string& value) {
        this->_params.emplace_back(name, value);
    }

    template <c_writable T>
    void add_param(const std::string& name, const T& value) {
        std::ostringstream oss;
        oss << std::boolalpha << value;
        this->_params.emplace_back(name, oss.str());
    }

    template <std::ranges::range R>
    requires(c_writable<std::ranges::range_value_t<R>>)
    void add_range_param(
        const std::string& name,
        const R& range,
        const std::string_view delimiter = default_delimiter
    ) {
        this->_params.emplace_back(name, join_with(range, delimiter));
    }

    // TODO: add arg name alignment
    std::string get(
        const uint8_t indent_width, std::optional<std::size_t> max_line_width = std::nullopt
    ) const {
        if (this->_params.empty())
            return this->_get_single_line();

        if (max_line_width.has_value()) {
            std::string single_line_str = this->_get_single_line();
            if (single_line_str.size() <= max_line_width.value())
                return single_line_str;
        }

        return this->_get_multi_line(indent_width);
    }

private:
    [[nodiscard]] std::string _get_single_line() const {
        std::ostringstream oss;

        oss << this->_name;
        if (this->_help.has_value())
            oss << " : " << this->_help.value();

        if (not this->_params.empty()) {
            oss << " (" << join_with(this->_params | std::views::transform([](const auto& param) {
                                         return std::format("{}: {}", param.name, param.value);
                                     }))
                << ")";
        }

        return oss.str();
    }

    [[nodiscard]] std::string _get_multi_line(const uint8_t indent_width) const {
        std::ostringstream oss;

        oss << this->_name;
        if (this->_help.has_value())
            oss << " : " << this->_help.value();

        std::size_t max_param_name_len = 0;
        for (const auto& param : this->_params)
            max_param_name_len = std::max(max_param_name_len, param.name.size());

        for (const auto& param : this->_params) {
            oss << "\n"
                << std::string(indent_width * 2, ' ') << "- "
                << std::setw(static_cast<int>(max_param_name_len)) << std::left << param.name
                << " = " << param.value;
        }
        oss << "\n";

        return oss.str();
    }

    std::string _name;
    std::optional<std::string> _help;
    std::vector<parameter_descriptor> _params;

    static constexpr std::string_view default_delimiter = ", ";
};

} // namespace ap::detail
