#include "doctest.h"

#include <ap/detail/str_utility.hpp>

#include <array>
#include <format>
#include <vector>

using namespace ap::detail;

namespace {

constexpr std::string_view delimiter = ",";

struct dummy_writable {
    int x;
    int y;
};

std::ostream& operator<<(std::ostream& os, const dummy_writable& dw) {
    os << dw.x << "," << dw.y;
    return os;
}

} // namespace

TEST_CASE("as_string should convert the given writable object to string") {
    int value = 5;
    CHECK_EQ(as_string(value), std::to_string(value));

    dummy_writable dw{3, 14};
    std::ostringstream dw_oss;
    dw_oss << dw;
    CHECK_EQ(as_string(dw), dw_oss.str());
}

TEST_CASE("contains_whitespace should return true for strings containing at least one whitespace "
          "characted") {
    CHECK_FALSE(contains_whitespaces("str-without-whitespaces"));

    constexpr std::array<char, 6> whitespace_chars = {' ', '\t', '\n', '\v', '\f', '\r'};
    for (char c : whitespace_chars)
        CHECK(contains_whitespaces(std::format("begin{}end", c)));
}

TEST_CASE("join should return an empty string for an empty range") {
    std::vector<int> range{};
    CHECK_EQ(join(range, delimiter), "");
}

TEST_CASE("join should return a string with no delimiters for a single element range") {
    std::vector<int> range = {1};
    CHECK_EQ(join(range, delimiter), "1");
}

TEST_CASE("join should return a proper range representation for a multi element range") {
    std::vector<int> range = {1, 2, 3};
    CHECK_EQ(join(range, delimiter), "1,2,3");
}
