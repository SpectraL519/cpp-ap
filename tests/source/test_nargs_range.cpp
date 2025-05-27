#define AP_TESTING

#include "doctest.h"

#include <ap/nargs/range.hpp>

#include <limits>

using namespace ap::nargs;

namespace {

constexpr range::count_type exact_bound = 1ull;
constexpr range::count_type lower_bound = 3ull;
constexpr range::count_type upper_bound = 9ull;
constexpr range::count_type mid = (lower_bound + upper_bound) / 2ull;

constexpr range::count_type min_bound = std::numeric_limits<range::count_type>::min();
constexpr range::count_type max_bound = std::numeric_limits<range::count_type>::max();

} // namespace

TEST_SUITE_BEGIN("test_nargs_range");

TEST_CASE("is_bound should return true only if at least one bound is set") {
    CHECK_FALSE(any().is_bound());

    CHECK(at_least(lower_bound).is_bound());
    CHECK(up_to(upper_bound).is_bound());
    CHECK(range(exact_bound).is_bound());
    CHECK(range(lower_bound, upper_bound).is_bound());
}

TEST_CASE("ordering should return true for default range only when n is 1") {
    const auto sut = range();

    REQUIRE(std::is_eq(sut.ordering(exact_bound)));

    REQUIRE(std::is_lt(sut.ordering(exact_bound - 1)));
    REQUIRE(std::is_gt(sut.ordering(exact_bound + 1)));
}

TEST_CASE("ordering should return true if n is in range") {
    SUBCASE("range is [n]") {
        const auto sut = range(mid);

        REQUIRE(std::is_eq(sut.ordering(mid)));

        REQUIRE(std::is_lt(sut.ordering(mid - 1)));
        REQUIRE(std::is_gt(sut.ordering(mid + 1)));
    }

    SUBCASE("range is [lower, upper]") {
        const auto sut = range(lower_bound, upper_bound);

        REQUIRE(std::is_eq(sut.ordering(lower_bound)));
        REQUIRE(std::is_eq(sut.ordering(upper_bound)));
        REQUIRE(std::is_eq(sut.ordering(mid)));

        REQUIRE(std::is_lt(sut.ordering(lower_bound - 1)));
        REQUIRE(std::is_gt(sut.ordering(upper_bound + 1)));
    }
}

TEST_CASE("range builders should return correct range objects") {
    SUBCASE("at_least") {
        const auto sut = at_least(lower_bound);

        REQUIRE(std::is_eq(sut.ordering(lower_bound)));
        REQUIRE(std::is_eq(sut.ordering(upper_bound)));
        REQUIRE(std::is_eq(sut.ordering(max_bound)));

        REQUIRE(std::is_lt(sut.ordering(lower_bound - 1)));
        REQUIRE(std::is_lt(sut.ordering(min_bound)));
    }

    SUBCASE("more_than") {
        const auto sut = more_than(lower_bound);

        REQUIRE(std::is_eq(sut.ordering(lower_bound + 1)));
        REQUIRE(std::is_eq(sut.ordering(upper_bound)));
        REQUIRE(std::is_eq(sut.ordering(max_bound)));

        REQUIRE(std::is_lt(sut.ordering(lower_bound)));
        REQUIRE(std::is_lt(sut.ordering(min_bound)));
    }

    SUBCASE("less_than") {
        const auto sut = less_than(upper_bound);

        REQUIRE(std::is_eq(sut.ordering(upper_bound - 1)));
        REQUIRE(std::is_eq(sut.ordering(lower_bound)));
        REQUIRE(std::is_eq(sut.ordering(min_bound)));

        REQUIRE(std::is_gt(sut.ordering(upper_bound)));
        REQUIRE(std::is_gt(sut.ordering(max_bound)));
    }

    SUBCASE("up_to") {
        const auto sut = up_to(upper_bound);

        REQUIRE(std::is_eq(sut.ordering(upper_bound)));
        REQUIRE(std::is_eq(sut.ordering(lower_bound)));
        REQUIRE(std::is_eq(sut.ordering(min_bound)));

        REQUIRE(std::is_gt(sut.ordering(upper_bound + 1)));
        REQUIRE(std::is_gt(sut.ordering(max_bound)));
    }

    SUBCASE("any") {
        const auto sut = any();

        REQUIRE(std::is_eq(sut.ordering(min_bound)));
        REQUIRE(std::is_eq(sut.ordering(exact_bound)));
        REQUIRE(std::is_eq(sut.ordering(lower_bound)));
        REQUIRE(std::is_eq(sut.ordering(mid)));
        REQUIRE(std::is_eq(sut.ordering(upper_bound)));
        REQUIRE(std::is_eq(sut.ordering(max_bound)));
    }
}

TEST_SUITE_END(); // test_nargs_range
