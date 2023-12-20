#define AP_TESTING

#include "../doctest.h"

#include <ap/argument_parser.hpp>

#include <limits>

using namespace ap::nargs;


namespace {

constexpr range::count_type ndefault = 1;
constexpr range::count_type nlow = 3;
constexpr range::count_type nhigh = 9;
constexpr range::count_type nmid = (nlow + nhigh) / 2;

constexpr range::count_type nmin = std::numeric_limits<range::count_type>::min();
constexpr range::count_type nmax = std::numeric_limits<range::count_type>::max();

} // namespace


namespace ap_testing {

TEST_SUITE_BEGIN("test_nargs_range");

TEST_CASE("in_range should return true for default range only when n is 1") {
    const auto sut = range();

    REQUIRE(std::is_eq(sut.contains(ndefault)));

    REQUIRE(std::is_lt(sut.contains(ndefault - 1)));
    REQUIRE(std::is_gt(sut.contains(ndefault + 1)));
}

TEST_CASE("in_range should return true if n is in range") {
    SUBCASE("range is [n]") {
        const auto sut = range(nmid);

        REQUIRE(std::is_eq(sut.contains(nmid)));

        REQUIRE(std::is_lt(sut.contains(nmid - 1)));
        REQUIRE(std::is_gt(sut.contains(nmid + 1)));
    }

    SUBCASE("range is [low, high]") {
        const auto sut = range(nlow, nhigh);

        REQUIRE(std::is_eq(sut.contains(nlow)));
        REQUIRE(std::is_eq(sut.contains(nhigh)));
        REQUIRE(std::is_eq(sut.contains(nmid)));

        REQUIRE(std::is_lt(sut.contains(nlow - 1)));
        REQUIRE(std::is_gt(sut.contains(nhigh + 1)));
    }
}

TEST_CASE("range builders should return correct range objects") {
    SUBCASE("at_least") {
        const auto sut = at_least(nlow);

        REQUIRE(std::is_eq(sut.contains(nlow)));
        REQUIRE(std::is_eq(sut.contains(nhigh)));
        REQUIRE(std::is_eq(sut.contains(nmax)));

        REQUIRE(std::is_lt(sut.contains(nlow - 1)));
        REQUIRE(std::is_lt(sut.contains(nmin)));
    }

    SUBCASE("more_than") {
        const auto sut = more_than(nlow);

        REQUIRE(std::is_eq(sut.contains(nlow + 1)));
        REQUIRE(std::is_eq(sut.contains(nhigh)));
        REQUIRE(std::is_eq(sut.contains(nmax)));

        REQUIRE(std::is_lt(sut.contains(nlow)));
        REQUIRE(std::is_lt(sut.contains(nmin)));
    }

    SUBCASE("less_than") {
        const auto sut = less_than(nhigh);

        REQUIRE(std::is_eq(sut.contains(nhigh - 1)));
        REQUIRE(std::is_eq(sut.contains(nlow)));
        REQUIRE(std::is_eq(sut.contains(nmin)));

        REQUIRE(std::is_gt(sut.contains(nhigh)));
        REQUIRE(std::is_gt(sut.contains(nmax)));
    }

    SUBCASE("up_to") {
        const auto sut = up_to(nhigh);

        REQUIRE(std::is_eq(sut.contains(nhigh)));
        REQUIRE(std::is_eq(sut.contains(nlow)));
        REQUIRE(std::is_eq(sut.contains(nmin)));

        REQUIRE(std::is_gt(sut.contains(nhigh + 1)));
        REQUIRE(std::is_gt(sut.contains(nmax)));
    }
}

TEST_SUITE_END(); // test_nargs_range

} // namespace ap_testing
