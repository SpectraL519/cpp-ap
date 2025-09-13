// Copyright (c) 2023-2025 Jakub Musiał
// This file is part of the CPP-AP project (https://github.com/SpectraL519/cpp-ap).
// Licensed under the MIT License. See the LICENSE file in the project root for full license information.

/**
 * @file range.hpp
 * @brief Defines the `nargs::range` class and it's builder functions.
 */

#pragma once

#include <format>
#include <limits>
#include <ostream>

namespace ap::nargs {

using count_type = std::size_t;

constexpr count_type min_bound = std::numeric_limits<count_type>::min();
constexpr count_type max_bound = std::numeric_limits<count_type>::max();

/// @brief Argument's number of values managing class.
/// TODO: use concrete bounds + numeric limits instead of optional
class range {
public:
    /// @brief Default constructor: creates an unbound range.
    range() = default;

    /**
     * @brief Exact count constructor: creates range [n, n].
     * @param n Expected value count.
     */
    explicit range(const count_type n) : _lower_bound(n), _upper_bound(n) {}

    /**
     * @brief Concrete range constructor: creates range [lower, upper].
     * @param lower The lower bound.
     * @param upper The upper bound.
     */
    range(const count_type lower, const count_type upper)
    : _lower_bound(lower), _upper_bound(upper) {
        if (upper < lower)
            throw std::logic_error(
                std::format("Invalid range bounds: lower = {}, upper = {}", lower, upper)
            );
    }

    [[nodiscard]] bool has_explicit_lower_bound() const noexcept {
        return this->_lower_bound > min_bound;
    }

    [[nodiscard]] bool has_explicit_upper_bound() const noexcept {
        return this->_upper_bound < max_bound;
    }

    [[nodiscard]] bool is_explicitly_bound() const noexcept {
        return this->has_explicit_lower_bound() or this->has_explicit_upper_bound();
    }

    [[nodiscard]] bool is_exactly_bound() const noexcept {
        return this->_lower_bound == this->_upper_bound;
    }

    /**
     * @brief Determines the ordering of the count against a range instance.
     *
     * For a `[lower, upper]` range and the count `n` the returned value is:
     * - `less` if `n < lower`,
     * - `equivalent` if `n >= lower` and `n <= upper`,
     * - `greater` if `n > upper`.
     *
     * @param n The value count to order.
     * @return Ordering relationship between the count and the range.
     */
    [[nodiscard]] friend std::weak_ordering operator<=>(
        const count_type n, const range& r
    ) noexcept {
        if (n < r._lower_bound)
            return std::weak_ordering::less;

        if (n > r._upper_bound)
            return std::weak_ordering::greater;

        return std::weak_ordering::equivalent;
    }

    friend std::ostream& operator<<(std::ostream& os, const range& r) noexcept {
        if (not r.is_explicitly_bound()) {
            os << "unbound";
            return os;
        }

        if (r.is_exactly_bound()) {
            os << r._upper_bound;
            return os;
        }

        os << "[" << r._lower_bound << ", ";
        if (r.has_explicit_upper_bound())
            os << r._upper_bound << "]";
        else
            os << "inf)";

        return os;
    }

    friend range at_least(const count_type) noexcept;
    friend range more_than(const count_type) noexcept;
    friend range less_than(const count_type) noexcept;
    friend range up_to(const count_type) noexcept;
    friend range any() noexcept;

private:
    count_type _lower_bound = min_bound;
    count_type _upper_bound = max_bound;
};

/**
 * @brief `range` class builder function. Creates a range [n, inf).
 * @param n The lower bound.
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range at_least(const count_type n) noexcept {
    return range(n, max_bound);
}

/**
 * @brief `range` class builder function. Creates a range [n + 1, inf).
 * @param n The lower bound.
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range more_than(const count_type n) noexcept {
    return range(n + 1ull, max_bound);
}

/**
 * @brief `range` class builder function. Creates a range [0, n - 1].
 * @param n The upper bound
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range less_than(const count_type n) noexcept {
    return range(min_bound, n - 1ull);
}

/**
 * @brief `range` class builder function. Creates a range [0, n].
 * @param n The upper bound
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range up_to(const count_type n) noexcept {
    return range(min_bound, n);
}

/**
 * @brief `range` class builder function. Creates a range [0, inf].
 * @return Built `range` class instance.
 */
[[nodiscard]] inline range any() noexcept {
    return range(min_bound, max_bound);
}

} // namespace ap::nargs
