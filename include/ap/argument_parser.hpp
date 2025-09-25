// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file ap/argument_parser.hpp
 * @brief Main library header file. Defines the `argument_parser` class.
 */

#pragma once

#include "argument.hpp"
#include "argument_group.hpp"
#include "detail/argument_token.hpp"
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

/// @brief The enumeration of default arguments provided by the library.
enum class default_argument : std::uint8_t {
    /**
     * @brief A positional argument representing a single input file path.
     * Equivalent to:
     * @code{.cpp}
     * parser.add_positional_argument("input")
     *       .action<ap::action_type::observe>(ap::action::check_file_exists())
     *       .help("Input file path");
     * @endcode
     */
    p_input,

    /**
     * @brief A positional argument representing a single output file path.
     * Equivalent to:
     * @code{.cpp}
     * parser.add_positional_argument("output")
     *       .help("Output file path");
     * @endcode
     */
    p_output,

    /**
     * @brief An optional argument representing the program's help flag.
     * Equivalent to:
     * @code{.cpp}
     * parser.add_optional_argument<ap::none_type>("help")
     *       .action<ap::action_type::on_flag>(ap::action::print_help(parser, EXIT_SUCCESS))
     *       .help("Display the help message");
     * @endcode
     */
    o_help,

    /**
     * @brief An optional argument representing the program's version flag.
     * Equivalent to:
     * @code{.cpp}
     * arg_parser.add_optional_argument<none_type>("version", "v")
     *           .action<action_type::on_flag>([&arg_parser]() {
     *               arg_parser.print_version();
     *               std::exit(EXIT_SUCCESS);
     *           })
     *           .help("Dsiplay program version info");
     * @endcode
     */
    o_version,

    /**
     * @brief A positional argument representing multiple input file paths.
     * Equivalent to:
     * @code{.cpp}
     * parser.add_positional_argument("input", "i")
     *       .nargs(1ull)
     *       .action<ap::action_type::observe>(ap::action::check_file_exists())
     *       .help("Input file path");
     * @endcode
     */
    o_input,

    /**
     * @brief A positional argument representing multiple output file paths.
     * Equivalent to:
     * @code{.cpp}
     * parser.add_positional_argument("output", "o")
     *       .nargs(1ull)
     *       .help("Output file path");
     * @endcode
     */
    o_output,

    /**
     * @brief A positional argument representing multiple input file paths.
     * Equivalent to:
     * @code{.cpp}
     * parser.add_positional_argument("input", "i")
     *       .nargs(ap::nargs::at_least(1ull))
     *       .action<ap::action_type::observe>(ap::action::check_file_exists())
     *       .help("Input file path");
     * @endcode
     */
    o_multi_input,

    /**
     * @brief A positional argument representing multiple output file paths.
     * Equivalent to:
     * @code{.cpp}
     * parser.add_positional_argument("output", "o")
     *       .nargs(ap::nargs::at_least(1ull))
     *       .help("Output file path");
     * @endcode
     */
    o_multi_output
};

/// @brief The enumeration of policies for handling unknown arguments.
enum class unknown_policy : std::uint8_t {
    fail, ///< Throw an exception when an unknown argument is encountered.
    warn, ///< Issue a warning when an unknown argument is encountered.
    ignore, ///< Ignore unknown arguments.
    as_values ///< Treat unknown arguments as positional values.
};

namespace detail {

void add_default_argument(const default_argument, argument_parser&) noexcept;

} // namespace detail

/**
 * @brief The main argument parser class.
 *
 * This class provides methods to define positional and optional arguments, set parser options,
 * and parse the command-line input.
 *
 * Example usage:
 * @code{.cpp}
 * #include <ap/argument_parser.hpp>
 *
 * int main(int argc, char* argv[]) {
 *     // Create the argument parser instance
 *     ap::argument_parser parser("fcopy");
 *     parser.program_version({ .major = 1, .minor = 0, .patch = 0 })
 *           .program_description("A simple file copy utility.")
 *           .default_arguments(
 *               ap::default_argument::o_help,
 *               ap::default_argument::o_input,
 *               ap::default_argument::o_output
 *           )
 *           .verbose()
 *           .unknown_arguments_policy(ap::unknown_policy::ignore)
 *           .try_parse_args(argc, argv);
 *
 *     // Access parsed argument values
 *     const std::string input_file = parser.value("input");
 *     const std::string output_file = parser.value("output");
 *
 *     // Application logic here
 *     std::cout << "Copying from " << input_file << " to " << output_file << std::endl;
 *
 *     return 0;
 * }
 * @endcode
 */
class argument_parser {
public:
    argument_parser(const argument_parser&) = delete;
    argument_parser& operator=(const argument_parser&) = delete;

    argument_parser(argument_parser&&) = delete;
    argument_parser& operator=(argument_parser&&) = delete;

    argument_parser(const std::string_view name) : argument_parser(name, "") {}

    ~argument_parser() = default;

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
        if (util::contains_whitespaces(version))
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
     * @brief Set the unknown argument flags handling policy.
     * @param policy The unknown arguments policy value.
     * @return Reference to the argument parser.
     * @note The default unknown arguments policy value is `ap::unknown_policy::fail`.
     */
    argument_parser& unknown_arguments_policy(const unknown_policy policy) noexcept {
        this->_unknown_policy = policy;
        return *this;
    }

    /**
     * @brief Add default arguments to the argument parser.
     * @tparam AR Type of the positional argument discriminator range.
     * @param arg_discriminators A range of default positional argument discriminators.
     * @note `arg_discriminators` must be a `std::ranges::range` with the `ap::default_argument` value type.
     * @return Reference to the argument parser.
     */
    template <util::c_range_of<default_argument> AR>
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
     * @param name The name of the argument.
     * @return Reference to the added positional argument.
     * @throws ap::invalid_configuration
     */
    template <util::c_argument_value_type T = std::string>
    positional_argument<T>& add_positional_argument(const std::string_view name) {
        return this->add_positional_argument<T>(this->_gr_positional_args, name);
    }

    /**
     * @brief Adds a positional argument to the parser's configuration and binds it to the given group.
     * @tparam T Type of the argument value.
     * @param primary_name The name of the argument.
     * @return Reference to the added positional argument.
     * @throws ap::invalid_configuration
     */
    template <util::c_argument_value_type T = std::string>
    positional_argument<T>& add_positional_argument(
        argument_group& group, const std::string_view name
    ) {
        this->_validate_group(group);
        this->_verify_arg_name_pattern(name);

        const detail::argument_name arg_name(std::make_optional<std::string>(name));
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        auto& new_arg_ptr =
            this->_positional_args.emplace_back(std::make_shared<positional_argument<T>>(arg_name));
        group._add_argument(new_arg_ptr);
        return static_cast<positional_argument<T>&>(*new_arg_ptr);
    }

    /**
     * @brief Adds an optional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param name The name of the argument.
     * @param name_discr The discriminator value specifying whether the given name should be treated as primary or secondary.
     * @return Reference to the added optional argument.
     * @throws ap::invalid_configuration
     */
    template <util::c_argument_value_type T = std::string>
    optional_argument<T>& add_optional_argument(
        const std::string_view name,
        const detail::argument_name_discriminator name_discr = n_primary
    ) {
        return this->add_optional_argument<T>(this->_gr_optional_args, name, name_discr);
    }

    /**
     * @brief Adds an optional argument to the parser's configuration.
     * @tparam T Type of the argument value.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added optional argument.
     * @throws ap::invalid_configuration
     */
    template <util::c_argument_value_type T = std::string>
    optional_argument<T>& add_optional_argument(
        const std::string_view primary_name, const std::string_view secondary_name
    ) {
        return this->add_optional_argument<T>(
            this->_gr_optional_args, primary_name, secondary_name
        );
    }

    /**
     * @brief Adds an optional argument to the parser's configuration and binds it to the given group.
     * @tparam T Type of the argument value.
     * @param group The argument group to bind the new argument to.
     * @param name The name of the argument.
     * @param name_discr The discriminator value specifying whether the given name should be treated as primary or secondary.
     * @return Reference to the added optional argument.
     * @throws std::logic_error, ap::invalid_configuration
     */
    template <util::c_argument_value_type T = std::string>
    optional_argument<T>& add_optional_argument(
        argument_group& group,
        const std::string_view name,
        const detail::argument_name_discriminator name_discr = n_primary
    ) {
        this->_validate_group(group);
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

        auto& new_arg_ptr =
            this->_optional_args.emplace_back(std::make_shared<optional_argument<T>>(arg_name));
        group._add_argument(new_arg_ptr);
        return static_cast<optional_argument<T>&>(*new_arg_ptr);
    }

    /**
     * @brief Adds an optional argument to the parser's configuration and binds it to the given group.
     * @tparam T Type of the argument value.
     * @param group The argument group to bind the new argument to.
     * @param primary_name The primary name of the argument.
     * @param secondary_name The secondary name of the argument.
     * @return Reference to the added optional argument.
     * @throws ap::invalid_configuration
     */
    template <util::c_argument_value_type T = std::string>
    optional_argument<T>& add_optional_argument(
        argument_group& group,
        const std::string_view primary_name,
        const std::string_view secondary_name
    ) {
        this->_validate_group(group);
        this->_verify_arg_name_pattern(primary_name);
        this->_verify_arg_name_pattern(secondary_name);

        const detail::argument_name arg_name(
            std::make_optional<std::string>(primary_name),
            std::make_optional<std::string>(secondary_name),
            this->_flag_prefix_char
        );
        if (this->_is_arg_name_used(arg_name))
            throw invalid_configuration::argument_name_used(arg_name);

        auto& new_arg_ptr =
            this->_optional_args.emplace_back(std::make_shared<optional_argument<T>>(arg_name));
        group._add_argument(new_arg_ptr);
        return static_cast<optional_argument<T>&>(*new_arg_ptr);
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
     * @brief Adds a boolean flag argument (an optional argument with `value_type = bool`) to the parser's configuration and binds it to the given group.
     * @tparam StoreImplicitly A boolean value used as the `implicit_values` parameter of the argument.
     * @note The argument's `default_values` attribute will be set to `not StoreImplicitly`.
     * @param group The argument group to bind the new argument to.
     * @param name The primary name of the flag.
     * @param name_discr The discriminator value specifying whether the given name should be treated as primary or secondary.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    optional_argument<bool>& add_flag(
        argument_group& group,
        const std::string_view name,
        const detail::argument_name_discriminator name_discr = n_primary
    ) {
        return this->add_optional_argument<bool>(group, name, name_discr)
            .default_values(not StoreImplicitly)
            .implicit_values(StoreImplicitly)
            .nargs(0ull);
    }

    /**
     * @brief Adds a boolean flag argument (an optional argument with `value_type = bool`) to the parser's configuration and binds it to the given group.
     * @tparam StoreImplicitly A boolean value used as the `implicit_values` parameter of the argument.
     * @note The argument's `default_values` attribute will be set to `not StoreImplicitly`.
     * @param group The argument group to bind the new argument to.
     * @param primary_name The primary name of the flag.
     * @param secondary_name The secondary name of the flag.
     * @return Reference to the added boolean flag argument.
     */
    template <bool StoreImplicitly = true>
    optional_argument<bool>& add_flag(
        argument_group& group,
        const std::string_view primary_name,
        const std::string_view secondary_name
    ) {
        return this->add_optional_argument<bool>(group, primary_name, secondary_name)
            .default_values(not StoreImplicitly)
            .implicit_values(StoreImplicitly)
            .nargs(0ull);
    }

    /**
     * @brief Adds an argument group with the given name to the parser's configuration.
     * @param name Name of the group.
     * @return Reference to the added argument group.
     */
    argument_group& add_group(const std::string_view name) noexcept {
        return *this->_argument_groups.emplace_back(argument_group::create(*this, name));
    }

    /**
     * @brief Adds an subparser with the given name to the parser's configuration.
     * @param name Name of the subparser.
     * @return Reference to the added subparser.
     */
    argument_parser& add_subparser(const std::string_view name) {
        const auto subparser_it = std::ranges::find(
            this->_subparsers, name, [](const auto& subparser) { return subparser->_name; }
        );
        if (subparser_it != this->_subparsers.end())
            throw std::logic_error(std::format(
                "A subparser with the given name () already exists in parser '{}'",
                (*subparser_it)->_name,
                this->_program_name
            ));

        return *this->_subparsers.emplace_back(
            std::unique_ptr<argument_parser>(new argument_parser(name, this->_program_name))
        );
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
     * @brief Parses the command-line arguments.
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @note `argv_rng` must be a `std::ranges::forward_range` with a value type convertible to `std::string`.
     * @throws ap::invalid_configuration, ap::parsing_failure
     * @attention This overload of the `parse_args` function assumes that the program name argument has already been discarded.
     */
    template <util::c_forward_range_of<std::string, util::type_validator::convertible> AR>
    void parse_args(const AR& argv_rng) {
        parsing_state state(*this);
        this->_parse_args_impl(std::ranges::begin(argv_rng), std::ranges::end(argv_rng), state);

        if (not state.unknown_args.empty())
            throw parsing_failure(std::format(
                "Failed to deduce the argument for values [{}]", util::join(state.unknown_args)
            ));
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
     * @brief Parses the command-line arguments and exits on error.
     *
     * Calls `parse_args(argv_rng)` in a try-catch block. If an error is thrown, then its
     * message and the parser are printed to `std::cerr` and the function exists with
     * `EXIT_FAILURE` status.
     *
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @note `argv_rng` must be a `std::ranges::forward_range` with a value type convertible to `std::string`.
     * @attention This overload of the `try_parse_args` function assumes that the program name argument has already been discarded.
     */
    template <util::c_forward_range_of<std::string, util::type_validator::convertible> AR>
    void try_parse_args(const AR& argv_rng) {
        try {
            this->parse_args(argv_rng);
        }
        catch (const ap::argument_parser_exception& err) {
            std::cerr << "[ap::error] " << err.what() << std::endl
                      << this->resolved_parser() << std::endl;
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
     * @brief Parses the known command-line arguments.
     *
     * * An argument is considered "known" if it was defined using the parser's argument declaraion methods:
     * - `add_positional_argument`
     * - `add_optional_argument`
     * - `add_flag`
     *
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @note `argv_rng` must be a `std::ranges::forward_range` with a value type convertible to `std::string`.
     * @throws ap::invalid_configuration, ap::parsing_failure
     * @attention This overload of the `parse_known_args` function assumes that the program name argument already been discarded.
     */
    template <util::c_forward_range_of<std::string, util::type_validator::convertible> AR>
    std::vector<std::string> parse_known_args(const AR& argv_rng) {
        parsing_state state(*this, true);
        this->_parse_args_impl(std::ranges::begin(argv_rng), std::ranges::end(argv_rng), state);
        return std::move(state.unknown_args);
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
     * @brief Parses known the command-line arguments and exits on error.
     *
     * Calls `parse_known_args(argv_rng)` in a try-catch block. If an error is thrown, then its message
     * and the parser are printed to `std::cerr` and the function exists with `EXIT_FAILURE` status.
     * Otherwise the result of `parse_known_args(argv_rng)` is returned.
     *
     * @tparam AR The argument range type.
     * @param argv_rng A range of command-line argument values.
     * @note `argv_rng` must be a `std::ranges::forward_range` with a value type convertible to `std::string`.
     * @return A vector of unknown argument values.
     * @attention This overload of the `try_parse_known_args` function assumes that the program name argument has already been discarded.
     */
    template <util::c_forward_range_of<std::string, util::type_validator::convertible> AR>
    std::vector<std::string> try_parse_known_args(const AR& argv_rng) {
        try {
            return this->parse_known_args(argv_rng);
        }
        catch (const ap::argument_parser_exception& err) {
            std::cerr << "[ap::error] " << err.what() << std::endl
                      << this->resolved_parser() << std::endl;
            std::exit(EXIT_FAILURE);
        }
    }

    /// @brief Returns the parser's name.
    [[nodiscard]] std::string_view name() const noexcept {
        return this->_name;
    }

    /**
     * @brief Returns the parser's full program name.
     *
     * - For top-level parsers, this is the same as the parser's name.
     * - For subparsers, the name is prefixed with its parent parser names.
     *
     * @example
     * Top-level parser: `git`
     * Subparser: `git submodule`
     * Nested subparser : `git submodule init`
     */
    [[nodiscard]] std::string_view program_name() const noexcept {
        return this->_program_name;
    }

    /**
     * @brief Check whether this parser was invoked.
     * @return `true` if the parser was selected when parsing the command-line arguments, `false` otherwise.
     * @note A parser is *invoked* as soon as the parser is selected during parsing, even if parsing is later delegated to one of its subparsers.
     */
    [[nodiscard]] bool invoked() const noexcept {
        return this->_invoked;
    }

    /**
     * @brief Check whether the parser has finalized parsing its own arguments.
     * @return `true` if parsing was completed for the parser, `false` otherwise.
     */
    [[nodiscard]] bool finalized() const noexcept {
        return this->_finalized;
    }

    /**
     * @brief Returns the *deepest invoked parser*.
     * @return Reference to the finalized parser that ultimately processed the arguments.
     */
    [[nodiscard]] argument_parser& resolved_parser() noexcept {
        const auto used_subparser_it = std::ranges::find_if(
            this->_subparsers, [](const auto& subparser) { return subparser->_invoked; }
        );
        if (used_subparser_it == this->_subparsers.end())
            return *this;
        return (*used_subparser_it)->resolved_parser();
    }

    /**
     * @brief Check if a specific argument was used in the command-line.
     * @param arg_name The name of the argument.
     * @return `true` if the argument was used on the command line, `false` otherwise.
     */
    [[nodiscard]] bool is_used(std::string_view arg_name) const noexcept {
        const auto arg = this->_get_argument(arg_name);
        return arg ? arg->is_used() : false;
    }

    /**
     * @brief Check if the given argument has a value.
     * @param arg_name The name of the argument.
     * @return `true` if the argument has a value, `false` otherwise.
     */
    [[nodiscard]] bool has_value(std::string_view arg_name) const noexcept {
        const auto arg = this->_get_argument(arg_name);
        return arg ? arg->has_value() : false;
    }

    /**
     * @brief Get the given argument's usage count.
     * @param arg_name The name of the argument.
     * @return The number of times the argument has been used.
     */
    [[nodiscard]] std::size_t count(std::string_view arg_name) const noexcept {
        const auto arg = this->_get_argument(arg_name);
        return arg ? arg->count() : 0ull;
    }

    /**
     * @brief Get the value of the given argument.
     * @tparam T Type of the argument value.
     * @param arg_name The name of the argument.
     * @return The value of the argument.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <util::c_argument_value_type T = std::string>
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
     * @brief Get the value of the given argument, if it has any, or a fallback value, if not.
     * @tparam T Type of the argument value.
     * @tparam U The default value type.
     * @param arg_name The name of the argument.
     * @param fallback_value The fallback value.
     * @return The value of the argument.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <util::c_argument_value_type T = std::string, std::convertible_to<T> U>
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
     * @brief Get all values of the given argument.
     * @tparam T Type of the argument values.
     * @param arg_name The name of the argument.
     * @return The values of the argument as a vector.
     * @throws ap::lookup_failure, ap::type_error
     */
    template <util::c_argument_value_type T = std::string>
    [[nodiscard]] std::vector<T> values(std::string_view arg_name) const {
        const auto arg = this->_get_argument(arg_name);
        if (not arg)
            throw lookup_failure::argument_not_found(arg_name);

        try {
            std::vector<T> values;
            // TODO: use std::ranges::to after transition to C++23
            std::ranges::copy(
                util::any_range_cast_view<T>(arg->values()), std::back_inserter(values)
            );
            return values;
        }
        catch (const std::bad_any_cast&) {
            throw type_error::invalid_value_type<T>(arg->name());
        }
    }

    /**
     * @brief Prints the argument parser's help message to an output stream.
     * @param verbose The verbosity mode value.
     * @param os The output stream.
     */
    void print_help(const bool verbose, std::ostream& os = std::cout) const noexcept {
        os << "Program: " << this->_program_name;
        if (this->_program_version)
            os << " (" << this->_program_version.value() << ')';
        os << '\n';

        if (this->_program_description)
            os << '\n'
               << std::string(this->_indent_width, ' ') << this->_program_description.value()
               << '\n';

        this->_print_subparsers(os);
        for (const auto& group : this->_argument_groups)
            this->_print_group(os, *group, verbose);
    }

    /**
     * @brief Prints the argument parser's version info to an output stream.
     *
     * If no version was spcified for the parser, `unspecified` will be printed.
     *
     * @param os The output stream.
     */
    void print_version(std::ostream& os = std::cout) const noexcept {
        os << this->_program_name << " : version " << this->_program_version.value_or("unspecified")
           << std::endl;
    }

    /**
     * @brief Prints the argument parser's details to an output stream.
     *
     * An `os << parser` operation is equivalent to a `parser.print_help(_verbose, os)` call,
     * where `_verbose` is the inner verbosity mode, which can be set with the @ref verbose function.
     *
     * @param os The output stream.
     * @param parser The argument parser to print.
     * @return The modified output stream.
     */
    friend std::ostream& operator<<(std::ostream& os, const argument_parser& parser) noexcept {
        parser.print_help(parser._verbose, os);
        return os;
    }

#ifdef AP_TESTING
    /// @brief Friend struct for testing purposes.
    friend struct ::ap_testing::argument_parser_test_fixture;
#endif

private:
    using arg_ptr_t = std::shared_ptr<detail::argument_base>;
    using arg_ptr_vec_t = std::vector<arg_ptr_t>;
    using arg_ptr_vec_iter_t = typename arg_ptr_vec_t::iterator;

    using arg_group_ptr_t = std::unique_ptr<argument_group>;
    using arg_group_ptr_vec_t = std::vector<arg_group_ptr_t>;

    using arg_parser_ptr_t = std::unique_ptr<argument_parser>;
    using arg_parser_ptr_vec_t = std::vector<arg_parser_ptr_t>;

    using arg_token_vec_t = std::vector<detail::argument_token>;
    using arg_token_vec_iter_t = typename arg_token_vec_t::const_iterator;

    /// @brief A collection of values used during the parsing process.
    struct parsing_state {
        parsing_state(argument_parser& parser, const bool parse_known_only = false)
        : curr_arg(nullptr),
          curr_pos_arg_it(parser._positional_args.begin()),
          parse_known_only(parse_known_only) {}

        /// @brief Update the parser-specific parameters of the state object.
        /// @param parser The new parser.
        void set_parser(argument_parser& parser) {
            this->curr_arg = nullptr;
            this->curr_pos_arg_it = parser._positional_args.begin();
        }

        arg_ptr_t curr_arg; ///< The currently processed argument.
        arg_ptr_vec_iter_t
            curr_pos_arg_it; ///< An iterator pointing to the next positional argument to be processed.
        const bool
            parse_known_only; ///< A flag indicating whether only known arguments should be parsed.
        std::vector<std::string> unknown_args = {}; ///< A vector of unknown argument values.
    };

    argument_parser(const std::string_view name, const std::string_view parent_name)
    : _name(name),
      _program_name(
          std::format("{}{}{}", parent_name, std::string(not parent_name.empty(), ' '), name)
      ),
      _gr_positional_args(add_group("Positional Arguments")),
      _gr_optional_args(add_group("Optional Arguments")) {
        if (name.empty()) // TODO: add test case
            throw invalid_configuration("The program name cannot be empty!");

        if (util::contains_whitespaces(name))
            throw invalid_configuration("The program name cannot contain whitespace characters!");
    }

    /**
     * @brief Verifies the pattern of an argument name and if it's invalid, an error is thrown
     * @throws ap::invalid_configuration
     */
    void _verify_arg_name_pattern(const std::string_view arg_name) const {
        if (arg_name.empty())
            throw invalid_configuration::invalid_argument_name(
                arg_name, "An argument name cannot be empty."
            );

        if (util::contains_whitespaces(arg_name))
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
     * @brief Check if the given group belongs to the parser.
     * @param group The group to validate.
     * @throws std::logic_error if the group doesn't belong to the parser.
     */
    void _validate_group(const argument_group& group) {
        if (group._parser != this)
            throw std::logic_error(std::format(
                "An argument group '{}' does not belong to the given parser.", group._name
            ));
    }

    /**
     * @brief Implementation of parsing command-line arguments.
     * @tparam AIt The command-line argument value iterator type.
     * @note `AIt` must be a `std::forward_iterator` with a value type convertible to `std::string`.
     * @param args_begin The begin iterator for the command-line argument value range.
     * @param args_end The end iterator for the command-line argument value range.
     * @param state The current parsing state.
     * @throws ap::invalid_configuration, ap::parsing_failure
     */
    template <util::c_forward_iterator_of<std::string, util::type_validator::convertible> AIt>
    void _parse_args_impl(AIt args_begin, const AIt args_end, parsing_state& state) {
        this->_invoked = true;

        if (args_begin != args_end) {
            // try to match a subparser
            const auto subparser_it =
                std::ranges::find(this->_subparsers, *args_begin, [](const auto& subparser) {
                    return subparser->_name;
                });
            if (subparser_it != this->_subparsers.end()) {
                auto& subparser = **subparser_it;
                state.set_parser(subparser);
                subparser._parse_args_impl(++args_begin, args_end, state);
                return;
            }
        }

        // process command-line arguments within the current parser
        this->_validate_argument_configuration();
        std::ranges::for_each(
            this->_tokenize(args_begin, args_end, state),
            std::bind_front(&argument_parser::_parse_token, this, std::ref(state))
        );
        this->_verify_final_state();
        this->_finalized = true;
    }

    /**
     * @brief Validate whether the definition/configuration of the parser's arguments is correct.
     *
     * What is verified:
     * 1. No required positional argument can be added after a non-required positional argument.
     */
    void _validate_argument_configuration() const {
        // step 1
        arg_ptr_t non_required_arg = nullptr;
        for (const auto& arg : this->_positional_args) {
            if (not arg->is_required()) {
                non_required_arg = arg;
                continue;
            }

            if (non_required_arg and arg->is_required())
                throw invalid_configuration(std::format(
                    "Required positional argument [{}] cannot be defined after a non-required "
                    "positional argument [{}].",
                    arg->name().str(),
                    non_required_arg->name().str()
                ));
        }
    }

    /**
     * @brief Converts the command-line arguments into a list of tokens.
     * @tparam AIt The command-line argument value iterator type.
     * @note `AIt` must be a `std::forward_iterator` with a value type convertible to `std::string`.
     * @param args_begin The begin iterator for the command-line argument value range.
     * @param args_end The end iterator for the command-line argument value range.
     * @return A list of preprocessed command-line argument tokens.
     */
    template <util::c_forward_iterator_of<std::string, util::type_validator::convertible> AIt>
    [[nodiscard]] arg_token_vec_t _tokenize(
        AIt args_begin, const AIt args_end, const parsing_state& state
    ) {
        arg_token_vec_t toks;
        toks.reserve(static_cast<std::size_t>(std::ranges::distance(args_begin, args_end)));

        std::ranges::for_each(
            args_begin,
            args_end,
            std::bind_front(&argument_parser::_tokenize_arg, this, std::ref(state), std::ref(toks))
        );

        return toks;
    }

    /**
     * @brief Appends an argument token(s) created from `arg_value` to the `toks` vector.
     * @param toks The argument token list to which the processed token(s) will be appended.
     * @param arg_value The command-line argument's value to be processed.
     */
    void _tokenize_arg(
        const parsing_state& state, arg_token_vec_t& toks, const std::string_view arg_value
    ) {
        detail::argument_token tok{
            .type = this->_deduce_token_type(arg_value), .value = std::string(arg_value)
        };

        if (not tok.is_flag_token() or this->_validate_flag_token(tok)) {
            toks.emplace_back(std::move(tok));
            return;
        }

        // not a value token -> flag token
        // flag token could not be validated -> unknown flag
        if (state.parse_known_only) { // do nothing (will be handled during parsing)
            toks.emplace_back(std::move(tok));
            return;
        }

        switch (this->_unknown_policy) {
        case unknown_policy::fail:
            throw parsing_failure::unknown_argument(tok.value);
        case unknown_policy::warn:
            std::cerr << "[ap::warning] Unknown argument '" << tok.value << "' will be ignored."
                      << std::endl;
            [[fallthrough]];
        case unknown_policy::ignore:
            return;
        case unknown_policy::as_values:
            tok.type = detail::argument_token::t_value;
            toks.emplace_back(std::move(tok));
            break;
        }
    }

    /**
     * @brief Returns the most appropriate *initial* token type based on a command-line argument's value.
     *
     * The token's *initial* type is deduced using the following rules:
     * - `t_value`: an argument contains whitespace characters or cannot be a flag token
     * - `t_flag_primary`: an argument begins with a primary flag prefix (`--`)
     * - `t_flag_secondary`: an argument begins with a secondary flag prefix (`-`)
     * - `t_flag_compound`: INITIALLY a token can NEVER have a compound flag type (may only be set when a flag token is validated)
     */
    [[nodiscard]] detail::argument_token::token_type _deduce_token_type(
        const std::string_view arg_value
    ) const noexcept {
        if (util::contains_whitespaces(arg_value))
            return detail::argument_token::t_value;

        if (arg_value.starts_with(this->_flag_prefix))
            return detail::argument_token::t_flag_primary;

        if (arg_value.starts_with(this->_flag_prefix_char))
            return detail::argument_token::t_flag_secondary;

        return detail::argument_token::t_value;
    }

    /**
     * @brief Check if a flag token is valid based on its value.
     * @attention Sets the `args` member of the token if an argument with the given name (token's value) is present.
     * @param tok The argument token to validate.
     * @return `true` if the given token represents a valid argument flag.
     */
    [[nodiscard]] bool _validate_flag_token(detail::argument_token& tok) noexcept {
        const auto opt_arg_it = this->_find_opt_arg(tok);
        if (opt_arg_it == this->_optional_args.end())
            return this->_validate_compound_flag_token(tok);

        tok.args.emplace_back(*opt_arg_it);
        return true;
    }

    /**
     * @brief Check if a flag token is a valid compound argument flag based on its value.
     * @attention If the token indeed represents valid compound flag, the token's type is changed to `t_flag_compuund`
     * @attention and its `args` list is filled with all the arguments the token represents.
     * @param tok The argument token to validate.
     * @return `true` if the given token represents a valid compound argument flag.
     */
    bool _validate_compound_flag_token(detail::argument_token& tok) noexcept {
        if (tok.type != detail::argument_token::t_flag_secondary)
            return false;

        const auto actual_tok_value = this->_strip_flag_prefix(tok);
        tok.args.reserve(actual_tok_value.size());

        for (const char c : actual_tok_value) {
            const auto opt_arg_it = std::ranges::find_if(
                this->_optional_args,
                this->_name_match_predicate(
                    std::string_view(&c, 1ull), detail::argument_name::m_secondary
                )
            );

            if (opt_arg_it == this->_optional_args.end()) {
                tok.args.clear();
                return false;
            }

            tok.args.emplace_back(*opt_arg_it);
        }

        tok.type = detail::argument_token::t_flag_compound;
        return true;
    }

    /**
     * @brief Find an optional argument based on a flag token.
     * @param flag_tok An argument_token instance, the value of which will be used to find the argument.
     * @return An iterator to the argument's position.
     * @note If the `flag_tok.type` is not a valid flag token, then the end iterator will be returned.
     */
    [[nodiscard]] arg_ptr_vec_iter_t _find_opt_arg(const detail::argument_token& flag_tok
    ) noexcept {
        if (not flag_tok.is_flag_token())
            return this->_optional_args.end();

        const auto actual_tok_value = this->_strip_flag_prefix(flag_tok);
        const auto match_type =
            flag_tok.type == detail::argument_token::t_flag_primary
                ? detail::argument_name::m_primary
                : detail::argument_name::m_secondary;

        return std::ranges::find_if(
            this->_optional_args, this->_name_match_predicate(actual_tok_value, match_type)
        );
    }

    /**
     * @brief Removes the flag prefix from a flag token's value.
     * @param tok The argument token to be processed.
     * @return The token's value without the flag prefix.
     */
    [[nodiscard]] std::string_view _strip_flag_prefix(const detail::argument_token& tok
    ) const noexcept {
        switch (tok.type) {
        case detail::argument_token::t_flag_primary:
            return std::string_view(tok.value).substr(this->_primary_flag_prefix_length);
        case detail::argument_token::t_flag_secondary:
            return std::string_view(tok.value).substr(this->_secondary_flag_prefix_length);
        default:
            return tok.value;
        }
    }

    /**
     * @brief Parse a single command-line argument token.
     * @param state The current parsing state.
     * @param tok The token to be parsed.
     * @throws ap::parsing_failure
     */
    void _parse_token(parsing_state& state, const detail::argument_token& tok) {
        if (state.curr_arg and state.curr_arg->is_greedy()) {
            this->_set_argument_value(state, tok.value);
            return;
        }

        if (tok.is_flag_token())
            this->_parse_flag_token(state, tok);
        else
            this->_parse_value_token(state, tok);
    }

    /**
     * @brief Parse a single command-line argument *flag* token.
     * @param state The current parsing state.
     * @param tok The token to be parsed.
     * @throws ap::parsing_failure
     */
    void _parse_flag_token(parsing_state& state, const detail::argument_token& tok) {
        if (not tok.is_valid_flag_token()) {
            if (state.parse_known_only) {
                state.curr_arg.reset();
                state.unknown_args.emplace_back(tok.value);
                return;
            }
            else {
                // should never happen as unknown flags are filtered out during tokenization
                throw parsing_failure::unknown_argument(tok.value);
            }
        }

        for (const auto& arg : tok.args) {
            if (arg->mark_used())
                state.curr_arg = arg;
            else
                state.curr_arg.reset();
        }
    }

    /**
     * @brief Parse a single command-line argument *value* token.
     * @param state The current parsing state.
     * @param tok The token to be parsed.
     * @throws ap::parsing_failure
     */
    void _parse_value_token(parsing_state& state, const detail::argument_token& tok) {
        if (not state.curr_arg) {
            if (state.curr_pos_arg_it == this->_positional_args.end()) {
                state.unknown_args.emplace_back(tok.value);
                return;
            }

            state.curr_arg = *state.curr_pos_arg_it;
        }

        this->_set_argument_value(state, tok.value);
    }

    /**
     * @brief Set the value for the currently processed argument.
     * @attention This function assumes that the current argument is set (i.e. `state.curr_arg != nullptr`).
     * @param state The current parsing state.
     * @param value The value to be set for the current argument.
     */
    void _set_argument_value(parsing_state& state, const std::string_view value) noexcept {
        if (state.curr_arg->set_value(std::string(value)))
            return; // argument still accepts values

        // advance to the next positional argument if possible
        if (state.curr_arg->is_positional()
            and state.curr_pos_arg_it != this->_positional_args.end())
            ++state.curr_pos_arg_it;

        state.curr_arg.reset();
    }

    /**
     * @brief Verifies the correctness of the parsed command-line arguments.
     * @throws ap::parsing_failure if the state of the parsed arguments is invalid.
     */
    void _verify_final_state() const {
        const bool supress_arg_checks = this->_are_arg_checks_supressed();
        for (const auto& group : this->_argument_groups)
            this->_verify_group_requirements(*group, supress_arg_checks);
    }

    /**
     * @brief Check whether required argument bypassing is enabled
     * @return true if at least one argument with enabled required argument bypassing is used, false otherwise.
     */
    [[nodiscard]] bool _are_arg_checks_supressed() const noexcept {
        // TODO: use std::views::join after the transition to C++23
        return std::ranges::any_of(
                   this->_positional_args,
                   [](const arg_ptr_t& arg) {
                       return arg->is_used() and arg->supresses_arg_checks();
                   }
               )
            or std::ranges::any_of(this->_optional_args, [](const arg_ptr_t& arg) {
                   return arg->is_used() and arg->supresses_arg_checks();
               });
    }

    /**
     * @brief Verifies whether the requirements of the given argument group are satisfied.
     * @param group The argument group to verify.
     * @param supress_arg_checks A flag indicating whether argument checks are suppressed.
     * @throws ap::parsing_failure if the requirements are not satistied.
     */
    void _verify_group_requirements(const argument_group& group, const bool supress_arg_checks)
        const {
        if (group._arguments.empty())
            return;

        const auto n_used_args = static_cast<std::size_t>(
            std::ranges::count_if(group._arguments, [](const auto& arg) { return arg->is_used(); })
        );

        if (group._mutually_exclusive) {
            if (n_used_args > 1ull)
                throw parsing_failure(std::format(
                    "At most one argument from the mutually exclusive group '{}' can be used",
                    group._name
                ));

            const auto used_arg_it = std::ranges::find_if(group._arguments, [](const auto& arg) {
                return arg->is_used();
            });

            if (used_arg_it != group._arguments.end()) {
                // only the one used argument has to be validated
                this->_verify_argument_requirements(*used_arg_it, supress_arg_checks);
                return;
            }
        }

        if (group._required and n_used_args == 0ull)
            throw parsing_failure(std::format(
                "At least one argument from the required group '{}' must be used", group._name
            ));

        // all arguments in the group have to be validated
        for (const auto& arg : group._arguments)
            this->_verify_argument_requirements(arg, supress_arg_checks);
    }

    /**
     * @brief Verifies whether the requirements of the given argument are satisfied.
     * @param arg The argument to verify.
     * @param supress_arg_checks A flag indicating whether argument checks are suppressed.
     * @throws ap::parsing_failure if the requirements are not satistied.
     */
    void _verify_argument_requirements(const arg_ptr_t& arg, const bool supress_arg_checks) const {
        if (supress_arg_checks)
            return;

        if (arg->is_required() and not arg->has_value())
            throw parsing_failure(
                std::format("No values parsed for a required argument [{}]", arg->name().str())
            );
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

    void _print_subparsers(std::ostream& os) const noexcept {
        if (this->_subparsers.empty())
            return;

        os << "\nCommands:\n";

        std::vector<detail::help_builder> builders;
        builders.reserve(this->_subparsers.size());

        for (const auto& subparser : this->_subparsers)
            builders.emplace_back(subparser->_name, subparser->_program_description);

        std::size_t max_subparser_name_length = 0ull;
        for (const auto& bld : builders)
            max_subparser_name_length = std::max(max_subparser_name_length, bld.name.length());

        for (const auto& bld : builders)
            os << '\n' << bld.get_basic(this->_indent_width, max_subparser_name_length);

        os << '\n';
    }

    /**
     * @brief Print the given argument list to an output stream.
     * @param os The output stream to print to.
     * @param group The argument group to print.
     * @param verbose A verbosity mode indicator flag.
     * @attention If a group has no visible arguments, nothing will be printed.
     */
    void _print_group(std::ostream& os, const argument_group& group, const bool verbose)
        const noexcept {
        auto visible_args = std::views::filter(group._arguments, [](const auto& arg) {
            return not arg->is_hidden();
        });

        if (std::ranges::empty(visible_args))
            return;

        os << '\n' << group._name << ":";

        std::vector<std::string> group_attrs;
        if (group._required)
            group_attrs.emplace_back("required");
        if (group._mutually_exclusive)
            group_attrs.emplace_back("mutually exclusive");
        if (not group_attrs.empty())
            os << " (" << util::join(group_attrs) << ')';

        os << '\n';

        if (verbose) {
            for (const auto& arg : visible_args)
                os << '\n' << arg->help_builder(verbose).get(this->_indent_width) << '\n';
        }
        else {
            std::vector<detail::help_builder> builders;
            builders.reserve(group._arguments.size());

            for (const auto& arg : visible_args)
                builders.emplace_back(arg->help_builder(verbose));

            std::size_t max_arg_name_length = 0ull;
            for (const auto& bld : builders)
                max_arg_name_length = std::max(max_arg_name_length, bld.name.length());

            for (const auto& bld : builders)
                os << '\n' << bld.get_basic(this->_indent_width, max_arg_name_length);

            os << '\n';
        }
    }

    std::string _name; ///< The name of the parser.
    std::string
        _program_name; ///< The name of the program in the format "<parent-parser-names>... <program-name>".
    std::optional<std::string> _program_version; ///< The version of the program.
    std::optional<std::string> _program_description; ///< The description of the program.
    bool _verbose = false; ///< Verbosity flag.
    unknown_policy _unknown_policy = unknown_policy::fail; ///< Policy for unknown arguments.

    arg_ptr_vec_t _positional_args; ///< The list of positional arguments.
    arg_ptr_vec_t _optional_args; ///< The list of optional arguments.
    arg_group_ptr_vec_t _argument_groups; ///< The list of argument groups.
    argument_group& _gr_positional_args; ///< The positional argument group.
    argument_group& _gr_optional_args; ///< The optional argument group.
    arg_parser_ptr_vec_t _subparsers; ///< The list of subparsers.

    bool _invoked =
        false; ///< A flag indicating whether the parser has been invoked to parse arguments.
    bool _finalized = false; ///< A flag indicating whether the parsing process has been finalized.

    static constexpr std::uint8_t _primary_flag_prefix_length = 2u;
    static constexpr std::uint8_t _secondary_flag_prefix_length = 1u;
    static constexpr char _flag_prefix_char = '-';
    static constexpr std::string_view _flag_prefix = "--";
    static constexpr std::uint8_t _indent_width = 2;
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
        arg_parser.add_optional_argument<none_type>("help", "h")
            .action<action_type::on_flag>(action::print_help(arg_parser, EXIT_SUCCESS))
            .help("Display the help message");
        break;

    case default_argument::o_version:
        arg_parser.add_optional_argument<none_type>("version", "v")
            .action<action_type::on_flag>([&arg_parser]() {
                arg_parser.print_version();
                std::exit(EXIT_SUCCESS);
            })
            .help("Dsiplay program version info");
        break;

    case default_argument::o_input:
        arg_parser.add_optional_argument("input", "i")
            .nargs(1ull)
            .action<action_type::observe>(action::check_file_exists())
            .help("Input file path");
        break;

    case default_argument::o_output:
        arg_parser.add_optional_argument("output", "o").nargs(1ull).help("Output file path");
        break;

    case default_argument::o_multi_input:
        arg_parser.add_optional_argument("input", "i")
            .nargs(ap::nargs::at_least(1ull))
            .action<action_type::observe>(action::check_file_exists())
            .help("Input files paths");
        break;

    case default_argument::o_multi_output:
        arg_parser.add_optional_argument("output", "o")
            .nargs(ap::nargs::at_least(1ull))
            .help("Output files paths");
        break;
    }
}

} // namespace detail

} // namespace ap
