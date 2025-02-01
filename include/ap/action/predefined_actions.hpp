// Copyright (c) 2024-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "ap/error/exceptions.hpp"
#include "detail/utility.hpp"

#include <filesystem>

namespace ap::action {

/// @brief Returns a default argument action.
template <ap::detail::c_argument_value_type T>
detail::callable_type<ap::action_type::modify, T> none() noexcept {
    return [](T&) {};
}

/// @brief Returns a predefined action for file name handling arguments. Checks whether a file with the given name exists.
inline detail::callable_type<ap::action_type::modify, std::string> check_file_exists() noexcept {
    return [](std::string& file_path) {
        if (not std::filesystem::exists(file_path))
            throw argument_parser_exception(std::format("File `{}` does not exists!", file_path));
    };
}

} // namespace ap::action
