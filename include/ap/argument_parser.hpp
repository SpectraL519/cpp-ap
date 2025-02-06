// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "argument/default.hpp"
#include "argument/optional.hpp"
#include "argument/positional.hpp"
#include "detail/argument_token.hpp"
#include "detail/concepts.hpp"

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
     * @brief Set default positional arguments.
     * @param arg_discriminator_list Vector of default positional argument categories.
     * @return Reference to the argument parser.
     */
    argument_parser& default_positional_arguments(
        const std::vector<argument::default_positional>& arg_discriminator_list
    ) noexcept {
        for (const auto arg_discriminator : arg_discriminator_list)
            this->_add_default_positional_argument(arg_discriminator);
        return *this;
    }

    /**
     * @brief Set default optional arguments.
     * @param arg_discriminator_list Vector of default optional argument categories.
     * @return Reference to the argument parser.
     */
    argument_parser& default_optional_arguments(
        const std::vector<argument::default_optional>& arg_discriminator_list
    ) noexcept {
        for (const auto arg_discriminator : arg_discriminator_list)
            this->_add_default_optional_argument(arg_discriminator);
        return *this;
    }

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
            .nargs(0ull);
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
            .nargs(0ull);
    }

    /**
     * @brief Parses the command-line arguments.
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     */
    void parse_args(int argc, char* argv[]) {
        this->_parse_args_impl(this->_tokenize(argc, argv));

        if (this->_are_required_args_bypassed())
            return;

        this->_check_required_args();
        this->_check_nvalues_in_range();
    }

    /**
     * @param arg_name The name of the argument.
     * @return True if the argument has a value, false otherwise.
     */
    bool has_value(std::string_view arg_name) const noexcept {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().has_value() : false;
    }

    /**
     * @param arg_name The name of the argument.
     * @return The count of times the argument has been used.
     */
    std::size_t count(std::string_view arg_name) const noexcept {
        const auto arg_opt = this->_get_argument(arg_name);
        return arg_opt ? arg_opt->get().nused() : 0ull;
    }

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
    friend std::ostream& operator<<(std::ostream& os, const argument_parser& parser) noexcept {
        if (parser._program_name)
            os << parser._program_name.value() << std::endl;

        if (parser._program_description)
            os << parser._program_description.value() << std::endl;

        for (const auto& argument : parser._positional_args)
            os << "\t" << *argument << std::endl;

        for (const auto& argument : parser._optional_args)
            os << "\t" << *argument << std::endl;

        return os;
    }

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
    ) noexcept {
        switch (arg_discriminator) {
        case argument::default_positional::input:
            this->add_positional_argument("input")
                .action<action_type::modify>(action::check_file_exists())
                .help("Input file path");
            break;

        case argument::default_positional::output:
            this->add_positional_argument("output").help("Output file path");
            break;
        }
    }

    // TODO: extract to detail after impl split
    /**
     * @brief Adds a default optional argument based on the specified discriminator.
     * @param arg_discriminator The default optional argument discriminator.
     */
    void _add_default_optional_argument(const argument::default_optional arg_discriminator
    ) noexcept {
        switch (arg_discriminator) {
        case argument::default_optional::help:
            this->add_flag("help", "h").bypass_required().help("Display help message");
            break;

        case argument::default_optional::input:
            this->add_optional_argument("input", "i")
                .required()
                .nargs(1)
                .action<action_type::modify>(action::check_file_exists())
                .help("Input file path");
            break;

        case argument::default_optional::output:
            this->add_optional_argument("output", "o").required().nargs(1).help("Output file path");
            break;

        case argument::default_optional::multi_input:
            this->add_optional_argument("input", "i")
                .required()
                .nargs(ap::nargs::at_least(1))
                .action<action_type::modify>(action::check_file_exists())
                .help("Input files paths");
            break;

        case argument::default_optional::multi_output:
            this->add_optional_argument("output", "o")
                .required()
                .nargs(ap::nargs::at_least(1))
                .help("Output files paths");
            break;
        }
    }

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
     * @param argc Number of command-line arguments.
     * @param argv Array of command-line argument strings.
     * @return List of preprocessed command-line argument tokens.
     */
    [[nodiscard]] arg_token_list_t _tokenize(int argc, char* argv[]) const noexcept {
        if (argc < 2)
            return arg_token_list_t{};

        arg_token_list_t args;
        args.reserve(argc - 1);

        for (int i = 1; i < argc; ++i) {
            std::string value = argv[i];
            if (this->_is_flag(value)) {
                this->_strip_flag_prefix(value);
                args.emplace_back(detail::argument_token::t_flag, std::move(value));
            }
            else {
                args.emplace_back(detail::argument_token::t_value, std::move(value));
            }
        }

        return args;
    }

    /**
     * @brief Check if an argument is a flag based on its value.
     * @param arg The cmd argument's value.
     * @return True if the argument is a flag, false otherwise.
     */
    [[nodiscard]] bool _is_flag(const std::string& arg) const noexcept {
        {
            if (arg.starts_with(this->_flag_prefix))
                return this->_is_arg_name_used({arg.substr(this->_flag_prefix_length)});

            if (arg.starts_with(this->_flag_prefix_char))
                return this->_is_arg_name_used({arg.substr(this->_flag_prefix_char_length)});

            return false;
        }
    }

    /**
     * @brief Remove the flag prefix from the argument.
     * @param arg_flag The argument flag to strip the prefix from.
     */
    void _strip_flag_prefix(std::string& arg_flag) const noexcept {
        if (arg_flag.starts_with(this->_flag_prefix))
            arg_flag.erase(0, this->_flag_prefix_length);
        else
            arg_flag.erase(0, this->_flag_prefix_char_length);
    }

    /**
     * @brief Implementation of parsing command-line arguments.
     * @param arg_tokens The list of command-line argument tokens.
     */
    void _parse_args_impl(const arg_token_list_t& arg_tokens) {
        arg_token_list_iterator_t token_it = arg_tokens.begin();
        this->_parse_positional_args(arg_tokens, token_it);
        this->_parse_optional_args(arg_tokens, token_it);
    }

    /**
     * @brief Parse positional arguments based on command-line input.
     * @param arg_tokens The list of command-line argument tokens.
     * @param token_it Iterator for iterating through command-line argument tokens.
     */
    void _parse_positional_args(
        const arg_token_list_t& arg_tokens, arg_token_list_iterator_t& token_it
    ) noexcept {
        for (const auto& pos_arg : this->_positional_args) {
            if (token_it == arg_tokens.end())
                return;

            if (token_it->type == detail::argument_token::t_flag)
                return;

            pos_arg->set_value(token_it->value);
            ++token_it;
        }
    }

    /**
     * @brief Parse optional arguments based on command-line input.
     * @param arg_tokens The list of command-line argument tokens.
     * @param token_it Iterator for iterating through command-line argument tokens.
     */
    void _parse_optional_args(
        const arg_token_list_t& arg_tokens, arg_token_list_iterator_t& token_it
    ) {
        std::optional<std::reference_wrapper<arg_ptr_t>> curr_opt_arg;

        while (token_it != arg_tokens.end()) {
            if (token_it->type == detail::argument_token::t_flag) {
                auto opt_arg_it = std::ranges::find_if(
                    this->_optional_args, this->_name_match_predicate(token_it->value)
                );

                if (opt_arg_it == this->_optional_args.end())
                    throw error::argument_not_found(token_it->value);

                curr_opt_arg = std::ref(*opt_arg_it);
                curr_opt_arg->get()->mark_used();
            }
            else {
                if (not curr_opt_arg)
                    throw error::free_value(token_it->value);

                curr_opt_arg->get()->set_value(token_it->value);
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

    /// @brief Check if all required positional and optional arguments are used.
    void _check_required_args() const {
        for (const auto& arg : this->_positional_args)
            if (not arg->is_used())
                throw error::required_argument_not_parsed(arg->name());

        for (const auto& arg : this->_optional_args)
            if (arg->is_required() and not arg->has_value())
                throw error::required_argument_not_parsed(arg->name());
    }

    /// @brief Check if the number of argument values is within the specified range.
    void _check_nvalues_in_range() const {
        for (const auto& arg : this->_positional_args) {
            const auto nvalues_ordering = arg->nvalues_in_range();
            if (not std::is_eq(nvalues_ordering))
                throw error::invalid_nvalues(nvalues_ordering, arg->name());
        }

        for (const auto& arg : this->_optional_args) {
            const auto nvalues_ordering = arg->nvalues_in_range();
            if (not std::is_eq(nvalues_ordering))
                throw error::invalid_nvalues(nvalues_ordering, arg->name());
        }
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
