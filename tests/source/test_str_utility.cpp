#define AP_TESTING

#include "doctest.h"

#include <ap/detail/str_utility.hpp>

#include <vector>

using namespace ap::detail;

namespace {

constexpr std::string_view delimiter = ",";

} // namespace

TEST_SUITE_BEGIN("test_str_utility");

TEST_CASE("join_with should return an empty string for an empty range") {
    std::vector<int> range{};
    CHECK_EQ(join_with(range, delimiter), "");
}

TEST_CASE("join_with should return a string with no delimiters for a single element range") {
    std::vector<int> range = {1};
    CHECK_EQ(join_with(range, delimiter), "1");
}

TEST_CASE("join_with should return a proper range representation for a multi element range") {
    std::vector<int> range = {1, 2, 3};
    CHECK_EQ(join_with(range, delimiter), "1,2,3");
}

TEST_SUITE_END(); // test_str_utility
