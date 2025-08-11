#include "doctest.h"

#include <stdexcept>

namespace {

int add(int a, int b) {
    return a + b;
}

float multiply(float a, float b) {
    return a * b;
}

int divide(const int numerator, const int denominator) {
    if (not denominator)
        throw std::logic_error("Division by zero");
    return numerator / denominator;
}

} // namespace

TEST_CASE("addition of integers") {
    int a = 5;
    int b = 10;
    CHECK(add(a, b) == 15);
}

TEST_CASE("multiplication of floats") {
    constexpr float x = 2.5f;
    constexpr float y = 3.0f;
    CHECK(multiply(x, y) == doctest::Approx(7.5).epsilon(0.01));
}

TEST_CASE("division by zero") {
    int numerator = 10;
    int denominator = 0;
    CHECK_THROWS_AS(divide(numerator, denominator), std::logic_error);
}
