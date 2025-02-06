// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#include "ap/argument_parser.hpp"

namespace ap {

argument_parser& argument_parser::program_name(std::string_view name) noexcept {
    this->_program_name = name;
    return *this;
}

argument_parser& argument_parser::program_description(std::string_view description) noexcept {
    this->_program_description = description;
    return *this;
}

argument_parser& argument_parser::default_positional_arguments(
    const std::vector<argument::default_positional>& arg_discriminator_list
) noexcept {
    for (const auto arg_discriminator : arg_discriminator_list)
        this->_add_default_positional_argument(arg_discriminator);
    return *this;
}

argument_parser& argument_parser::default_optional_arguments(
    const std::vector<argument::default_optional>& arg_discriminator_list
) noexcept {
    for (const auto arg_discriminator : arg_discriminator_list)
        this->_add_default_optional_argument(arg_discriminator);
    return *this;
}

void argument_parser::parse_args(int argc, char* argv[]) {
    this->_parse_args_impl(this->_tokenize(argc, argv));

    if (this->_are_required_args_bypassed())
        return;

    this->_check_required_args();
    this->_check_nvalues_in_range();
}

bool argument_parser::has_value(std::string_view arg_name) const noexcept {
    const auto arg_opt = this->_get_argument(arg_name);
    return arg_opt ? arg_opt->get().has_value() : false;
}

std::size_t argument_parser::count(std::string_view arg_name) const noexcept {
    const auto arg_opt = this->_get_argument(arg_name);
    return arg_opt ? arg_opt->get().nused() : 0ull;
}

std::ostream& operator<<(std::ostream& os, const argument_parser& parser) noexcept {
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

void argument_parser::_add_default_positional_argument(
    const argument::default_positional arg_discriminator
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

void argument_parser::_add_default_optional_argument(
    const argument::default_optional arg_discriminator
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

bool argument_parser::_is_arg_name_used(const detail::argument_name& arg_name) const noexcept {
    const auto predicate = this->_name_match_predicate(arg_name);

    if (std::ranges::find_if(this->_positional_args, predicate) != this->_positional_args.end())
        return true;

    if (std::ranges::find_if(this->_optional_args, predicate) != this->_optional_args.end())
        return true;

    return false;
}

argument_parser::arg_token_list_t argument_parser::_tokenize(int argc, char* argv[]) const noexcept {
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

bool argument_parser::_is_flag(const std::string& arg) const noexcept {
    if (arg.starts_with(this->_flag_prefix))
        return this->_is_arg_name_used({arg.substr(this->_flag_prefix_length)});

    if (arg.starts_with(this->_flag_prefix_char))
        return this->_is_arg_name_used({arg.substr(this->_flag_prefix_char_length)});

    return false;
}

void argument_parser::_strip_flag_prefix(std::string& arg_flag) const noexcept {
    if (arg_flag.starts_with(this->_flag_prefix))
        arg_flag.erase(0, this->_flag_prefix_length);
    else
        arg_flag.erase(0, this->_flag_prefix_char_length);
}

void argument_parser::_parse_args_impl(const arg_token_list_t& arg_tokens) {
    arg_token_list_iterator_t token_it = arg_tokens.begin();
    this->_parse_positional_args(arg_tokens, token_it);
    this->_parse_optional_args(arg_tokens, token_it);
}

void argument_parser::_parse_positional_args(
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

void argument_parser::_parse_optional_args(
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

bool argument_parser::_are_required_args_bypassed() const noexcept {
    return std::ranges::any_of(this->_optional_args, [](const arg_ptr_t& arg) {
        return arg->is_used() and arg->bypass_required_enabled();
    });
}

void argument_parser::_check_required_args() const {
    for (const auto& arg : this->_positional_args)
        if (not arg->is_used())
            throw error::required_argument_not_parsed(arg->name());

    for (const auto& arg : this->_optional_args)
        if (arg->is_required() and not arg->has_value())
            throw error::required_argument_not_parsed(arg->name());
}

void argument_parser::_check_nvalues_in_range() const {
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

argument_parser::arg_opt_t argument_parser::_get_argument(std::string_view arg_name
) const noexcept {
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

} // namespace ap
