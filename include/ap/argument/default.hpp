// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include <cstdint>

namespace ap::argument {

/// @brief Enum class representing positional arguments.
enum class default_positional : uint8_t { input, output };

/// @brief Enum class representing optional arguments.
enum class default_optional : uint8_t { help, input, output, multi_input, multi_output };

} // namespace ap::argument
