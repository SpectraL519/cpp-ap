#pragma once

#include <iostream>


namespace ap {

namespace detail {

template <typename T>
concept readable =
    requires(T value, std::istream& input_stream) { input_stream >> value; };


template <readable T>
class argument_interface {
public:
    using value_type = std::remove_reference_t<T>;

    virtual argument_interface& help(const std::string&) = 0;
    virtual argument_interface& required(bool) = 0;
    virtual argument_interface& default_value(const value_type&) = 0;

    virtual bool is_positional() const = 0;
    virtual bool is_optional() const = 0;

    virtual bool has_value() const = 0;
    virtual const value_type& value() const = 0;

    virtual const std::string& name() const = 0;
    virtual const std::string& help() const = 0;
    virtual bool required() const = 0;
    virtual const value_type& default_value() const = 0;

    virtual ~argument_interface() = default;
};


template <readable T>
class positional_argument : public argument_interface<T> {
public:
    using value_type = std::remove_reference_t<T>;

    positional_argument() = default;
    ~positional_argument() = default;

    [[nodiscard]] argument_interface& help(const std::string&) override;
    [[nodiscard]] argument_interface& required(bool) override;
    [[nodiscard]] argument_interface& default_value(const value_type&) override;

    [[nodiscard]] bool is_positional() const override;
    [[nodiscard]] bool is_optional() const override;

    [[nodiscard]] bool has_value() const override;
    [[nodiscard]] const value_type& value() const override;

    [[nodiscard]] const std::string& name() const override;
    [[nodiscard]] const std::string& help() const override;
    [[nodiscard]] bool required() const override;
    [[nodiscard]] const value_type& default_value() const override;
};

} // namespace detail

} // namespace ap
