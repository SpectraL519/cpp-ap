// Copyright (c) 2023-2026 Jakub Musiał
// This file is part of the CPP-ARGON project (https://github.com/SpectraL519/cpp-argon).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file argon/detail/help_builder.hpp
 * @brief Defines structures for creating and formatting help messages.
 */

#pragma once

#include "argon/detail/argument_name.hpp"
#include "argon/util/concepts.hpp"
#include "argon/util/string.hpp"

#include <cstdint>
#include <format>
#include <iomanip>
#include <optional>
#include <sstream>
#include <vector>

namespace argon::detail {

/// @brief A structure used to represent an argument's parameter description.
struct parameter_descriptor {
    std::string name;
    std::string value;
};

/// @brief A help message builder class.
class help_builder {
public:
    /**
     * @param name The string representation of the argument's name.
     * @param help An optional help message string.
     */
    help_builder(const std::string& name, const std::optional<std::string>& help = std::nullopt)
    : name(name), help(help) {}

    /**
     * @brief Adds a parameter descriptor with the given string value.
     * @param param_name The parameter's name.
     * @param value The parameter's value's string representation.
     */
    void add_param(const std::string& param_name, const std::string& value) {
        this->params.emplace_back(param_name, value);
    }

    /**
     * @brief Adds a parameter descriptor with the given value.
     * @tparam T The type of the parameter; must satisfy the @ref argon::util::c_writable concept.
     * @param param_name The parameter's name.
     * @param value The parameter's value.
     */
    template <util::c_writable T>
    void add_param(const std::string& param_name, const T& value) {
        std::ostringstream oss;
        oss << std::boolalpha << value;
        this->params.emplace_back(param_name, oss.str());
    }

    /**
     * @brief Adds a range parameter descriptor with the given value.
     * @tparam R The type of the parameter range. The value type of R must satisfy the @ref argon::util::c_writable concept.
     * @param param_name The parameter's name.
     * @param range The parameter value range.
     * @param delimiter The delimiter used to separate the range values.
     */
    template <std::ranges::range R>
    requires(util::c_writable<std::ranges::range_value_t<R>>)
    void add_range_param(
        const std::string& param_name,
        const R& range,
        const std::string_view delimiter = default_delimiter
    ) {
        this->params.emplace_back(param_name, util::join(range, delimiter));
    }

    /**
     * @param indent_width The indentation width.
     * @param align_to Optional minimum width for the argument name; if provided, the name will be left-aligned and padded with spaces to this width.
     * @return A basic argument description in the format "<indent><arg-name> : <help-msg>"
     * @note The help message will only be visible in the output if it's not `std::nullopt`
     */
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

    /**
     * Generates a full string representation of the argument descriptor, optionally formatted
     * to fit within a specified maximum line width. The output includes the argument name,
     * help message (if present), and any added parameters.
     *
     * If no parameters are present, or the generated one-line representation fits within
     * the `max_line_width`, a single-line format is used:
     *   "<indent><arg-name> : <help-msg> (<param1>: <value1>, ...)"
     *
     * Otherwise, a multi-line format is returned, listing each parameter on its own line
     * with additional indentation.
     *
     * @param indent_width The number of spaces to insert before the argument name.
     * @param max_line_width Optional maximum number of characters allowed for the one-line representation.
     *                       If the one-line output exceeds this, a multi-line format is used instead.
     * @return A formatted string describing the argument and its parameters.
     * @note The help message is only included if it is not `std::nullopt`.
     */
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
    /**
     * @brief Generates a single-line string representation of the argument and its parameters.
     * @param indent_width Number of spaces before the argument name.
     * @return A single-line formatted description string.
     */
    // clang-format off
    [[nodiscard]] std::string _get_single_line(const uint8_t indent_width) const {
        std::ostringstream oss;

        oss << this->get_basic(indent_width);
        if (not this->params.empty()) {
            oss << " ("
                << util::join(this->params | std::views::transform(
                    [](const auto& param) { return std::format("{}: {}", param.name, param.value); }
                ))
                << ")";
        }

        return oss.str();
    }

    // clang-format on

    /**
     * @brief Generates a multi-line string representation of the argument and its parameters, formatting each parameter on its own line with aligned names.
     * @param indent_width Number of spaces before the argument name and additional indentation for parameters.
     * @return A multi-line formatted description string.
     */
    [[nodiscard]] std::string _get_multi_line(const uint8_t indent_width) const {
        std::ostringstream oss;

        oss << this->get_basic(indent_width);

        std::size_t max_param_name_len = 0ull;
        for (const auto& param : this->params)
            max_param_name_len = std::max(max_param_name_len, param.name.size());

        for (const auto& param : this->params) {
            oss << '\n'
                << std::string(indent_width * 2, ' ') << "- "
                << std::setw(static_cast<int>(max_param_name_len)) << std::left << param.name
                << " = " << param.value;
        }

        return oss.str();
    }

    static constexpr std::string_view default_delimiter = ", ";
};

} // namespace argon::detail
