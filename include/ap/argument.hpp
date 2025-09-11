// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "action/detail/utility.hpp"
#include "action/predefined_actions.hpp"
#include "detail/argument_base.hpp"
#include "detail/argument_base_new.hpp"
#include "detail/argument_descriptor.hpp"
#include "detail/concepts.hpp"
#include "nargs/range.hpp"

namespace ap {

enum class argument_type : bool { positional, optional };

template <detail::c_argument_value_type T = std::string, argument_type AT>
class argument {
public:
    using value_type = T;
    using count_type = nargs::range::count_type;

    argument() = delete;

    argument(const detail::argument_name& name) : _name(name) {}

    /// @return Reference the name of the positional argument.
    [[nodiscard]] const ap::detail::argument_name& name() const noexcept {
        return this->_name;
    }

    /// @return Optional help message for the positional argument.
    [[nodiscard]] const std::optional<std::string>& help() const noexcept {
        return this->_help_msg;
    }

    /// @return `true` if the argument is hidden, `false` otherwise
    [[nodiscard]] bool is_hidden() const noexcept {
        return this->_hidden;
    }

    /// @return `true` if the argument is required, `false` otherwise
    [[nodiscard]] bool is_required() const noexcept {
        return this->_required;
    }

    /**
     * @return `true` if required argument bypassing is enabled for the argument, `false` otherwise.
     * @note Required argument bypassing is enabled only when both `required` and `bypass_required` flags are set to `true`.
     */
    [[nodiscard]] bool bypass_required_enabled() const noexcept {
        return not this->_required and this->_bypass_required;
    }

private:
    const ap::detail::argument_name _name;
    std::optional<std::string> _help_msg;

    bool _required : 1;
    bool _bypass_required : 1 = false;
    bool _hidden : 1 = false;
};

// /**
//  * @brief Checks if the provided choice is valid for the given set of choices.
//  * @param value A value, the validity of which is to be checked.
//  * @param choices The set against which the choice validity will be checked.
//  * @return `true` if the choice is valid, `false` otherwise.
//  * \todo replace with `std::ranges::contains` after transition to C++23
//  */
// template <c_argument_value_type T>
// [[nodiscard]] bool is_valid_choice(const T& value, const std::vector<T>& choices) noexcept {
//     return choices.empty() or std::ranges::find(choices, value) != choices.end();
// }

} // namespace ap
