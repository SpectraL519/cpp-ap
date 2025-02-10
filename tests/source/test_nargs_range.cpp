#define AP_TESTING

#include "doctest.h"

#include <ap/nargs/range.hpp>

#include <limits>

using namespace ap::nargs;

namespace {

constexpr range::count_type ndefault = 1ull;
constexpr range::count_type nlow = 3ull;
constexpr range::count_type nhigh = 9ull;
constexpr range::count_type nmid = (nlow + nhigh) / 2ull;

constexpr range::count_type nmin = std::numeric_limits<range::count_type>::min();
constexpr range::count_type nmax = std::numeric_limits<range::count_type>::max();

} // namespace

TEST_SUITE_BEGIN("test_nargs_range");

TEST_CASE("ordering should return true for default range only when n is 1") {
    const auto sut = range();

    REQUIRE(std::is_eq(sut.ordering(ndefault)));

    REQUIRE(std::is_lt(sut.ordering(ndefault - 1)));
    REQUIRE(std::is_gt(sut.ordering(ndefault + 1)));
}

TEST_CASE("ordering should return true if n is in range") {
    SUBCASE("range is [n]") {
        const auto sut = range(nmid);

        REQUIRE(std::is_eq(sut.ordering(nmid)));

        REQUIRE(std::is_lt(sut.ordering(nmid - 1)));
        REQUIRE(std::is_gt(sut.ordering(nmid + 1)));
    }

    SUBCASE("range is [low, high]") {
        const auto sut = range(nlow, nhigh);

        REQUIRE(std::is_eq(sut.ordering(nlow)));
        REQUIRE(std::is_eq(sut.ordering(nhigh)));
        REQUIRE(std::is_eq(sut.ordering(nmid)));

        REQUIRE(std::is_lt(sut.ordering(nlow - 1)));
        REQUIRE(std::is_gt(sut.ordering(nhigh + 1)));
    }
}

TEST_CASE("range builders should return correct range objects") {
    SUBCASE("at_least") {
        const auto sut = at_least(nlow);

        REQUIRE(std::is_eq(sut.ordering(nlow)));
        REQUIRE(std::is_eq(sut.ordering(nhigh)));
        REQUIRE(std::is_eq(sut.ordering(nmax)));

        REQUIRE(std::is_lt(sut.ordering(nlow - 1)));
        REQUIRE(std::is_lt(sut.ordering(nmin)));
    }

    SUBCASE("more_than") {
        const auto sut = more_than(nlow);

        REQUIRE(std::is_eq(sut.ordering(nlow + 1)));
        REQUIRE(std::is_eq(sut.ordering(nhigh)));
        REQUIRE(std::is_eq(sut.ordering(nmax)));

        REQUIRE(std::is_lt(sut.ordering(nlow)));
        REQUIRE(std::is_lt(sut.ordering(nmin)));
    }

    SUBCASE("less_than") {
        const auto sut = less_than(nhigh);

        REQUIRE(std::is_eq(sut.ordering(nhigh - 1)));
        REQUIRE(std::is_eq(sut.ordering(nlow)));
        REQUIRE(std::is_eq(sut.ordering(nmin)));

        REQUIRE(std::is_gt(sut.ordering(nhigh)));
        REQUIRE(std::is_gt(sut.ordering(nmax)));
    }

    SUBCASE("up_to") {
        const auto sut = up_to(nhigh);

        REQUIRE(std::is_eq(sut.ordering(nhigh)));
        REQUIRE(std::is_eq(sut.ordering(nlow)));
        REQUIRE(std::is_eq(sut.ordering(nmin)));

        REQUIRE(std::is_gt(sut.ordering(nhigh + 1)));
        REQUIRE(std::is_gt(sut.ordering(nmax)));
    }

    SUBCASE("any") {
        const auto sut = any();

        REQUIRE(std::is_eq(sut.ordering(nmin)));
        REQUIRE(std::is_eq(sut.ordering(ndefault)));
        REQUIRE(std::is_eq(sut.ordering(nlow)));
        REQUIRE(std::is_eq(sut.ordering(nmid)));
        REQUIRE(std::is_eq(sut.ordering(nhigh)));
        REQUIRE(std::is_eq(sut.ordering(nmax)));
    }
}

TEST_SUITE_END(); // test_nargs_range
