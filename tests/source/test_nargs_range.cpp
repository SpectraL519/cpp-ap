#include "doctest.h"

#include <ap/nargs/range.hpp>

#include <limits>

using namespace ap::nargs;

TEST_SUITE_BEGIN("test_nargs_range");

namespace {

constexpr count_type exact_bound = 1ull;
constexpr count_type lower_bound = 3ull;
constexpr count_type upper_bound = 9ull;
constexpr count_type mid = (lower_bound + upper_bound) / 2ull;

} // namespace

TEST_CASE("has_explicit_lower_bound should return true only if the lower bound is explicitly set") {
    CHECK_FALSE(any().is_explicitly_bound());
    CHECK_FALSE(up_to(upper_bound).has_explicit_lower_bound());

    CHECK(at_least(lower_bound).has_explicit_lower_bound());
    CHECK(range(exact_bound).has_explicit_lower_bound());
    CHECK(range(lower_bound, upper_bound).has_explicit_lower_bound());
}

TEST_CASE("has_explicit_upper_bound should return true only if the upper bound is explicitly set") {
    CHECK_FALSE(any().has_explicit_upper_bound());
    CHECK_FALSE(at_least(lower_bound).has_explicit_upper_bound());

    CHECK(up_to(upper_bound).has_explicit_upper_bound());
    CHECK(range(exact_bound).has_explicit_upper_bound());
    CHECK(range(lower_bound, upper_bound).has_explicit_upper_bound());
}

TEST_CASE("is_explicitly_bound should return true only if at least one bound is explicitly set") {
    CHECK_FALSE(any().is_explicitly_bound());

    CHECK(at_least(lower_bound).is_explicitly_bound());
    CHECK(up_to(upper_bound).is_explicitly_bound());
    CHECK(range(exact_bound).is_explicitly_bound());
    CHECK(range(lower_bound, upper_bound).is_explicitly_bound());
}

TEST_CASE("range instances should be considered equal only if they have the same bounds") {
    CHECK_EQ(any(), any());
    CHECK_EQ(at_least(lower_bound), at_least(lower_bound));
    CHECK_EQ(more_than(lower_bound), more_than(lower_bound));
    CHECK_EQ(less_than(upper_bound), less_than(upper_bound));
    CHECK_EQ(up_to(upper_bound), up_to(upper_bound));
    CHECK_EQ(range(lower_bound, upper_bound), range(lower_bound, upper_bound));
    CHECK_EQ(range(exact_bound), range(exact_bound));

    CHECK_NE(at_least(lower_bound), any());
    CHECK_NE(more_than(lower_bound), any());
    CHECK_NE(less_than(lower_bound), any());
    CHECK_NE(up_to(lower_bound), any());
    CHECK_NE(range(lower_bound, upper_bound), any());
    CHECK_NE(range(lower_bound, upper_bound), range(lower_bound, upper_bound + 1ull));
    CHECK_NE(range(lower_bound, upper_bound), range(lower_bound - 1ull, upper_bound));
    CHECK_NE(range(exact_bound), any());
    CHECK_NE(range(exact_bound), range(lower_bound, upper_bound));
}

TEST_CASE("operator<=> should return eq for default range only when n is 1") {
    const auto sut = range(exact_bound);

    CHECK(std::is_eq(exact_bound <=> sut));

    CHECK(std::is_lt(exact_bound - 1 <=> sut));
    CHECK(std::is_gt(exact_bound + 1 <=> sut));
}

TEST_CASE("operator<=> should return eq if n is in range") {
    SUBCASE("range is [n]") {
        const auto sut = range(mid);

        CHECK(std::is_eq(mid <=> sut));

        CHECK(std::is_lt(mid - 1 <=> sut));
        CHECK(std::is_gt(mid + 1 <=> sut));
    }

    SUBCASE("range is [lower, upper]") {
        const auto sut = range(lower_bound, upper_bound);

        CHECK(std::is_eq(lower_bound <=> sut));
        CHECK(std::is_eq(upper_bound <=> sut));
        CHECK(std::is_eq(mid <=> sut));

        CHECK(std::is_lt(lower_bound - 1 <=> sut));
        CHECK(std::is_gt(upper_bound + 1 <=> sut));
    }
}

TEST_CASE("range builders should return correct range objects") {
    SUBCASE("at_least") {
        const auto sut = at_least(lower_bound);

        CHECK(std::is_eq(lower_bound <=> sut));
        CHECK(std::is_eq(upper_bound <=> sut));
        CHECK(std::is_eq(max_bound <=> sut));

        CHECK(std::is_lt(lower_bound - 1 <=> sut));
        CHECK(std::is_lt(min_bound <=> sut));
    }

    SUBCASE("more_than") {
        const auto sut = more_than(lower_bound);

        CHECK(std::is_eq(lower_bound + 1 <=> sut));
        CHECK(std::is_eq(upper_bound <=> sut));
        CHECK(std::is_eq(max_bound <=> sut));

        CHECK(std::is_lt(lower_bound <=> sut));
        CHECK(std::is_lt(min_bound <=> sut));
    }

    SUBCASE("less_than") {
        const auto sut = less_than(upper_bound);

        CHECK(std::is_eq(upper_bound - 1 <=> sut));
        CHECK(std::is_eq(lower_bound <=> sut));
        CHECK(std::is_eq(min_bound <=> sut));

        CHECK(std::is_gt(upper_bound <=> sut));
        CHECK(std::is_gt(max_bound <=> sut));
    }

    SUBCASE("up_to") {
        const auto sut = up_to(upper_bound);

        CHECK(std::is_eq(upper_bound <=> sut));
        CHECK(std::is_eq(lower_bound <=> sut));
        CHECK(std::is_eq(min_bound <=> sut));

        CHECK(std::is_gt(upper_bound + 1 <=> sut));
        CHECK(std::is_gt(max_bound <=> sut));
    }

    SUBCASE("any") {
        const auto sut = any();

        CHECK(std::is_eq(min_bound <=> sut));
        CHECK(std::is_eq(exact_bound <=> sut));
        CHECK(std::is_eq(lower_bound <=> sut));
        CHECK(std::is_eq(mid <=> sut));
        CHECK(std::is_eq(upper_bound <=> sut));
        CHECK(std::is_eq(max_bound <=> sut));
    }
}

TEST_SUITE_END(); // test_nargs_range
