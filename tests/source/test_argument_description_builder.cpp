#define AP_TESTING

#include "doctest.h"

#include <ap/argument/optional.hpp>
#include <ap/argument/positional.hpp>
#include <ap/detail/argument_description_builder.hpp>

using sut_type = ap::detail::argument_description_builder;

TEST_SUITE_BEGIN("test_argument_description_builder");

TEST_CASE("dummy") {
    sut_type sut;
}

TEST_SUITE_END(); // test_argument_description_builder
