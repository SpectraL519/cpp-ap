#define AP_TESTING

#include "../doctest.h"
#include "../testing_friend_definitions.hpp"

#include <ap/argument_parser.hpp>

using namespace ap::detail;



namespace {

using correct_type = int;
using invalid_type = double;

} // namespace



TEST_SUITE("test_holds_type") {
    TEST_CASE("holds_type should return false if std::any object holds no value") {
        std::any any;

        REQUIRE_FALSE(holds_type<correct_type>(any));
        REQUIRE_FALSE(holds_type<invalid_type>(any));
    }

    TEST_CASE("holds_type should return true only when std::any object holds value of given type") {
        constexpr correct_type test_value{};
        std::any any = static_cast<correct_type>(test_value);

        REQUIRE(holds_type<correct_type>(any));
        REQUIRE_FALSE(holds_type<invalid_type>(any));
    }
}
