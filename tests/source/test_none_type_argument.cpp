#include "argument_test_fixture.hpp"
#include "doctest.h"

using namespace ap_testing;

using ap::optional_argument;
using ap::parsing_failure;
using ap::detail::argument_name;

namespace {

constexpr std::string_view primary_name = "test";
const auto primary_name_opt = std::make_optional<std::string>(primary_name);

constexpr std::string_view secondary_name = "t";
const auto secondary_name_opt = std::make_optional<std::string>(secondary_name);

const argument_name arg_name(primary_name_opt, secondary_name_opt);

using sut_value_type = ap::none_type;
using sut_type = optional_argument<sut_value_type>;

const std::string some_value = "some-value";

} // namespace

TEST_CASE_FIXTURE(
    argument_test_fixture, "mark_used should always return false (further values not accepted)"
) {
    auto sut = sut_type(arg_name);
    CHECK_FALSE(mark_used(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "set_value should always throw and no values should be stored"
) {
    auto sut = sut_type(arg_name);

    CHECK_THROWS_WITH_AS(
        set_value(sut, some_value),
        std::format(
            "Cannot set values for a none-type argument '{}' (value: '{}')",
            arg_name.str(),
            some_value
        )
            .c_str(),
        parsing_failure
    );
}
