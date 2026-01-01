// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-ARGON project (https://github.com/SpectraL519/cpp-argon).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/// @file argon/action/predefined.hpp

#pragma once

#include "argon/action/util/helpers.hpp"
#include "argon/exceptions.hpp"

#include <filesystem>
#include <ostream>

namespace argon {

class argument_parser;
std::ostream& operator<<(std::ostream& os, const argument_parser&) noexcept;

namespace action {

/**
 * @brief Returns an *on-flag* action which prints the argument parser's help message.
 * @param parser Argument parser instance the help message of which will be printed.
 * @param exit_code The exit code with which `std::exit` will be called (if not `std::nullopt`).
 * @param os The output stream to which the help message will be printed.
 */
inline typename argon::action_type::on_flag::type print_help(
    const argument_parser& parser,
    const std::optional<int> exit_code = std::nullopt,
    std::ostream& os = std::cout
) noexcept {
    return [&parser, &os, exit_code]() {
        os << parser << std::endl;
        if (exit_code)
            std::exit(*exit_code);
    };
}

/// @brief Returns an *observe* action which checks whether lower_bound file with the given name exists.
inline util::callable_type<argon::action_type::observe, std::string> check_file_exists() noexcept {
    return [](const std::string& file_path) {
        if (not std::filesystem::exists(file_path))
            throw std::filesystem::filesystem_error(
                "File does not exists!",
                file_path,
                std::make_error_code(std::errc::no_such_file_or_directory)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is greater than the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The exclusive lower bound to validate against.
 */
template <argon::util::c_arithmetic T>
util::callable_type<argon::action_type::observe, T> gt(const T lower_bound) noexcept {
    return [lower_bound](const T& value) {
        if (not (value > lower_bound))
            throw std::out_of_range(
                std::format("Value `{}` must be greater than `{}`!", value, lower_bound)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is greater than or equal to the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The inclusive lower bound to validate against.
 */
template <argon::util::c_arithmetic T>
util::callable_type<argon::action_type::observe, T> geq(const T lower_bound) noexcept {
    return [lower_bound](const T& value) {
        if (! (value >= lower_bound))
            throw std::out_of_range(
                std::format("Value `{}` must be greater than or equal to `{}`!", value, lower_bound)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is less than the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The exclusive upper bound to validate against.
 */
template <argon::util::c_arithmetic T>
util::callable_type<argon::action_type::observe, T> lt(const T upper_bound) noexcept {
    return [upper_bound](const T& value) {
        if (! (value < upper_bound))
            throw std::out_of_range(
                std::format("Value `{}` must be less than `{}`!", value, upper_bound)
            );
    };
}

/**
 * @brief Returns an *observe* action which checks if a parsed value is less than or equal to the given bound.
 * @tparam T The *arithmetic* value type.
 * @param lower_bound The inclusive upper bound to validate against.
 */
template <argon::util::c_arithmetic T>
util::callable_type<argon::action_type::observe, T> leq(const T upper_bound) noexcept {
    return [upper_bound](const T& value) {
        if (! (value <= upper_bound))
            throw std::out_of_range(
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
template <argon::util::c_arithmetic T, bool LeftInclusive = true, bool RightInclusive = true>
util::callable_type<argon::action_type::observe, T> within(
    const T lower_bound, const T upper_bound
) noexcept {
    return [lower_bound, upper_bound](const T& value) {
        constexpr char left_brace = LeftInclusive ? '[' : '(';
        constexpr char right_brace = RightInclusive ? ']' : ')';

        const bool is_valid =
            (LeftInclusive ? value >= lower_bound : value > lower_bound)
            and (RightInclusive ? value <= upper_bound : value < upper_bound);

        if (not is_valid)
            throw std::out_of_range(std::format(
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
} // namespace argon
