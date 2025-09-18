// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/argument_parser.hpp
 * @brief Main library header file. Defines the `argument_parser` class.
 */

#pragma once

#include "argument.hpp"
#include "detail/argument_token.hpp"
#include "detail/concepts.hpp"
#include "types.hpp"

#include <algorithm>
#include <format>
#include <iostream>
#include <ranges>
#include <span>

#ifdef AP_TESTING

namespace ap_testing {
struct argument_parser_test_fixture;
} // namespace ap_testing

#endif

namespace ap {

class argument_parser;

enum class default_argument {
    p_input,
    p_output,
    o_help,
    o_input,
    o_output,
    o_multi_input,
    o_multi_output
};

namespace detail {

void add_default_argument(const default_argument, argument_parser&) noexcept;

} // namespace detail

/**
 * @brief The main argument parser class.
 * This class is responsible for the configuration and parsing of command-line arguments.
 */
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
    argument_parser& program_name(std::string_view name) {
        if (detail::contains_whitespaces(name))
            throw invalid_configuration("The program name cannot contain whitespace characters!");

        this->_program_name.emplace(name);
        return *this;
    }

    /**
     * @brief Set the program version.
     * @param version The version of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_version(const version& version) noexcept {
        this->_program_version.emplace(version.str());
        return *this;
    }

    /**
     * @brief Set the program version.
     * @param version The version of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_version(std::string_view version) {
        if (detail::contains_whitespaces(version))
            throw invalid_configuration("The program version cannot contain whitespace characters!"
            );

        this->_program_version.emplace(version);
        return *this;
    }

    /**
     * @brief Set the program description.
     * @param description The description of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_description(std::string_view description) noexcept {
        this->_program_description.emplace(description);
        return *this;
    }

    /**
     * @brief Set the verbosity mode.
     * @note The default verbosity mode value is `false`.
     * @param v The verbosity mode value.
     * @return Reference to the argument parser.
     */
    argument_parser& verbose(const bool v = true) noexcept {
        this->_verbose = v;
        return *this;
    }

    /**
     * @brief Add default arguments to the argument parser.
     * @tparam AR Type of the positional argument discriminator range.
     * @param arg_discriminators A range of default positional argument discriminators.
     * @return Reference to the argument parser.
     */
    template <detail::c_range_of<default_argument> AR>
    argument_parser& default_arguments(const AR& arg_discriminators) noexcept {
        for (const auto arg_discriminator : arg_discriminators)
            detail::add_default_argument(arg_discriminator, *this);
        return *this;
    }

    /**
     * @brief Add default arguments to the argument parser.
     * @param arg_discriminators A list of default positional argument discriminators.
     * @return Reference to the argument parser.
     */
    argument_parser& default_arguments(
        const std::initializer_list<default_argument>& arg_discriminators
    ) noexcept {
        return this->default_arguments<>(arg_discriminators);
    }

    /**
     * @brief Add default arguments to the argument parser.
     * @param arg_discriminators A list of default positional argument discriminators.
     * @return Reference to the argument parser.
     */
    argument_parser& default_arguments(
        const std::same_as<default_argument> auto... arg_discriminators
    ) noexcept {
        (detail::add_default_argument(arg_discriminators, *this), ...);
        return *this;
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @return Reference to the added positional argument.
     * @throws ap::invalid_configuration
     */
    template <detail::c_argument_value_type T = std::string>
    positional_argument<T>& add_positional_argument(const std::string_view primary_name) {
        this->_verify_arg_name_pattern(primary_name);

        const detail::argument_name arg_name(std::make_optional<std::string>(primary_name));
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_positional_args.emplace_back(std::make_shared<positional_argument<T>>(arg_name));
        return static_cast<positional_argument<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added positional argument.
     * @throws ap::invalid_configuration
     */
    template <detail::c_argument_value_type T = std::string>
    positional_argument<T>& add_positional_argument(
        const std::string_view primary_name, const std::string_view secondary_name
    ) {
        this->_verify_arg_name_pattern(primary_name);
        this->_verify_arg_name_pattern(secondary_name);

        const detail::argument_name arg_name{
            std::make_optional<std::string>(primary_name),
            std::make_optional<std::string>(secondary_name)
        };
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_positional_args.emplace_back(std::make_shared<positional_argument<T>>(arg_name));
        return static_cast<positional_argument<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param name The name of the argument.
     * @param name_discr The discriminator value specifying whether the given name should be treated as primary or secondary.
     * @return Reference to the added optional argument.
     * @throws ap::invalid_configuration
     */
    template <detail::c_argument_value_type T = std::string>
    optional_argument<T>& add_optional_argument(
        const std::string_view name,
        const detail::argument_name_discriminator name_discr = n_primary
    ) {
        this->_verify_arg_name_pattern(name);

        const auto arg_name =
            name_discr == n_primary
                ? detail::
                      argument_name{std::make_optional<std::string>(name), std::nullopt, this->_flag_prefix_char}
                : detail::argument_name{
                      std::nullopt, std::make_optional<std::string>(name), this->_flag_prefix_char
                  };

        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_optional_args.push_back(std::make_shared<optional_argument<T>>(arg_name));
        return static_cast<optional_argument<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added optional argument.
     * @throws ap::invalid_configuration
     */
    template <detail::c_argument_value_type T = std::string>
    optional_argument<T>& add_optional_argument(
        const std::string_view primary_name, const std::string_view secondary_name
    ) {
        this->_verify_arg_name_pattern(primary_name);
        this->_verify_arg_name_pattern(secondary_name);

        const detail::argument_name arg_name(
            std::make_optional<std::string>(primary_name),
            std::make_optional<std::string>(secondary_name),
            this->_flag_prefix_char
        );
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_optional_args.emplace_back(std::make_shared<optional_argument<T>>(arg_name));
        return static_cast<optional_argument<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a boolean flag argument (an optional argument with `value_type = bool`) to the parser's configuration.
     * @tparam StoreImplicitly A boolean value used as the `implicit_values` parameter of the argument.
     * @note The argument's `default_values` attribute will be set to `not StoreImplicitly`.
     * @param name The primary name of the flag.
     * @param name_discr The discriminator value specifying whether the given name should be treated as primary or secondary.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    optional_argument<bool>& add_flag(
        const std::string_view name,
        const detail::argument_name_discriminator name_discr = n_primary
    ) {
        return this->add_optional_argument<bool>(name, name_discr)
            .default_values(not StoreImplicitly)
            .implicit_values(StoreImplicitly)
            .nargs(0ull);
    }

    /**
     * @brief Adds a boolean flag argument (an optional argument with `value_type = bool`) to the parser's configuration.
     * @tparam StoreImplicitly A boolean value used as the `implicit_values` parameter of the argument.
     * @note The argument's `default_values` attribute will be set to `not StoreImplicitly`.
     * @param primary_name The primary name of the flag.
     * @param secondary_name The secondary name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    optional_argument<bool>& add_flag(
        const std::string_view primary_name, const std::string_view secondary_name
    ) {
        return this->add_optional_argument<bool>(primary_name, secondary_name)
            .default_values(not StoreImplicitly)
            .implicit_values(StoreImplicitly)
            .nargs(0ull);
    }

    /**
     * @brief Parses the command-line arguments.
     *
     * Equivalent to:
     * ```cpp
     * parse_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)))
     * ```
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument values.
     * @throws ap::invalid_configuration, ap::parsing_failure
     * @attention The first argument (the program name) is ignored.
     */
    void parse_args(int argc, char* argv[]) {
        this->parse_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)));
    }

    /**
     * @todo use std::ranges::forward_range
     * @brief Parses the command-line arguments.
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @throws ap::invalid_configuration, ap::parsing_failure
     * @attention This overload of the `parse_args` function assumes that the program name argument has already been discarded.
     */
    template <detail::c_range_of<std::string, detail::type_validator::convertible> AR>
    void parse_args(const AR& argv_rng) {
        this->_validate_argument_configuration();

        std::vector<std::string> unknown_args;
        this->_parse_args_impl(this->_tokenize(argv_rng), unknown_args);

        if (not unknown_args.empty())
            throw parsing_failure::argument_deduction_failure(unknown_args);

        if (not this->_are_required_args_bypassed()) {
            this->_verify_required_args();
            this->_verify_nvalues();
        }
    }

    /**
     * @brief Parses the command-line arguments and exits on error.
     *
     * Equivalent to:
     * ```cpp
     * try_parse_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)))
     * ```
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument values.
     * @note The first argument (the program name) is ignored.
     */
    void try_parse_args(int argc, char* argv[]) {
        this->try_parse_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)));
    }

    /**
     * @todo use std::ranges::forward_range
     * @brief Parses the command-line arguments and exits on error.
     *
     * Calls `parse_args(argv_rng)` in a try-catch block. If an error is thrown, then its
     * message and the parser are printed to `std::cerr` and the function exists with
     * `EXIT_FAILURE` status.
     *
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @attention This overload of the `try_parse_args` function assumes that the program name argument has already been discarded.
     */
    template <detail::c_range_of<std::string, detail::type_validator::convertible> AR>
    void try_parse_args(const AR& argv_rng) {
        try {
            this->parse_args(argv_rng);
        }
        catch (const ap::argument_parser_exception& err) {
            std::cerr << "[ERROR] : " << err.what() << std::endl << *this << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Parses the known command-line arguments.
     *
     * Equivalent to:
     * ```cpp
     * try_parse_known_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)))
     * ```
     *
     * An argument is considered "known" if it was defined using the parser's argument declaraion methods:
     * - `add_positional_argument`
     * - `add_optional_argument`
     * - `add_flag`
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument values.
     * @throws ap::invalid_configuration, ap::parsing_failure
     * @attention The first argument (the program name) is ignored.
     */
    std::vector<std::string> parse_known_args(int argc, char* argv[]) {
        return this->parse_known_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)));
    }

    /**
     * @todo use std::ranges::forward_range
     * @brief Parses the known command-line arguments.
     *
     * * An argument is considered "known" if it was defined using the parser's argument declaraion methods:
     * - `add_positional_argument`
     * - `add_optional_argument`
     * - `add_flag`
     *
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @throws ap::invalid_configuration, ap::parsing_failure
     * @attention This overload of the `parse_known_args` function assumes that the program name argument already been discarded.
     */
    template <detail::c_range_of<std::string, detail::type_validator::convertible> AR>
    std::vector<std::string> parse_known_args(const AR& argv_rng) {
        this->_validate_argument_configuration();

        std::vector<std::string> unknown_args;
        this->_parse_args_impl(this->_tokenize(argv_rng), unknown_args, false);

        if (not this->_are_required_args_bypassed()) {
            this->_verify_required_args();
            this->_verify_nvalues();
        }

        return unknown_args;
    }

    /**
     * @brief Parses the known command-line arguments and exits on error.
     *
     * Equivalent to:
     * ```cpp
     * try_parse_known_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)))
     * ```
     *
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument values.
     * @return A vector of unknown argument values.
     * @attention The first argument (the program name) is ignored.
     */
    std::vector<std::string> try_parse_known_args(int argc, char* argv[]) {
        return this->try_parse_known_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)));
    }

    /**
     * @todo use std::ranges::forward_range
     * @brief Parses known the command-line arguments and exits on error.
     *
     * Calls `parse_known_args(argv_rng)` in a try-catch block. If an error is thrown, then its message
     * and the parser are printed to `std::cerr` and the function exists with `EXIT_FAILURE` status.
     * Otherwise the result of `parse_known_args(argv_rng)` is returned.
     *
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @return A vector of unknown argument values.
     * @attention This overload of the `try_parse_known_args` function assumes that the program name argument has already been discarded.
     */
    template <detail::c_range_of<std::string, detail::type_validator::convertible> AR>
    std::vector<std::string> try_parse_known_args(const AR& argv_rng) {
        try {
            return this->parse_known_args(argv_rng);
        }
        catch (const ap::argument_parser_exception& err) {
            std::cerr << "[ERROR] : " << err.what() << std::endl << *this << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    // clang-format off

    /**
     * @brief Handles the `help` argument logic.
     *
     * Checks the value of the `help` boolean flag argument and if the value `is` true,
     * prints the parser to `std::cout` anb exists with `EXIT_SUCCESS` status.
     */
    [[deprecated("The default help argument now uses the `print_config` on-flag action")]]
    void handle_help_action() const noexcept {
        if (this->value<bool>("help")) {
            std::cout << *this << std::endl;
            std::exit(EXIT_SUCCESS);
        }
    }

    // clang-format on

    /**
     * @param arg_name The name of the argument.
     * @return True if the argument has a value, false otherwise.
     */
    [[nodiscard]] bool has_value(std::string_view arg_name) const noexcept {
        const auto arg = this->_get_argument(arg_name);
        return arg ? arg->has_value() : false;
    }

    /**
     * @param arg_name The name of the argument.
     * @return The count of times the argument has been used.
     */
    [[nodiscard]] std::size_t count(std::string_view arg_name) const noexcept {
        const auto arg = this->_get_argument(arg_name);
        return arg ? arg->count() : 0ull;
    }

    /**
     * @tparam T Type of the argument value.
     * @param arg_name The name of the argument.
     * @return The value of the argument.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <detail::c_argument_value_type T = std::string>
    [[nodiscard]] T value(std::string_view arg_name) const {
        const auto arg = this->_get_argument(arg_name);
        if (not arg)
            throw lookup_failure::argument_not_found(arg_name);

        const auto& arg_value = arg->value();
        try {
            return std::any_cast<T>(arg_value);
        }
        catch (const std::bad_any_cast&) {
            throw type_error::invalid_value_type<T>(arg->name());
        }
    }

    /**
     * @tparam T Type of the argument value.
     * @tparam U The default value type.
     * @param arg_name The name of the argument.
     * @param fallback_value The fallback value.
     * @return The value of the argument.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <detail::c_argument_value_type T = std::string, std::convertible_to<T> U>
    [[nodiscard]] T value_or(std::string_view arg_name, U&& fallback_value) const {
        const auto arg = this->_get_argument(arg_name);
        if (not arg)
            throw lookup_failure::argument_not_found(arg_name);

        try {
            const auto& arg_value = arg->value();
            return std::any_cast<T>(arg_value);
        }
        catch (const std::logic_error&) {
            // positional: no value parsed
            // optional: no value parsed + no predefined value
            return T{std::forward<U>(fallback_value)};
        }
        catch (const std::bad_any_cast&) {
            throw type_error::invalid_value_type<T>(arg->name());
        }
    }

    /**
     * @tparam T Type of the argument values.
     * @param arg_name The name of the argument.
     * @return The values of the argument as a vector.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <detail::c_argument_value_type T = std::string>
    [[nodiscard]] std::vector<T> values(std::string_view arg_name) const {
        const auto arg = this->_get_argument(arg_name);
        if (not arg)
            throw lookup_failure::argument_not_found(arg_name);

        try {
            std::vector<T> values;
            // TODO: use std::ranges::to after transition to C++23
            std::ranges::copy(
                detail::any_range_cast_view<T>(arg->values()), std::back_inserter(values)
            );
            return values;
        }
        catch (const std::bad_any_cast&) {
            throw type_error::invalid_value_type<T>(arg->name());
        }
    }

    /**
     * @brief Prints the argument parser's details to an output stream.
     * @param verbose The verbosity mode value.
     * @param os Output stream.
     */
    void print_config(const bool verbose, std::ostream& os = std::cout) const noexcept {
        if (this->_program_name) {
            os << "Program: " << this->_program_name.value();
            if (this->_program_version)
                os << " (" << this->_program_version.value() << ')';
            os << '\n';
        }

        if (this->_program_description)
            os << '\n'
               << std::string(this->_indent_width, ' ') << this->_program_description.value()
               << '\n';

        if (not this->_positional_args.empty()) {
            os << "\nPositional arguments:\n";
            this->_print(os, this->_positional_args, verbose);
        }

        if (not this->_optional_args.empty()) {
            os << "\nOptional arguments:\n";
            this->_print(os, this->_optional_args, verbose);
        }
    }

    /**
     * @brief Prints the argument parser's details to an output stream.
     *
     * An `os << parser` operation is equivalent to a `parser.print_config(_verbose, os)` call,
     * where `_verbose` is the inner verbosity mode, which can be set with the @ref verbose function.
     *
     * @param os Output stream.
     * @param parser The argument parser to print.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const argument_parser& parser) noexcept {
        parser.print_config(parser._verbose, os);
        return os;
    }

#ifdef AP_TESTING
    /// @brief Friend struct for testing purposes.
    friend struct ::ap_testing::argument_parser_test_fixture;
#endif

private:
    using arg_ptr_t = std::shared_ptr<detail::argument_base>;
    using arg_ptr_list_t = std::vector<arg_ptr_t>;
    using arg_ptr_list_iter_t = typename arg_ptr_list_t::iterator;
    using arg_ptr_opt_t = detail::uptr_opt_t<detail::argument_base>;
    using const_arg_opt_t = std::optional<std::reference_wrapper<const detail::argument_base>>;

    using arg_token_list_t = std::vector<detail::argument_token>;
    using arg_token_list_iter_t = typename arg_token_list_t::const_iterator;

    /**
     * @brief Verifies the pattern of an argument name and if it's invalid, an error is thrown
     * @throws ap::invalid_configuration
     */
    void _verify_arg_name_pattern(const std::string_view arg_name) const {
        if (arg_name.empty())
            throw invalid_configuration::invalid_argument_name(
                arg_name, "An argument name cannot be empty."
            );

        if (detail::contains_whitespaces(arg_name))
            throw invalid_configuration::invalid_argument_name(
                arg_name, "An argument name cannot contain whitespaces."
            );

        if (arg_name.front() == this->_flag_prefix_char)
            throw invalid_configuration::invalid_argument_name(
                arg_name,
                std::format(
                    "An argument name cannot begin with a flag prefix character ({}).",
                    this->_flag_prefix_char
                )
            );

        if (std::isdigit(arg_name.front()))
            throw invalid_configuration::invalid_argument_name(
                arg_name, "An argument name cannot begin with a digit."
            );
    }

    /**
     * @brief Returns a unary predicate function which checks if the given name matches the argument's name
     * @param arg_name The name of the argument.
     * @param m_type The match type used within the predicate.
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] auto _name_match_predicate(
        const std::string_view arg_name,
        const detail::argument_name::match_type m_type = detail::argument_name::m_any
    ) const noexcept {
        return [=](const arg_ptr_t& arg) { return arg->name().match(arg_name, m_type); };
    }

    /**
     * @brief Returns a unary predicate function which checks if the given name matches the argument's name
     * @param arg_name The name of the argument.
     * @param m_type The match type used within the predicate.
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] auto _name_match_predicate(
        const detail::argument_name& arg_name,
        const detail::argument_name::match_type m_type = detail::argument_name::m_any
    ) const noexcept {
        return [&arg_name, m_type](const arg_ptr_t& arg) {
            return arg->name().match(arg_name, m_type);
        };
    }

    /**
     * @brief Check if an argument name is already used.
     * @param arg_name The name of the argument.
     * @param m_type The match type used to find the argument.
     * @return True if the argument name is already used, false otherwise.
     */
    [[nodiscard]] bool _is_arg_name_used(
        const detail::argument_name& arg_name,
        const detail::argument_name::match_type m_type = detail::argument_name::m_any
    ) const noexcept {
        const auto predicate = this->_name_match_predicate(arg_name, m_type);

        if (std::ranges::find_if(this->_positional_args, predicate) != this->_positional_args.end())
            return true;

        if (std::ranges::find_if(this->_optional_args, predicate) != this->_optional_args.end())
            return true;

        return false;
    }

    /**
     * @brief Validate whether the definition/configuration of the parser's arguments is correct.
     *
     * What is verified:
     * 1. No required positional argument can be added after a non-required positional argument.
     */
    void _validate_argument_configuration() const {
        // step 1
        const_arg_opt_t non_required_arg = std::nullopt;
        for (const auto& arg : this->_positional_args) {
            if (not arg->is_required()) {
                non_required_arg = std::ref(*arg);
                continue;
            }

            if (non_required_arg and arg->is_required())
                throw invalid_configuration::positional::required_after_non_required(
                    arg->name(), non_required_arg->get().name()
                );
        }
    }

    /**
     * @brief Converts the command-line arguments into a list of tokens.
     * @tparam AR The command-line argument value range type.
     * @param arg_range The command-line argument value range.
     * @return A list of preprocessed command-line argument tokens.
     */
    template <detail::c_sized_range_of<std::string_view, detail::type_validator::convertible> AR>
    [[nodiscard]] arg_token_list_t _tokenize(const AR& arg_range) {
        const auto n_args = std::ranges::size(arg_range);

        arg_token_list_t toks;
        toks.reserve(n_args);
        std::ranges::for_each(
            arg_range, std::bind_front(&argument_parser::_tokenize_arg, this, std::ref(toks))
        );

        return toks;
    }

    /**
     * @brief Appends an argument token(s) created from `arg_value` to the `toks` vector.
     * @param toks The argument token list to which the processed token(s) will be appended.
     * @param arg_value The command-line argument's value to be processed.
     */
    void _tokenize_arg(arg_token_list_t& toks, const std::string_view arg_value) {
        auto tok = this->_build_token(arg_value);

        if (not tok.is_flag_token() or this->_validate_flag_token(tok)) {
            toks.emplace_back(std::move(tok));
            return;
        }

        // invalid flag - check for compound secondary flag
        const auto compound_toks = this->_try_split_compound_flag(tok);
        if (not compound_toks.empty()) { // not a valid compound flag
            toks.insert(toks.end(), compound_toks.begin(), compound_toks.end());
            return;
        }

#ifdef AP_UNKNOWN_FLAGS_AS_VALUES
        toks.emplace_back(detail::argument_token::t_value, std::string(arg_value));
#else
        toks.emplace_back(std::move(tok));
#endif
    }

    /**
     * @brief Builds an argument token from the given value.
     * @param arg_value The command-line argument's value to be processed.
     * @return An argument token with removed flag prefix (if present) and an adequate token type.
     */
    [[nodiscard]] detail::argument_token _build_token(const std::string_view arg_value
    ) const noexcept {
        if (detail::contains_whitespaces(arg_value))
            return {.type = detail::argument_token::t_value, .value = std::string(arg_value)};

        if (arg_value.starts_with(this->_flag_prefix))
            return {
                .type = detail::argument_token::t_flag_primary,
                .value = std::string(arg_value.substr(this->_primary_flag_prefix_length))
            };

        if (arg_value.starts_with(this->_flag_prefix_char))
            return {
                .type = detail::argument_token::t_flag_secondary,
                .value = std::string(arg_value.substr(this->_secondary_flag_prefix_length))
            };

        return {.type = detail::argument_token::t_value, .value = std::string(arg_value)};
    }

    /**
     * @brief Check if a flag token is valid based on its value.
     * @attention Sets the `arg` member of the token if an argument with the given name (token's value) is present.
     * @param tok The argument token to validate.
     * @return true if the given token represents a valid argument flag.
     */
    [[nodiscard]] bool _validate_flag_token(detail::argument_token& tok) noexcept {
        const auto opt_arg_it = this->_find_opt_arg(tok);
        if (opt_arg_it == this->_optional_args.end())
            return false;

        tok.arg = *opt_arg_it;
        return true;
    }

    /**
     * @brief Get the unstripped token value (including the flag prefix).
     *
     * Given an argument token, this function reconstructs and returns the original argument string,
     * including any flag prefix that may have been stripped during tokenization.
     *
     * @param tok An argument token, the value of which will be processed.
     * @return The reconstructed argument value:
     *   - If the token type is `t_flag_primary`, returns the value prefixed with "--".
     *   - If the token type is `t_flag_secondary`, returns the value prefixed with "-".
     *   - For all other token types, returns the token's value as is (without any prefix).
     */
    [[nodiscard]] std::string _unstripped_token_value(const detail::argument_token& tok
    ) const noexcept {
        switch (tok.type) {
        case detail::argument_token::t_flag_primary:
            return std::format("{}{}", this->_flag_prefix, tok.value);
        case detail::argument_token::t_flag_secondary:
            return std::format("{}{}", this->_flag_prefix_char, tok.value);
        default:
            return tok.value;
        }
    }

    /**
     * @brief Tries to split a secondary flag token into separate flag token (one for each character of the token's value).
     * @param tok The token to be processed.
     * @return A vector of new argument tokens.
     * @note If ANY of the characters in the token's value does not match an argument, an empty vector will be returned.
     */
    [[nodiscard]] std::vector<detail::argument_token> _try_split_compound_flag(
        const detail::argument_token& tok
    ) noexcept {
        std::vector<detail::argument_token> compound_toks;
        compound_toks.reserve(tok.value.size());

        if (tok.type != detail::argument_token::t_flag_secondary)
            return compound_toks;

        for (const char c : tok.value) {
            detail::argument_token ctok{
                detail::argument_token::t_flag_secondary, std::string(1ull, c)
            };
            if (not this->_validate_flag_token(ctok)) {
                compound_toks.clear();
                return compound_toks;
            }
            compound_toks.emplace_back(std::move(ctok));
        }

        return compound_toks;
    }

    /**
     * @brief Implementation of parsing command-line arguments.
     * @param arg_tokens The list of command-line argument tokens.
     * @param handle_unknown A flag specifying whether unknown arguments should be handled or collected.
     * @throws ap::parsing_failure
     */
    void _parse_args_impl(
        const arg_token_list_t& arg_tokens,
        std::vector<std::string>& unknown_args,
        const bool handle_unknown = true
    ) {
        // set the current argument indicators
        arg_ptr_t curr_arg_opt = nullptr;
        arg_ptr_list_iter_t curr_positional_arg_it = this->_positional_args.begin();

        if (curr_positional_arg_it != this->_positional_args.end())
            curr_arg_opt = *curr_positional_arg_it;

        // process argument tokens
        std::ranges::for_each(
            arg_tokens,
            std::bind_front(
                &argument_parser::_parse_token,
                this,
                std::ref(curr_arg_opt),
                std::ref(curr_positional_arg_it),
                std::ref(unknown_args),
                handle_unknown
            )
        );
    }

    /**
     * @brief Parse a single command-line argument token.
     * @param curr_arg_opt The currently processed argument.
     * @param curr_positional_arg_it An iterator pointing to the current positional argument.
     * @param unknown_args The unknown arguments collection.
     * @param handle_unknown A flag specifying whether unknown arguments should be handled or collected.
     * @param tok The argument token to be processed.
     * @throws ap::parsing_failure
     */
    void _parse_token(
        arg_ptr_t& curr_arg_opt,
        arg_ptr_list_iter_t& curr_positional_arg_it,
        std::vector<std::string>& unknown_args,
        const bool handle_unknown,
        const detail::argument_token& tok
    ) {
        switch (tok.type) {
        case detail::argument_token::t_flag_primary:
            [[fallthrough]];
        case detail::argument_token::t_flag_secondary: {
            if (not tok.is_valid_flag_token()) {
                if (handle_unknown) {
                    throw parsing_failure::unrecognized_argument(this->_unstripped_token_value(tok)
                    );
                }
                else {
                    curr_arg_opt.reset();
                    unknown_args.emplace_back(this->_unstripped_token_value(tok));
                    break;
                }
            }

            if (tok.arg->mark_used())
                curr_arg_opt = tok.arg;
            else
                curr_arg_opt.reset();

            break;
        }
        case detail::argument_token::t_value: {
            if (not curr_arg_opt) {
                if (curr_positional_arg_it == this->_positional_args.end()) {
                    unknown_args.emplace_back(tok.value);
                    break;
                }

                curr_arg_opt = *curr_positional_arg_it;
            }

            if (auto& curr_arg = *curr_arg_opt; not curr_arg.set_value(std::string(tok.value))) {
                // advance to the next positional argument if possible
                if (curr_arg.is_positional()
                    and curr_positional_arg_it != this->_positional_args.end()
                    and ++curr_positional_arg_it != this->_positional_args.end()) {
                    curr_arg_opt = *curr_positional_arg_it;
                    break;
                }

                curr_arg_opt.reset();
            }

            break;
        }
        }
    }

    /**
     * @brief Check whether required argument bypassing is enabled
     * @return true if at least one argument with enabled required argument bypassing is used, false otherwise.
     */
    [[nodiscard]] bool _are_required_args_bypassed() const noexcept {
        // TODO: use std::views::join after the transition to C++23
        return std::ranges::any_of(
                   this->_positional_args,
                   [](const arg_ptr_t& arg) {
                       return arg->is_used() and arg->is_bypass_required_enabled();
                   }
               )
            or std::ranges::any_of(this->_optional_args, [](const arg_ptr_t& arg) {
                   return arg->is_used() and arg->is_bypass_required_enabled();
               });
    }

    /**
     * @brief Check if all required positional and optional arguments are used.
     * @throws ap::parsing_failure
     */
    void _verify_required_args() const {
        // TODO: use std::views::join after the transition to C++23
        for (const auto& arg : this->_positional_args)
            if (arg->is_required() and not arg->has_value())
                throw parsing_failure::required_argument_not_parsed(arg->name());

        for (const auto& arg : this->_optional_args)
            if (arg->is_required() and not arg->has_value())
                throw parsing_failure::required_argument_not_parsed(arg->name());
    }

    /**
     * @brief Check if the number of argument values is within the specified range.
     * @throws ap::parsing_failure
     */
    void _verify_nvalues() const {
        // TODO: use std::views::join after the transition to C++23
        for (const auto& arg : this->_positional_args)
            if (const auto nv_ord = arg->nvalues_ordering(); not std::is_eq(nv_ord))
                throw parsing_failure::invalid_nvalues(arg->name(), nv_ord);

        for (const auto& arg : this->_optional_args)
            if (const auto nv_ord = arg->nvalues_ordering(); not std::is_eq(nv_ord))
                throw parsing_failure::invalid_nvalues(arg->name(), nv_ord);
    }

    /**
     * @brief Get the argument with the specified name.
     * @param arg_name The name of the argument.
     * @return The argument with the specified name, if found; otherwise, std::nullopt.
     */
    arg_ptr_t _get_argument(std::string_view arg_name) const noexcept {
        const auto predicate = this->_name_match_predicate(arg_name);

        if (auto pos_arg_it = std::ranges::find_if(this->_positional_args, predicate);
            pos_arg_it != this->_positional_args.end()) {
            return *pos_arg_it;
        }

        if (auto opt_arg_it = std::ranges::find_if(this->_optional_args, predicate);
            opt_arg_it != this->_optional_args.end()) {
            return *opt_arg_it;
        }

        return nullptr;
    }

    /**
     * @brief Find an optional argument based on a flag token.
     * @param flag_tok An argument_token instance, the value of which will be used to find the argument.
     * @return An iterator to the argument's position.
     * @note If the `flag_tok.type` is not a valid flag token, then the end iterator will be returned.
     */
    [[nodiscard]] arg_ptr_list_iter_t _find_opt_arg(const detail::argument_token& flag_tok
    ) noexcept {
        switch (flag_tok.type) {
        case detail::argument_token::t_flag_primary:
            return std::ranges::find(this->_optional_args, flag_tok.value, [](const auto& arg_ptr) {
                return arg_ptr->name().primary;
            });
        case detail::argument_token::t_flag_secondary:
            return std::ranges::find(this->_optional_args, flag_tok.value, [](const auto& arg_ptr) {
                return arg_ptr->name().secondary;
            });
        default:
            return this->_optional_args.end();
        }
    }

    /**
     * @brief Print the given argument list to an output stream.
     * @param os The output stream to print to.
     * @param args The argument list to print.
     */
    void _print(std::ostream& os, const arg_ptr_list_t& args, const bool verbose) const noexcept {
        auto visible_args =
            std::views::filter(args, [](const auto& arg) { return not arg->is_hidden(); });

        if (verbose) {
            for (const auto& arg : visible_args)
                os << '\n' << arg->desc(verbose).get(this->_indent_width) << '\n';
        }
        else {
            std::vector<detail::argument_descriptor> descriptors;
            descriptors.reserve(args.size());

            for (const auto& arg : visible_args)
                descriptors.emplace_back(arg->desc(verbose));

            std::size_t max_arg_name_length = 0ull;
            for (const auto& desc : descriptors)
                max_arg_name_length = std::max(max_arg_name_length, desc.name.length());

            for (const auto& desc : descriptors)
                os << '\n' << desc.get_basic(this->_indent_width, max_arg_name_length);

            os << '\n';
        }
    }

    std::optional<std::string> _program_name;
    std::optional<std::string> _program_version;
    std::optional<std::string> _program_description;
    bool _verbose = false;

    arg_ptr_list_t _positional_args;
    arg_ptr_list_t _optional_args;

    static constexpr uint8_t _primary_flag_prefix_length = 2u;
    static constexpr uint8_t _secondary_flag_prefix_length = 1u;
    static constexpr char _flag_prefix_char = '-';
    static constexpr std::string_view _flag_prefix = "--";
    static constexpr uint8_t _indent_width = 2;
};

namespace detail {

/**
 * @brief Adds a predefined/default positional argument to the parser.
 * @param arg_discriminator The default argument discriminator.
 * @param arg_parser The argument parser to which the argument will be added.
 */
inline void add_default_argument(
    const default_argument arg_discriminator, argument_parser& arg_parser
) noexcept {
    switch (arg_discriminator) {
    case default_argument::p_input:
        arg_parser.add_positional_argument("input")
            .action<action_type::observe>(action::check_file_exists())
            .help("Input file path");
        break;

    case default_argument::p_output:
        arg_parser.add_positional_argument("output").help("Output file path");
        break;

    case default_argument::o_help:
        arg_parser.add_flag("help", "h")
            .action<action_type::on_flag>(action::print_config(arg_parser, EXIT_SUCCESS))
            .help("Display the help message");
        break;

    case default_argument::o_input:
        arg_parser.add_optional_argument("input", "i")
            .required()
            .nargs(1ull)
            .action<action_type::observe>(action::check_file_exists())
            .help("Input file path");
        break;

    case default_argument::o_output:
        arg_parser.add_optional_argument("output", "o")
            .required()
            .nargs(1ull)
            .help("Output file path");
        break;

    case default_argument::o_multi_input:
        arg_parser.add_optional_argument("input", "i")
            .required()
            .nargs(ap::nargs::at_least(1ull))
            .action<action_type::observe>(action::check_file_exists())
            .help("Input files paths");
        break;

    case default_argument::o_multi_output:
        arg_parser.add_optional_argument("output", "o")
            .required()
            .nargs(ap::nargs::at_least(1ull))
            .help("Output files paths");
        break;
    }
}

} // namespace detail

} // namespace ap
