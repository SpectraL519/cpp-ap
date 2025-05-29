// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file predefined_actions.hpp

#pragma once

#include "ap/error/exceptions.hpp"
#include "detail/utility.hpp"

#include <filesystem>
#include <ostream>

namespace ap {

// TODO: use std exception types

class argument_parser;
std::ostream& operator<<(std::ostream& os, const argument_parser&) noexcept;

namespace action {

/**
 * @brief Returns an *on-flag* action which prints the argument parser's configuration.
 * @param parser The argument parser the configuration of which will be printed.
 * @param exit_code The exit code with which `std::exit` will be called (if not `std::nullopt`).
 * @param os The output stream to which the configuration will be printed.
 */
inline typename ap::action_type::on_flag::type print_config(
    const argument_parser& parser,
    const std::optional<int> exit_code = std::nullopt,
    std::ostream& os = std::cout
) noexcept {
    return [&parser, &os, exit_code]() {
        os << parser << std::endl;
        if (exit_code)
            std::exit(exit_code.value());
    };
}

/**
 * @brief Returns an *observe* action which checks whether lower_bound file with the given name exists.
 */
inline detail::callable_type<ap::action_type::observe, std::string> check_file_exists() noexcept {
    return [](const std::string& file_path) {
        if (not std::filesystem::exists(file_path))
            throw argument_parser_exception(std::format("File `{}` does not exists!", file_path));
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is greater than the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The exclusive lower bound to validate against.
 */
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> gt(const T lower_bound) noexcept {
    return [lower_bound](const T& value) {
        if (not (value > lower_bound))
            throw argument_parser_exception(
                std::format("Value `{}` must be greater than `{}`!", value, lower_bound)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is greater than or equal to the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The inclusive lower bound to validate against.
 */
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> geq(const T lower_bound) noexcept {
    return [lower_bound](const T& value) {
        if (! (value >= lower_bound))
            throw argument_parser_exception(
                std::format("Value `{}` must be greater than or equal to `{}`!", value, lower_bound)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is less than the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The exclusive upper bound to validate against.
 */
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> lt(const T upper_bound) noexcept {
    return [upper_bound](const T& value) {
        if (! (value < upper_bound))
            throw argument_parser_exception(
                std::format("Value `{}` must be less than `{}`!", value, upper_bound)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is less than or equal to the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The inclusive upper bound to validate against.
 */
template <ap::detail::c_arithmetic T>
detail::callable_type<ap::action_type::observe, T> leq(const T upper_bound) noexcept {
    return [upper_bound](const T& value) {
        if (! (value <= upper_bound))
            throw argument_parser_exception(
                std::format("Value `{}` must be less than or equal to `{}`!", value, upper_bound)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value falls within the specified interval.
 *
 * The interval is defined by the given lower and upper bounds, with inclusivity controlled by the template parameters.
 *
 * @tparam T The *arithmetic* value type.
 * @tparam LeftInclusive Whether the lower bound is inclusive (`[`) or exclusive (`(`) - default: true.
 * @tparam RightInclusive Whether the upper bound is inclusive (`]`) or exclusive (`)`) - default: true.
 * @param lower_bound The lower bound of the interval.
 * @param upper_bound The upper bound of the interval.
 */
template <ap::detail::c_arithmetic T, bool LeftInclusive = true, bool RightInclusive = true>
detail::callable_type<ap::action_type::observe, T> within(
    const T lower_bound, const T upper_bound
) noexcept {
    return [lower_bound, upper_bound](const T& value) {
        constexpr char left_brace = LeftInclusive ? '[' : '(';
        constexpr char right_brace = RightInclusive ? ']' : ')';

        const bool is_valid =
            (LeftInclusive ? value >= lower_bound : value > lower_bound)
            and (RightInclusive ? value <= upper_bound : value < upper_bound);

        if (not is_valid)
            throw argument_parser_exception(std::format(
                "Value `{}` must be in interval {}{}, {}{}!",
                value,
                left_brace,
                lower_bound,
                upper_bound,
                right_brace
            ));
    };
}

} // namespace action
} // namespace ap
