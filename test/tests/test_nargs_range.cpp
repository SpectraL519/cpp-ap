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

    REQUIRE(in_range(sut, ndefault));

    REQUIRE_FALSE(in_range(sut, ndefault - 1));
    REQUIRE_FALSE(in_range(sut, ndefault + 1));
}

TEST_CASE("in_range should return true if n is in range") {
    SUBCASE("range is [n]") {
        const auto sut = range(nmid);

        REQUIRE(in_range(sut, nmid));

        REQUIRE_FALSE(in_range(sut, nmid - 1));
        REQUIRE_FALSE(in_range(sut, nmid + 1));
    }

    SUBCASE("range is [low, high]") {
        const auto sut = range(nlow, nhigh);

        REQUIRE(in_range(sut, nlow));
        REQUIRE(in_range(sut, nhigh));
        REQUIRE(in_range(sut, nmid));

        REQUIRE_FALSE(in_range(sut, nlow - 1));
        REQUIRE_FALSE(in_range(sut, nhigh + 1));
    }
}

TEST_CASE("range builders should return correct range objects") {
    SUBCASE("at_least") {
        const auto sut = at_least(nlow);

        REQUIRE(in_range(sut, nlow));
        REQUIRE(in_range(sut, nhigh));
        REQUIRE(in_range(sut, nmax));

        REQUIRE_FALSE(in_range(sut, nlow - 1));
        REQUIRE_FALSE(in_range(sut, nmin));
    }

    SUBCASE("more_than") {
        const auto sut = more_than(nlow);

        REQUIRE(in_range(sut, nlow + 1));
        REQUIRE(in_range(sut, nhigh));
        REQUIRE(in_range(sut, nmax));

        REQUIRE_FALSE(in_range(sut, nlow));
        REQUIRE_FALSE(in_range(sut, nmin));
    }

    SUBCASE("less_than") {
        const auto sut = less_than(nhigh);

        REQUIRE(in_range(sut, nhigh - 1));
        REQUIRE(in_range(sut, nlow));
        REQUIRE(in_range(sut, nmin));

        REQUIRE_FALSE(in_range(sut, nhigh));
        REQUIRE_FALSE(in_range(sut, nmax));
    }

    SUBCASE("up_to") {
        const auto sut = up_to(nhigh);

        REQUIRE(in_range(sut, nhigh));
        REQUIRE(in_range(sut, nlow));
        REQUIRE(in_range(sut, nmin));

        REQUIRE_FALSE(in_range(sut, nhigh + 1));
        REQUIRE_FALSE(in_range(sut, nmax));
    }
}

TEST_SUITE_END(); // test_nargs_range

} // namespace ap_testing
