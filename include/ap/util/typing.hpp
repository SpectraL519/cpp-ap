// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/util/typing.hpp
 * @brief Provides common typing utility functions.
 */

#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <source_location>
#include <string_view>

namespace argon::util {

/**
 * @brief Retrieves the demangled name of a type `T`.
 * @tparam T The type to retrieve the name for.
 * @return A string view representing the demangled name of type `T`.
 * @note - The function uses compiler-specific macros to extract the type name.
 * @note - Supported compilers: GCC, Clang, MSVC.
 * @ingroup util
 */
template <typename T>
constexpr std::string_view get_demangled_type_name() {
#if defined(__clang__) || defined(__GNUC__)
    constexpr std::string_view func_name = __PRETTY_FUNCTION__;
    constexpr std::string_view begin_key = "T = ";
    constexpr std::string_view end_key = ";]";

    auto start_pos = func_name.find(begin_key) + begin_key.size();
    auto end_pos = func_name.find_first_of(end_key, start_pos);
    return func_name.substr(start_pos, end_pos - start_pos);
#elif defined(_MSC_VER)
    constexpr std::string_view func_name = __FUNCSIG__;
    constexpr std::string_view begin_key = "type_name<";
    constexpr char end_key = '>';

    auto start_pos = func_name.find(begin_key) + begin_key.size();
    auto end_pos = func_name.find(end_key, start_pos);
    return func_name.substr(start_pos, end_pos - start_pos);
#else
    return typeid(T).name();
#endif
}

} // namespace argon::util
