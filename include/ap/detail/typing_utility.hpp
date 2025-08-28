#pragma once

#include <string_view>
#include <source_location>

namespace ap::detail {

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

} // namespace ap::detail
