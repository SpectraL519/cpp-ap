#include "doctest.h"

/*
This test file was created to determine whether the doctest library
and testing CMakeLists.txt file have been set up correctly :)
*/

TEST_SUITE_BEGIN("test_boolean");

TEST_CASE("true should be true") {
    REQUIRE(true == true);
}

TEST_CASE("true should not be false") {
    REQUIRE_FALSE(true == false);
}

TEST_CASE("false should be false") {
    REQUIRE(false == false);
}

TEST_CASE("false should not be true") {
    REQUIRE_FALSE(false == true);
}

TEST_SUITE_END();
