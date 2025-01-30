#pragma once

#include <cstdint>

namespace ap::argument {

/// @brief Enum class representing positional arguments.
enum class default_positional : uint8_t { input, output };

/// @brief Enum class representing optional arguments.
enum class default_optional : uint8_t { help, input, output, multi_input, multi_output };

} // namespace ap::argument
