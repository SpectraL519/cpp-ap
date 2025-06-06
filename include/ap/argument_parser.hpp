// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file argument_parser.hpp
 * @brief Main library header file. Defines the `argument_parser` class.
 */

#pragma once

#include "argument/default.hpp"
#include "argument/optional.hpp"
#include "argument/positional.hpp"
#include "detail/argument_token.hpp"
#include "detail/concepts.hpp"

#include <algorithm>
#include <format>
#include <ranges>
#include <span>

#ifdef AP_TESTING

namespace ap_testing {
struct argument_parser_test_fixture;
} // namespace ap_testing

#endif

namespace ap {

// TODO: argument namespace alias

class argument_parser;

namespace detail {

void add_default_argument(const argument::default_positional, argument_parser&) noexcept;
void add_default_argument(const argument::default_optional, argument_parser&) noexcept;

} // namespace detail

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
    argument_parser& program_name(std::string_view name) noexcept {
        this->_program_name = name;
        return *this;
    }

    /**
     * @brief Set the program description.
     * @param description The description of the program.
     * @return Reference to the argument parser.
     */
    argument_parser& program_description(std::string_view description) noexcept {
        this->_program_description = description;
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
     * @brief Set default positional arguments.
     * @tparam AR Type of the positional argument discriminator range.
     * @param arg_discriminator_range A range of default positional argument discriminators.
     * @return Reference to the argument parser.
     */
    template <detail::c_range_of<argument::default_positional> AR>
    argument_parser& default_positional_arguments(const AR& arg_discriminator_range) noexcept {
        for (const auto arg_discriminator : arg_discriminator_range)
            detail::add_default_argument(arg_discriminator, *this);
        return *this;
    }

    /**
     * @brief Set default positional arguments.
     * @param arg_discriminator_list A list of default positional argument discriminators.
     * @return Reference to the argument parser.
     */
    argument_parser& default_positional_arguments(
        const std::initializer_list<argument::default_positional> arg_discriminator_list
    ) noexcept {
        return this->default_positional_arguments<>(arg_discriminator_list);
    }

    /**
     * @brief Set default optional arguments.
     * @tparam AR Type of the optional argument discriminator range.
     * @param arg_discriminator_range A range of default optional argument discriminators.
     * @return Reference to the argument parser.
     */
    template <detail::c_range_of<argument::default_optional> AR>
    argument_parser& default_optional_arguments(const AR& arg_discriminator_range) noexcept {
        for (const auto arg_discriminator : arg_discriminator_range)
            detail::add_default_argument(arg_discriminator, *this);
        return *this;
    }

    /**
     * @brief Set default optional arguments.
     * @param arg_discriminator_list A list of default optional argument discriminators.
     * @return Reference to the argument parser.
     */
    argument_parser& default_optional_arguments(
        const std::initializer_list<argument::default_optional> arg_discriminator_list
    ) noexcept {
        return this->default_optional_arguments<>(arg_discriminator_list);
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @return Reference to the added positional argument.
     * @throws ap::invalid_configuration
     *
     * \todo Check forbidden characters (after adding the assignment character).
     */
    template <detail::c_argument_value_type T = std::string>
    argument::positional<T>& add_positional_argument(std::string_view primary_name) {
        this->_verify_arg_name_pattern(primary_name);

        const detail::argument_name arg_name = {primary_name};
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_positional_args.emplace_back(std::make_unique<argument::positional<T>>(arg_name));
        return static_cast<argument::positional<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added positional argument.
     * @throws ap::invalid_configuration
     *
     * \todo Check forbidden characters (after adding the assignment character).
     */
    template <detail::c_argument_value_type T = std::string>
    argument::positional<T>& add_positional_argument(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        this->_verify_arg_name_pattern(primary_name);
        this->_verify_arg_name_pattern(secondary_name);

        const detail::argument_name arg_name = {primary_name, secondary_name};
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_positional_args.emplace_back(std::make_unique<argument::positional<T>>(arg_name));
        return static_cast<argument::positional<T>&>(*this->_positional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @return Reference to the added optional argument.
     * @throws ap::invalid_configuration
     *
     * \todo Check forbidden characters (after adding the assignment character).
     */
    template <detail::c_argument_value_type T = std::string>
    argument::optional<T>& add_optional_argument(std::string_view primary_name) {
        this->_verify_arg_name_pattern(primary_name);

        const detail::argument_name arg_name = {primary_name};
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_optional_args.push_back(std::make_unique<argument::optional<T>>(arg_name));
        return static_cast<argument::optional<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a positional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added optional argument.
     * @throws ap::invalid_configuration
     *
     * \todo Check forbidden characters (after adding the assignment character).
     */
    template <detail::c_argument_value_type T = std::string>
    argument::optional<T>& add_optional_argument(
        std::string_view primary_name, std::string_view secondary_name
    ) {
        this->_verify_arg_name_pattern(primary_name);
        this->_verify_arg_name_pattern(secondary_name);

        const detail::argument_name arg_name = {primary_name, secondary_name};
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        this->_optional_args.emplace_back(std::make_unique<argument::optional<T>>(arg_name));
        return static_cast<argument::optional<T>&>(*this->_optional_args.back());
    }

    /**
     * @brief Adds a boolean flag argument (an optional argument with `value_type = bool`) to the parser's configuration.
     * @tparam StoreImplicitly A boolean value used as the `implicit_value` parameter of the argument.
     * @param primary_name The primary name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    argument::optional<bool>& add_flag(std::string_view primary_name) {
        return this->add_optional_argument<bool>(primary_name)
            .default_value(not StoreImplicitly)
            .implicit_value(StoreImplicitly)
            .nargs(0ull);
    }

    /**
     * @brief Adds a boolean flag argument (an optional argument with `value_type = bool`) to the parser's configuration.
     * @tparam StoreImplicitly A boolean value used as the `implicit_value` parameter of the argument.
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
            .nargs(0ull);
    }

    /**
     * @brief Parses the command-line arguments.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument values.
     * @note The first argument (the program name) is ignored.
     */
    void parse_args(int argc, char* argv[]) {
        this->parse_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)));
    }

    /**
     * @brief Parses the command-line arguments.
     * @tparam AR The argument range type.
     * @param argv A range of command-line argument values.
     */
    template <detail::c_range_of<std::string, detail::type_validator::convertible> AR>
    void parse_args(const AR& argv) {
        this->_parse_args_impl(this->_tokenize(argv));

        if (this->_are_required_args_bypassed())
            return;

        this->_verify_required_args();
        this->_verify_nvalues();
    }

    /**
     * @brief Parses the command-line arguments and exits on error.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument values.
     * @note The first argument (the program name) is ignored.
     */
    void try_parse_args(int argc, char* argv[]) {
        this->try_parse_args(std::span(argv + 1, static_cast<std::size_t>(argc - 1)));
    }

    /**
     * @brief Parses the command-line arguments and exits on error.
     *
     * Calls `parse_args(argv)` in a try-catch block. If an error is thrown, then its
     * message and the parser are printed to `std::cerr` and the function exists with
     * `EXIT_FAILURE` status.
     *
     * @tparam AR The argument range type.
     * @param argv A range of command-line argument values.
     */
    template <detail::c_range_of<std::string, detail::type_validator::convertible> AR>
    void try_parse_args(const AR& argv) {
        try {
            this->parse_args(argv);
        }
        catch (const ap::argument_parser_exception& err) {
            std::cerr << "[ERROR] : " << err.what() << std::endl << *this << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    /**
     * @brief Handles the `help` argument logic.
     *
     * Checks the value of the `help` boolean flag argument and if the value `is` true,
     * prints the parser to `std::cout` anb exists with `EXIT_SUCCESS` status.
     */
    // clang-format off
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
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().has_value() : false;
    }

    /**
     * @param arg_name The name of the argument.
     * @return The count of times the argument has been used.
     */
    [[nodiscard]] std::size_t count(std::string_view arg_name) const noexcept {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().count() : 0ull;
    }

    /**
     * @tparam T Type of the argument value.
     * @param arg_name The name of the argument.
     * @return The value of the argument.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <detail::c_argument_value_type T = std::string>
    [[nodiscard]] T value(std::string_view arg_name) const {
        const auto arg_opt = this->_get_argument(arg_name);
        if (not arg_opt)
            throw lookup_failure::argument_not_found(arg_name);

        const auto& arg_value = arg_opt->get().value();
        try {
            return std::any_cast<T>(arg_value);
        }
        catch (const std::bad_any_cast& err) {
            throw type_error::invalid_value_type(arg_opt->get().name(), typeid(T));
        }
    }

    /**
     * @tparam T Type of the argument value.
     * @tparam U The default value type.
     * @param arg_name The name of the argument.
     * @param default_value The default value.
     * @return The value of the argument.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <detail::c_argument_value_type T = std::string, std::convertible_to<T> U>
    [[nodiscard]] T value_or(std::string_view arg_name, U&& default_value) const {
        const auto arg_opt = this->_get_argument(arg_name);
        if (not arg_opt)
            throw lookup_failure::argument_not_found(arg_name);

        try {
            const auto& arg_value = arg_opt->get().value();
            return std::any_cast<T>(arg_value);
        }
        catch (const std::logic_error&) {
            // positional: no value parsed
            // optional: no value parsed + no predefined value
            return T{std::forward<U>(default_value)};
        }
        catch (const std::bad_any_cast& err) {
            throw type_error::invalid_value_type(arg_opt->get().name(), typeid(T));
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
        const auto arg_opt = this->_get_argument(arg_name);
        if (not arg_opt)
            throw lookup_failure::argument_not_found(arg_name);

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
            throw type_error::invalid_value_type(arg.name(), typeid(T));
        }
    }

    /**
     * @brief Prints the argument parser's details to an output stream.
     * @param verbose The verbosity mode value.
     * @param os Output stream.
     */
    void print_config(const bool verbose, std::ostream& os = std::cout) const noexcept {
        if (this->_program_name)
            os << "Program: " << this->_program_name.value() << std::endl;

        if (this->_program_description)
            os << "\n"
               << std::string(this->_indent_width, ' ') << this->_program_description.value()
               << std::endl;

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
    using arg_ptr_t = std::unique_ptr<detail::argument_base>;
    using arg_ptr_list_t = std::vector<arg_ptr_t>;
    using arg_opt_t = std::optional<std::reference_wrapper<detail::argument_base>>;

    using arg_token_list_t = std::vector<detail::argument_token>;
    using arg_token_list_iterator_t = typename arg_token_list_t::const_iterator;

    /**
     * @brief Verifies the pattern of an argument name and if it's invalid, an error is thrown
     * @throws ap::invalid_configuration
     */
    void _verify_arg_name_pattern(const std::string_view arg_name) const {
        if (arg_name.empty())
            throw invalid_configuration::invalid_argument_name(
                arg_name, "An argument name cannot be empty."
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
     * @return Argument predicate based on the provided name.
     */
    [[nodiscard]] auto _name_match_predicate(const std::string_view arg_name) const noexcept {
        return [arg_name](const arg_ptr_t& arg) { return arg->name().match(arg_name); };
    }

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
    [[nodiscard]] bool _is_arg_name_used(const detail::argument_name& arg_name) const noexcept {
        const auto predicate = this->_name_match_predicate(arg_name);

        if (std::ranges::find_if(this->_positional_args, predicate) != this->_positional_args.end())
            return true;

        if (std::ranges::find_if(this->_optional_args, predicate) != this->_optional_args.end())
            return true;

        return false;
    }

    /**
     * @brief Converts the command-line arguments into a list of tokens.
     * @tparam AR The command-line argument value range type.
     * @param arg_range The command-line argument value range.
     * @return A list of preprocessed command-line argument tokens.
     */
    template <detail::c_sized_range_of<std::string, detail::type_validator::convertible> AR>
    [[nodiscard]] arg_token_list_t _tokenize(const AR& arg_range) const noexcept {
        const auto n_args = std::ranges::size(arg_range);
        if (n_args == 0ull)
            return arg_token_list_t{};

        arg_token_list_t toks;
        toks.reserve(n_args);

        for (const auto& arg : arg_range) {
            std::string value = static_cast<std::string>(arg);
            if (this->_is_flag(value)) {
                this->_strip_flag_prefix(value);
                toks.emplace_back(detail::argument_token::t_flag, std::move(value));
            }
            else {
                toks.emplace_back(detail::argument_token::t_value, std::move(value));
            }
        }

        return toks;
    }

    /**
     * @brief Check if an argument is a flag based on its value.
     * @param arg The cmd argument's value.
     * @return True if the argument is a flag, false otherwise.
     */
    [[nodiscard]] bool _is_flag(const std::string& arg) const noexcept {
        if (arg.starts_with(this->_flag_prefix))
            return this->_is_arg_name_used({arg.substr(this->_primary_flag_prefix_length)});

        if (arg.starts_with(this->_flag_prefix_char))
            return this->_is_arg_name_used({arg.substr(this->_secondary_flag_prefix_length)});

        return false;
    }

    /**
     * @brief Remove the flag prefix from the argument.
     * @param arg_flag The argument flag to strip the prefix from.
     */
    void _strip_flag_prefix(std::string& arg_flag) const noexcept {
        if (arg_flag.starts_with(this->_flag_prefix))
            arg_flag.erase(0, this->_primary_flag_prefix_length);
        else
            arg_flag.erase(0, this->_secondary_flag_prefix_length);
    }

    /**
     * @brief Implementation of parsing command-line arguments.
     * @param arg_tokens The list of command-line argument tokens.
     * @throws ap::parsing_failure
     */
    void _parse_args_impl(const arg_token_list_t& arg_tokens) {
        arg_token_list_iterator_t token_it = arg_tokens.begin();

        this->_parse_positional_args(token_it, arg_tokens.end());

        std::vector<std::string_view> dangling_values;
        this->_parse_optional_args(token_it, arg_tokens.end(), dangling_values);

        if (not dangling_values.empty())
            throw parsing_failure::argument_deduction_failure(dangling_values);
    }

    /**
     * @brief Parse positional arguments based on command-line input.
     * @param token_it Iterator for iterating through command-line argument tokens.
     * @param tokens_end The token list end iterator.
     */
    void _parse_positional_args(
        arg_token_list_iterator_t& token_it, const arg_token_list_iterator_t& tokens_end
    ) noexcept {
        for (const auto& pos_arg : this->_positional_args) {
            if (token_it == tokens_end)
                return;

            if (token_it->type == detail::argument_token::t_flag)
                return;

            pos_arg->set_value(token_it->value);
            ++token_it;
        }
    }

    /**
     * @brief Parse optional arguments based on command-line input.
     * @param token_it Iterator for iterating through command-line argument tokens.
     * @param tokens_end The token list end iterator.
     * @param dangling_values Reference to the vector into which the dangling values shall be collected.
     * @throws ap::parsing_failure
     * \todo Enable/disable argument_deduction_failure for the purpose of `parse_known_args` functionality
     */
    void _parse_optional_args(
        arg_token_list_iterator_t& token_it,
        const arg_token_list_iterator_t& tokens_end,
        std::vector<std::string_view>& dangling_values
    ) {
        std::optional<std::reference_wrapper<arg_ptr_t>> curr_opt_arg;

        while (token_it != tokens_end) {
            switch (token_it->type) {
            case detail::argument_token::t_flag: {
                if (not dangling_values.empty())
                    throw parsing_failure::argument_deduction_failure(dangling_values);

                auto opt_arg_it = std::ranges::find_if(
                    this->_optional_args, this->_name_match_predicate(token_it->value)
                );

                if (opt_arg_it == this->_optional_args.end())
                    throw parsing_failure::unknown_argument(token_it->value);

                curr_opt_arg = std::ref(*opt_arg_it);
                if (not curr_opt_arg->get()->mark_used())
                    curr_opt_arg.reset();

                break;
            }
            case detail::argument_token::t_value: {
                if (not curr_opt_arg) {
                    dangling_values.emplace_back(token_it->value);
                    break;
                }

                if (not curr_opt_arg->get()->set_value(token_it->value))
                    curr_opt_arg.reset();

                break;
            }
            }

            ++token_it;
        }
    }

    /**
     * @brief Check if optional arguments can bypass the required arguments.
     * @return True if optional arguments can bypass required arguments, false otherwise.
     */
    [[nodiscard]] bool _are_required_args_bypassed() const noexcept {
        return std::ranges::any_of(this->_optional_args, [](const arg_ptr_t& arg) {
            return arg->is_used() and arg->bypass_required_enabled();
        });
    }

    /**
     * @brief Check if all required positional and optional arguments are used.
     * @throws ap::parsing_failure
     */
    void _verify_required_args() const {
        for (const auto& arg : this->_positional_args)
            if (not arg->is_used()) // ? use has_parsed_values
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
    arg_opt_t _get_argument(std::string_view arg_name) const noexcept {
        const auto predicate = this->_name_match_predicate(arg_name);

        if (auto pos_arg_it = std::ranges::find_if(this->_positional_args, predicate);
            pos_arg_it != this->_positional_args.end()) {
            return std::ref(**pos_arg_it);
        }

        if (auto opt_arg_it = std::ranges::find_if(this->_optional_args, predicate);
            opt_arg_it != this->_optional_args.end()) {
            return std::ref(**opt_arg_it);
        }

        return std::nullopt;
    }

    /**
     * @brief Print the given argument list to an output stream.
     * @param os The output stream to print to.
     * @param args The argument list to print.
     */
    void _print(std::ostream& os, const arg_ptr_list_t& args, const bool verbose) const noexcept {
        if (verbose) {
            for (const auto& arg : args)
                os << '\n'
                   << arg->desc(verbose, this->_flag_prefix_char).get(this->_indent_width) << '\n';
        }
        else {
            std::vector<detail::argument_descriptor> descriptors;
            descriptors.reserve(args.size());

            for (const auto& arg : args)
                descriptors.emplace_back(arg->desc(verbose, this->_flag_prefix_char));

            std::size_t max_arg_name_length = 0ull;
            for (const auto& desc : descriptors)
                max_arg_name_length = std::max(max_arg_name_length, desc.name.length());

            for (const auto& desc : descriptors)
                os << '\n' << desc.get_basic(this->_indent_width, max_arg_name_length);

            os << '\n';
        }
    }

    std::optional<std::string> _program_name;
    std::optional<std::string> _program_description;
    bool _verbose = false;

    arg_ptr_list_t _positional_args;
    arg_ptr_list_t _optional_args;

    static constexpr uint8_t _primary_flag_prefix_length = 2u;
    static constexpr uint8_t _secondary_flag_prefix_length = 1u;
    static constexpr char _flag_prefix_char = '-';
    static constexpr std::string _flag_prefix = "--";
    static constexpr uint8_t _indent_width = 2;
};

namespace detail {

/**
 * @brief Adds a predefined/default positional argument to the parser.
 * @param arg_discriminator The default argument discriminator.
 * @param arg_parser The argument parser to which the argument will be added.
 */
inline void add_default_argument(
    const argument::default_positional arg_discriminator, argument_parser& arg_parser
) noexcept {
    switch (arg_discriminator) {
    case argument::default_positional::input:
        arg_parser.add_positional_argument("input")
            .action<action_type::observe>(action::check_file_exists())
            .help("Input file path");
        break;

    case argument::default_positional::output:
        arg_parser.add_positional_argument("output").help("Output file path");
        break;
    }
}

/**
 * @brief Adds a predefined/default optional argument to the parser.
 * @param arg_discriminator The default argument discriminator.
 * @param arg_parser The argument parser to which the argument will be added.
 */
inline void add_default_argument(
    const argument::default_optional arg_discriminator, argument_parser& arg_parser
) noexcept {
    switch (arg_discriminator) {
    case argument::default_optional::help:
        arg_parser.add_flag("help", "h")
            .action<action_type::on_flag>(action::print_config(arg_parser, EXIT_SUCCESS))
            .help("Display the help message");
        break;

    case argument::default_optional::input:
        arg_parser.add_optional_argument("input", "i")
            .required()
            .nargs(1)
            .action<action_type::observe>(action::check_file_exists())
            .help("Input file path");
        break;

    case argument::default_optional::output:
        arg_parser.add_optional_argument("output", "o").required().nargs(1).help("Output file path");
        break;

    case argument::default_optional::multi_input:
        arg_parser.add_optional_argument("input", "i")
            .required()
            .nargs(ap::nargs::at_least(1))
            .action<action_type::observe>(action::check_file_exists())
            .help("Input files paths");
        break;

    case argument::default_optional::multi_output:
        arg_parser.add_optional_argument("output", "o")
            .required()
            .nargs(ap::nargs::at_least(1))
            .help("Output files paths");
        break;
    }
}

} // namespace detail

} // namespace ap
