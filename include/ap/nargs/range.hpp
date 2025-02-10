// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

#pragma once

#include <iostream>
#include <optional>

namespace ap::nargs {

/// @brief Argument's number of values managing class.
class range {
public:
    using count_type = std::size_t;

    /// @brief Default constructor: creates range [1, 1].
    range() : _nlow(_ndefault), _nhigh(_ndefault) {}

    /**
     * @brief Exact count constructor: creates range [n, n].
     * @param n Expected value count.
     */
    explicit range(const count_type n) : _nlow(n), _nhigh(n) {}

    /**
     * @brief Concrete range constructor: creates range [nlow, nhigh].
     * @param nlow The lower bound.
     * @param nhigh The upper bound.
     */
    range(const count_type nlow, const count_type nhigh) : _nlow(nlow), _nhigh(nhigh) {}

    range(const range&) = default;
    range(range&&) = default;

    range& operator=(const range&) = default;
    range& operator=(range&&) = default;

    ~range() = default;

    /**
     * @brief Determines the ordering of the count against a range instance.
     *
     * For a `[low, high]` range and the count `n` the returned value is:
     * - `less` if `n < low`,
     * - `equivalent` if `n >= low` and `n <= high`,
     * - `greater` if `n > high`.
     * If either `low` or `high` limits are not set (std::nullopt),
     * then the corresponding conditions are dropped.
     *
     * @param n The value count to order.
     * @return Ordering relationship between the count and the range.
     */
    [[nodiscard]] std::weak_ordering ordering(const range::count_type n) const noexcept {
        if (not (this->_nlow.has_value() or this->_nhigh.has_value()))
            return std::weak_ordering::equivalent;

        if (this->_nlow.has_value() and this->_nhigh.has_value()) {
            if (n < this->_nlow.value())
                return std::weak_ordering::less;

            if (n > this->_nhigh.value())
                return std::weak_ordering::greater;

            return std::weak_ordering::equivalent;
        }

        if (this->_nlow.has_value())
            return (n < this->_nlow.value())
                     ? std::weak_ordering::less
                     : std::weak_ordering::equivalent;

        return (n > this->_nhigh.value())
                 ? std::weak_ordering::greater
                 : std::weak_ordering::equivalent;
    }

    friend range at_least(const count_type) noexcept;
    friend range more_than(const count_type) noexcept;
    friend range less_than(const count_type) noexcept;
    friend range up_to(const count_type) noexcept;
    friend range any() noexcept;

private:
    /**
     * @brief Private constructor: creates a possibly unbound range
     * @param nlow The optional lower bound of the range.
     * @param nhigh The optional upper bound of the range.
     */
    range(const std::optional<count_type> nlow, const std::optional<count_type> nhigh)
    : _nlow(nlow), _nhigh(nhigh) {}

    std::optional<count_type> _nlow;
    std::optional<count_type> _nhigh;

    static constexpr count_type _ndefault = 1ull;
};

/**
 * @brief `range` class builder function. Creates a range [n, inf].
 * @param n The lower bound.
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range at_least(const range::count_type n) noexcept {
    return range(n, std::nullopt);
}

/**
 * @brief `range` class builder function. Creates a range [n + 1, inf].
 * @param n The lower bound.
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range more_than(const range::count_type n) noexcept {
    return range(n + 1, std::nullopt);
}

/**
 * @brief `range` class builder function. Creates a range [0, n - 1].
 * @param n The upper bound
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range less_than(const range::count_type n) noexcept {
    return range(std::nullopt, n - 1);
}

/**
 * @brief `range` class builder function. Creates a range [0, n].
 * @param n The upper bound
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range up_to(const range::count_type n) noexcept {
    return range(std::nullopt, n);
}

/**
 * @brief `range` class builder function. Creates a range [0, inf].
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range any() noexcept {
    return range(std::nullopt, std::nullopt);
}

} // namespace ap::nargs
