#pragma once

#include <iostream>


namespace ap {

namespace detail {

template <typename T>
concept readable = requires (T value, std::istream& input_stream) {
    input_stream >> value;
};


template <readable T>
class argument_interface {
public:
    using value_type = T;

    virtual const std::string& name() = 0;

    virtual bool is_positional() = 0;
    virtual bool is_optional() = 0;

    virtual bool has_value() = 0;
    virtual const value_type& value() = 0;

    virtual argument_interface& help(const std::string&) = 0;
    virtual argument_interface& required(bool) = 0;
    virtual argument_interface& default_value(const value_type&) = 0;

    virtual const std::string& help() = 0;
    virtual bool required() = 0;
    virtual const value_type& default_value() = 0;

    virtual ~argument_interface() = default;
};

} // namespace detail

} // namespace ap
