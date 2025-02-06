// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "action/predefined_actions.hpp"
#include "action/specifiers.hpp"
#include "argument/default.hpp"
#include "argument/optional.hpp"
#include "argument/positional.hpp"
#include "detail/argument_interface.hpp"
#include "detail/argument_name.hpp"
#include "detail/argument_token.hpp"
#include "detail/concepts.hpp"
#include "error/exceptions.hpp"
#include "nargs/range.hpp"

#include <algorithm>
#include <ranges>

#ifdef AP_TESTING

namespace ap_testing {
struct argument_parser_test_fixture;
} // namespace ap_testing

#endif

namespace ap {

// TODO: argument namespace alias

/// @brief Main argument parser class.
class argument_parser {
public:
    argument_parser(const argument_parser&) = delete;
    argument_parser& operator=(const argument_parser&) = delete;

    argument_parser() = default;

    argument_parser(argument_parser&&) = default;
    argument_parser& operator=(argument_parser&&) = default;

    ~argument_parser() = default;

    /**
     * @brief Set the program name.
     * @param name The name of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_name(std::string_view name) noexcept;

    /**
     * @brief Set the program description.
     * @param description The description of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_description(std::string_view description) noexcept;

    /**
     * @brief Set default positional arguments.
     * @param arg_discriminator_list Vector of default positional argument categories.
     * @return Reference to the argument parser.
     */
    argument_parser& default_positional_arguments(
        const std::vector<argument::default_positional>& arg_discriminator_list
    ) noexcept;

    /**
     * @brief Set default optional arguments.
     * @param arg_discriminator_list Vector of default optional argument categories.
     * @return Reference to the argument parser.
     */
    argument_parser& default_optional_arguments(
        const std::vector<argument::default_optional>& arg_discriminator_list
    ) noexcept;

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @return Reference to the added positional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::positional<T>& add_positional_argument(std::string_view primary_name) {
        // TODO: check forbidden characters

        const detail::argument_name arg_name = {primary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_positional_args.emplace_back(std::make_unique<argument::positional<T>>(arg_name));
        return static_cast<argument::positional<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added positional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::positional<T>& add_positional_argument(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        // TODO: check forbidden characters

        const detail::argument_name arg_name = {primary_name, secondary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_positional_args.emplace_back(std::make_unique<argument::positional<T>>(arg_name));
        return static_cast<argument::positional<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @return Reference to the added optional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::optional<T>& add_optional_argument(std::string_view primary_name) {
        // TODO: check forbidden characters

        const detail::argument_name arg_name = {primary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_optional_args.push_back(std::make_unique<argument::optional<T>>(arg_name));
        return static_cast<argument::optional<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added optional argument.
     */
    template <detail::c_argument_value_type T = std::string>
    argument::optional<T>& add_optional_argument(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        // TODO: check forbidden characters

        const detail::argument_name arg_name = {primary_name, secondary_name};
        if (this->_is_arg_name_used(arg_name))
            throw error::argument_name_used(arg_name);

        this->_optional_args.emplace_back(std::make_unique<argument::optional<T>>(arg_name));
        return static_cast<argument::optional<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a boolean flag argument to the parser's configuration.
     * @tparam StoreImplicitly Flag indicating whether to store implicitly.
     * @param primary_name The primary name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    argument::optional<bool>& add_flag(std::string_view primary_name) {
        return this->add_optional_argument<bool>(primary_name)
            .default_value(not StoreImplicitly)
            .implicit_value(StoreImplicitly)
            .nargs(0);
    }

    /**
     * @brief Adds a boolean flag argument to the parser's configuration.
     * @tparam StoreImplicitly Flag indicating whether to store implicitly.
     * @param primary_name The primary name of the flag.
     * @param secondary_name The secondary name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    argument::optional<bool>& add_flag(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        return this->add_optional_argument<bool>(primary_name, secondary_name)
            .default_value(not StoreImplicitly)
            .implicit_value(StoreImplicitly)
            .nargs(0);
    }

    /**
     * @brief Parses the command-line arguments.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     */
    void parse_args(int argc, char* argv[]);

    /**
     * @param arg_name The name of the argument.
     * @return True if the argument has a value, false otherwise.
     */
    bool has_value(std::string_view arg_name) const noexcept;

    /**
     * @param arg_name The name of the argument.
     * @return The count of times the argument has been used.
     */
    std::size_t count(std::string_view arg_name) const noexcept;

    /**
     * @tparam T Type of the argument value.
     * @param arg_name The name of the argument.
     * @return The value of the argument.
     */
    template <std::copy_constructible T = std::string>
    T value(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        if (not arg_opt)
            throw error::argument_not_found(arg_name);

        const auto& arg_value = arg_opt->get().value();
        try {
            return std::any_cast<T>(arg_value);
        }
        catch (const std::bad_any_cast& err) {
            throw error::invalid_value_type(arg_opt->get().name(), typeid(T));
        }
    }

    /**
     * @tparam T Type of the argument values.
     * @param arg_name The name of the argument.
     * @return The values of the argument as a vector.
     */
    template <std::copy_constructible T = std::string>
    std::vector<T> values(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        if (not arg_opt)
            throw error::argument_not_found(arg_name);

        const auto& arg = arg_opt->get();

        try {
            if (not arg.has_parsed_values() and arg.has_value())
                return std::vector<T>{std::any_cast<T>(arg.value())};

            std::vector<T> values;
            // TODO: use std::ranges::to after transition to C++23
            std::ranges::copy(
                std::views::transform(
                    arg.values(), [](const std::any& value) { return std::any_cast<T>(value); }
                ),
                std::back_inserter(values)
            );
            return values;
        }
        catch (const std::bad_any_cast& err) {
            throw error::invalid_value_type(arg.name(), typeid(T));
        }
    }

    /**
     * @brief Prints the argument parser's details to an output stream.
     * @param os Output stream.
     * @param parser The argument parser to print.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const argument_parser& parser) noexcept;

#ifdef AP_TESTING
    /// @brief Friend struct for testing purposes.
    friend struct ::ap_testing::argument_parser_test_fixture;
#endif

private:
    using arg_ptr_t = std::unique_ptr<detail::argument_interface>;
    using arg_ptr_list_t = std::vector<arg_ptr_t>;
    using arg_opt_t = std::optional<std::reference_wrapper<detail::argument_interface>>;

    using arg_token_list_t = std::vector<detail::argument_token>;
    using arg_token_list_iterator_t = typename arg_token_list_t::const_iterator;

    // TODO: extract to detail after impl split
    /**
     * @brief Adds a default positional argument based on the specified discriminator.
     * @param arg_discriminator The default positional argument discriminator.
     */
    void _add_default_positional_argument(const argument::default_positional arg_discriminator
    ) noexcept;

    // TODO: extract to detail after impl split
    /**
     * @brief Adds a default optional argument based on the specified discriminator.
     * @param arg_discriminator The default optional argument discriminator.
     */
    void _add_default_optional_argument(const argument::default_optional arg_discriminator
    ) noexcept;

    // TODO: extract to argument_name.{hpp,cpp} after impl split
    /**
     * @brief Returns a unary predicate function which checks if the given name matches the argument's name
     * @param arg_name The name of the argument.
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] auto _name_match_predicate(std::string_view arg_name) const noexcept {
        return [arg_name](const arg_ptr_t& arg) { return arg->name().match(arg_name); };
    }

    // TODO: extract to argument_name.{hpp,cpp} after impl split
    /**
     * @brief Returns a unary predicate function which checks if the given name matches the argument's name
     * @param arg_name The name of the argument.
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] auto _name_match_predicate(const detail::argument_name& arg_name) const noexcept {
        return [&arg_name](const arg_ptr_t& arg) { return arg->name().match(arg_name); };
    }

    /**
     * @brief Check if an argument name is already used.
     * @param arg_name The name of the argument.
     * @return True if the argument name is already used, false otherwise.
     */
    [[nodiscard]] bool _is_arg_name_used(const detail::argument_name& arg_name) const noexcept;

    /**
     * @brief Converts the command-line arguments into a list of tokens.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return List of preprocessed command-line argument tokens.
     */
    [[nodiscard]] arg_token_list_t _tokenize(int argc, char* argv[]) const noexcept;

    /**
     * @brief Check if an argument is a flag based on its value.
     * @param arg The cmd argument's value.
     * @return True if the argument is a flag, false otherwise.
     */
    [[nodiscard]] bool _is_flag(const std::string& arg) const noexcept;

    /**
     * @brief Remove the flag prefix from the argument.
     * @param arg_flag The argument flag to strip the prefix from.
     */
    void _strip_flag_prefix(std::string& arg_flag) const noexcept;

    /**
     * @brief Implementation of parsing command-line arguments.
     * @param arg_tokens The list of command-line argument tokens.
     */
    void _parse_args_impl(const arg_token_list_t& arg_tokens);

    /**
     * @brief Parse positional arguments based on command-line input.
     * @param arg_tokens The list of command-line argument tokens.
     * @param token_it Iterator for iterating through command-line argument tokens.
     */
    void _parse_positional_args(
        const arg_token_list_t& arg_tokens, arg_token_list_iterator_t& token_it
    ) noexcept;

    /**
     * @brief Parse optional arguments based on command-line input.
     * @param arg_tokens The list of command-line argument tokens.
     * @param token_it Iterator for iterating through command-line argument tokens.
     */
    void _parse_optional_args(
        const arg_token_list_t& arg_tokens, arg_token_list_iterator_t& token_it
    );

    /**
     * @brief Check if optional arguments can bypass the required arguments.
     * @return True if optional arguments can bypass required arguments, false otherwise.
     */
    [[nodiscard]] bool _are_required_args_bypassed() const noexcept;

    /// @brief Check if all required positional and optional arguments are used.
    void _check_required_args() const;

    /// @brief Check if the number of argument values is within the specified range.
    void _check_nvalues_in_range() const;

    /**
     * @brief Get the argument with the specified name.
     * @param arg_name The name of the argument.
     * @return The argument with the specified name, if found; otherwise, std::nullopt.
     */
    arg_opt_t _get_argument(std::string_view arg_name) const noexcept;

    std::optional<std::string> _program_name;
    std::optional<std::string> _program_description;

    arg_ptr_list_t _positional_args;
    arg_ptr_list_t _optional_args;

    static constexpr uint8_t _flag_prefix_char_length = 1u;
    static constexpr uint8_t _flag_prefix_length = 2u;
    static constexpr char _flag_prefix_char = '-';
    static constexpr std::string _flag_prefix = "--";
};

} // namespace ap
