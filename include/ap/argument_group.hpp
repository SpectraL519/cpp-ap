// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include "detail/argument_base.hpp"

#include <memory>

namespace ap {

class argument_group {
public:
    argument_group() = delete;

    // TODO: description

    argument_group& required(const bool r = true) noexcept {
        this->_required = r;
        return *this;
    }

    argument_group& mutually_exclusive(const bool me = true) noexcept {
        this->_mutually_exclusive = me;
        return *this;
    }

    friend class argument_parser;

private:
    using arg_ptr_t = std::shared_ptr<detail::argument_base>;
    using arg_ptr_vec_t = std::vector<arg_ptr_t>;

    static std::unique_ptr<argument_group> create(argument_parser& parser, std::string_view name) {
        return std::unique_ptr<argument_group>(new argument_group(parser, name));
    }

    argument_group(argument_parser& parser, const std::string_view name)
    : _parser(&parser), _name(name) {}

    void _add_argument(arg_ptr_t arg) noexcept {
        this->_arguments.emplace_back(std::move(arg));
    }

    argument_parser* _parser;
    std::string _name;
    arg_ptr_vec_t _arguments;

    bool _required : 1 = false;
    bool _mutually_exclusive : 1 = false;
};

} // namespace ap
