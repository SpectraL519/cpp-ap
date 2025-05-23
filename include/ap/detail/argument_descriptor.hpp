// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ap/detail/argument_name.hpp"
#include "concepts.hpp"
#include "str_utility.hpp"

#include <cstdint>
#include <format>
#include <iomanip>
#include <optional>
#include <sstream>
#include <vector>

namespace ap::detail {

struct parameter_descriptor {
    std::string name;
    std::string value;
};

class argument_descriptor {
public:
    argument_descriptor(const std::string& name, const std::optional<std::string>& help)
    : name(name), help(help) {}

    void add_param(const std::string& name, const std::string& value) {
        this->params.emplace_back(name, value);
    }

    template <c_writable T>
    void add_param(const std::string& name, const T& value) {
        std::ostringstream oss;
        oss << std::boolalpha << value;
        this->params.emplace_back(name, oss.str());
    }

    template <std::ranges::range R>
    requires(c_writable<std::ranges::range_value_t<R>>)
    void add_range_param(
        const std::string& name,
        const R& range,
        const std::string_view delimiter = default_delimiter
    ) {
        this->params.emplace_back(name, join_with(range, delimiter));
    }

    [[nodiscard]] std::string get_basic(
        const uint8_t indent_width, const std::optional<std::size_t> align_to = std::nullopt
    ) const {
        std::ostringstream oss;

        oss << std::string(indent_width, ' ');
        if (align_to.has_value())
            oss << std::setw(static_cast<int>(align_to.value())) << std::left;
        oss << this->name;

        if (this->help.has_value())
            oss << " : " << this->help.value();

        return oss.str();
    }

    [[nodiscard]] std::string get(
        const uint8_t indent_width, std::optional<std::size_t> max_line_width = std::nullopt
    ) const {
        std::ostringstream oss;

        if (this->params.empty())
            return this->_get_single_line(indent_width);

        if (max_line_width.has_value()) {
            std::string single_line_str = this->_get_single_line(indent_width);
            if (single_line_str.size() <= max_line_width.value())
                return single_line_str;
        }

        return this->_get_multi_line(indent_width);
    }

    std::string name;
    std::optional<std::string> help;
    std::vector<parameter_descriptor> params;

private:
    [[nodiscard]] std::string _get_single_line(const uint8_t indent_width) const {
        std::ostringstream oss;

        oss << this->get_basic(indent_width);
        if (not this->params.empty()) {
            oss << " (" << join_with(this->params | std::views::transform([](const auto& param) {
                                         return std::format("{}: {}", param.name, param.value);
                                     }))
                << ")";
        }

        return oss.str();
    }

    [[nodiscard]] std::string _get_multi_line(const uint8_t indent_width) const {
        std::ostringstream oss;

        oss << this->get_basic(indent_width);

        std::size_t max_param_name_len = 0ull;
        for (const auto& param : this->params)
            max_param_name_len = std::max(max_param_name_len, param.name.size());

        for (const auto& param : this->params) {
            oss << "\n"
                << std::string(indent_width * 2, ' ') << "- "
                << std::setw(static_cast<int>(max_param_name_len)) << std::left << param.name
                << " = " << param.value;
        }

        return oss.str();
    }

    static constexpr std::string_view default_delimiter = ", ";
};

} // namespace ap::detail
