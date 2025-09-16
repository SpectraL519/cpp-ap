#include "doctest.h"

#include <ap/argument.hpp>
#include <ap/detail/argument_token.hpp>

using sut_type = ap::detail::argument_token;
using enum sut_type::token_type;
using ap::optional_argument;
using ap::detail::argument_base;
using ap::detail::argument_name;

namespace {

const std::string token_value_1 = "tok-val-1";
const std::string token_value_2 = "tok-val-2";

} // namespace

TEST_CASE("argument tokens should not be equal if either their types or values do not match") {
    CHECK_NE(sut_type{t_value, token_value_1}, sut_type{t_value, token_value_2});
    CHECK_NE(sut_type{t_value, token_value_1}, sut_type{t_flag_primary, token_value_1});
}

TEST_CASE("argument tokens should be equal if their types and values match") {
    const std::string test_value = "test-token";
    const std::pair<sut_type::token_type, std::string> test_cases[] = {
        {         t_value, token_value_1},
        {  t_flag_primary, token_value_1},
        {t_flag_secondary, token_value_1},
        {         t_value, token_value_2},
        {  t_flag_primary, token_value_2},
        {t_flag_secondary, token_value_2}
    };

    for (const auto& [token_type, value] : test_cases) {
        CAPTURE(token_type);
        CAPTURE(value);
        CHECK_EQ(sut_type{token_type, value}, sut_type{token_type, value});
    }
}

TEST_CASE("is_flag_token should return true if the token's type is either primary or secondary flag"
) {
    CHECK(sut_type{t_flag_primary, ""}.is_flag_token());
    CHECK(sut_type{t_flag_secondary, ""}.is_flag_token());

    CHECK_FALSE(sut_type{t_value, ""}.is_flag_token());
}

TEST_CASE("is_valid_flag_token should return true if the token is a flag token and it's arg member "
          "is set") {
    CHECK_FALSE(sut_type{t_value, ""}.is_valid_flag_token());

    CHECK_FALSE(sut_type{t_flag_primary, ""}.is_valid_flag_token());
    CHECK_FALSE(sut_type{t_flag_secondary, ""}.is_valid_flag_token());

    std::unique_ptr<argument_base> arg_ptr =
        std::make_unique<optional_argument<>>(argument_name{""});
    const typename sut_type::arg_ptr_opt_t arg_ptr_opt{std::ref(arg_ptr)};

    CHECK(sut_type{t_flag_primary, "", arg_ptr_opt}.is_valid_flag_token());
    CHECK(sut_type{t_flag_secondary, "", arg_ptr_opt}.is_valid_flag_token());
}
