#include "argument_test_fixture.hpp"
#include "doctest.h"
#include "utility.hpp"

using namespace ap_testing;

using ap::parsing_failure;
using ap::positional_argument;
using ap::detail::argument_name;
using ap::detail::parameter_descriptor;

namespace {

constexpr std::string_view help_msg = "test help msg";

constexpr std::string_view primary_name = "test";
const auto primary_name_opt = std::make_optional<std::string>(primary_name);

constexpr std::string_view secondary_name = "t";
const auto secondary_name_opt = std::make_optional<std::string>(secondary_name);

const argument_name arg_name(primary_name_opt, secondary_name_opt);
const argument_name arg_name_primary(primary_name_opt, std::nullopt);

using sut_value_type = int;
using sut_type = positional_argument<sut_value_type>;

const std::string empty_str = "";
const std::string invalid_value_str = "invalid value";

constexpr sut_value_type default_value = 0;
constexpr sut_value_type value_1 = 1;
constexpr sut_value_type value_2 = 2;

const std::vector<sut_value_type> choices{1, 2, 3};
constexpr sut_value_type invalid_choice = 4;

} // namespace

TEST_CASE_FIXTURE(argument_test_fixture, "name() should return the proper argument_name instance") {
    SUBCASE("initialized with the primary name only") {
        const auto sut = sut_type(arg_name_primary);
        const auto name = get_name(sut);

        CHECK(name.match(primary_name));
        CHECK_FALSE(name.match(secondary_name));
    }

    SUBCASE("initialized with the primary and secondary names") {
        const auto sut = sut_type(arg_name);
        const auto name = get_name(sut);

        CHECK(name.match(primary_name));
        CHECK(name.match(secondary_name));
    }
}

TEST_CASE_FIXTURE(argument_test_fixture, "help() should return nullopt by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(get_help(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "help() should return a massage set for the argument") {
    auto sut = sut_type(arg_name_primary);
    sut.help(help_msg);

    const auto stored_help_msg = get_help(sut);

    REQUIRE(stored_help_msg);
    CHECK_EQ(stored_help_msg, help_msg);
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "desc(verbose=false) should return an argument_descriptor with no params"
) {
    constexpr bool verbose = false;

    auto sut = sut_type(arg_name);

    auto desc = get_desc(sut, verbose);
    REQUIRE_EQ(desc.name, get_name(sut).str());
    CHECK_FALSE(desc.help);
    CHECK(desc.params.empty());

    // with a help msg
    sut.help(help_msg);
    desc = get_desc(sut, verbose);
    REQUIRE_EQ(desc.name, get_name(sut).str());
    CHECK(desc.help);
    CHECK_EQ(desc.help.value(), help_msg);
    CHECK(desc.params.empty());
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "desc(verbose=true) should return an argument_descriptor with non-default params"
) {
    constexpr bool verbose = true;

    auto sut = sut_type(arg_name);

    auto desc = get_desc(sut, verbose);
    REQUIRE_EQ(desc.name, get_name(sut).str());
    CHECK_FALSE(desc.help);
    CHECK(desc.params.empty());

    // with a help msg
    sut.help(help_msg);

    desc = get_desc(sut, verbose);
    REQUIRE(desc.help);
    CHECK_EQ(desc.help.value(), help_msg);
    CHECK(desc.params.empty());

    // other parameters
    sut.bypass_required();
    sut.choices(choices);
    sut.default_value(default_value);

    // check the descriptor parameters
    desc = get_desc(sut, verbose);

    const auto bypass_required_it =
        std::ranges::find(desc.params, "bypass required", &parameter_descriptor::name);
    REQUIRE_NE(bypass_required_it, desc.params.end());
    CHECK_EQ(bypass_required_it->value, "true");

    // automatically set to false with bypass_required
    const auto required_it =
        std::ranges::find(desc.params, "required", &parameter_descriptor::name);
    REQUIRE_NE(required_it, desc.params.end());
    CHECK_EQ(required_it->value, "false");

    const auto choices_it = std::ranges::find(desc.params, "choices", &parameter_descriptor::name);
    REQUIRE_NE(choices_it, desc.params.end());
    CHECK_EQ(choices_it->value, ap::detail::join(choices, ", "));

    const auto default_value_it =
        std::ranges::find(desc.params, "default value", &parameter_descriptor::name);
    REQUIRE_NE(default_value_it, desc.params.end());
    CHECK_EQ(default_value_it->value, std::to_string(default_value));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "is_hidden() should return false by default or the value passed in the attribute setter"
) {
    auto sut = sut_type(arg_name_primary);
    REQUIRE_FALSE(is_hidden(sut));

    sut.hidden();
    CHECK(is_hidden(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "is_required() should return true by default") {
    auto sut = sut_type(arg_name_primary);
    CHECK(is_required(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "is_required() should return the value set using the `required` param setter"
) {
    auto sut = sut_type(arg_name_primary);

    sut.required(false);
    CHECK_FALSE(is_required(sut));

    sut.required();
    CHECK(is_required(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "bypass_required() should return the value set using the `bypass_required` param setter"
) {
    auto sut = sut_type(arg_name_primary);

    sut.bypass_required(true);
    CHECK(is_bypass_required_enabled(sut));

    sut.bypass_required(false);
    CHECK_FALSE(is_bypass_required_enabled(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "bypass_required_enabled() should return true only if the `required` flag is set to false and "
    "the `bypass_required` flags is set to true"
) {
    auto sut = sut_type(arg_name_primary);

    // disabled
    set_required(sut, false);
    set_bypass_required(sut, false);
    CHECK_FALSE(is_bypass_required_enabled(sut));

    set_required(sut, true);
    set_bypass_required(sut, false);
    CHECK_FALSE(is_bypass_required_enabled(sut));

    set_required(sut, true);
    set_bypass_required(sut, true);
    CHECK_FALSE(is_bypass_required_enabled(sut));

    // enabled
    set_required(sut, false);
    set_bypass_required(sut, true);
    CHECK(is_bypass_required_enabled(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "required(true) should disable `bypass_required` option and bypass_required(true) should "
    "disable the `required` option"
) {
    auto sut = sut_type(arg_name_primary);

    REQUIRE(is_required(sut));
    REQUIRE_FALSE(is_bypass_required_enabled(sut));

    sut.bypass_required();
    CHECK(is_bypass_required_enabled(sut));
    CHECK_FALSE(is_required(sut));

    sut.required();
    CHECK(is_required(sut));
    CHECK_FALSE(is_bypass_required_enabled(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "is_used() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(is_used(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "is_used() should return true when argument contains a value"
) {
    auto sut = sut_type(arg_name_primary);
    REQUIRE_FALSE(is_used(sut));

    set_value(sut, value_1);
    CHECK(is_used(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "count() should return 0 by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_EQ(get_count(sut), 0ull);
}

TEST_CASE_FIXTURE(argument_test_fixture, "count() should return 1 when argument contains a value") {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, value_1);

    CHECK_EQ(get_count(sut), 1ull);
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_value() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_value() should return true if the value is set") {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, value_1);

    CHECK(has_value(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "has_value() should return true if the default value is set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_value(default_value);

    CHECK(has_value(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_parsed_values() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "has_parsed_values() should false if only the default value is set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_value(default_value);

    CHECK_FALSE(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_parsed_values() should true if the value is set") {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, value_1);

    CHECK(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "set_value(any) should throw when a value has already been set"
) {
    auto sut = sut_type(arg_name_primary);

    REQUIRE_NOTHROW(set_value(sut, value_1));
    REQUIRE(has_value(sut));

    CHECK_THROWS_WITH_AS(
        set_value(sut, value_2),
        parsing_failure::invalid_nvalues(arg_name_primary, std::weak_ordering::greater).what(),
        parsing_failure
    );
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "set_value(any) should throw when the given string cannot be converted to an instance of "
    "value_type"
) {
    auto sut = sut_type(arg_name_primary);

    SUBCASE("given string is empty") {
        REQUIRE_THROWS_WITH_AS(
            set_value(sut, empty_str),
            parsing_failure::invalid_value(arg_name_primary, empty_str).what(),
            parsing_failure
        );
        CHECK_FALSE(has_value(sut));
    }

    SUBCASE("given string is non-convertible to value_type") {
        REQUIRE_THROWS_WITH_AS(
            set_value(sut, invalid_value_str),
            parsing_failure::invalid_value(arg_name_primary, invalid_value_str).what(),
            parsing_failure
        );
        CHECK_FALSE(has_value(sut));
    }
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "set_value(any) should throw when the choices set does not contain the parsed value"
) {
    auto sut = sut_type(arg_name_primary);
    sut.choices(choices);

    REQUIRE_THROWS_WITH_AS(
        set_value(sut, invalid_choice),
        parsing_failure::invalid_choice(arg_name_primary, std::to_string(invalid_choice)).what(),
        parsing_failure
    );
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "set_value(any) should accept the given value only when no value has been set yet "
    "and if the given value is present in the choices set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.choices(choices);

    for (const sut_value_type& value : choices) {
        reset_values(sut);
        REQUIRE_FALSE(has_value(sut));

        REQUIRE_NOTHROW(set_value(sut, value));
        REQUIRE(has_value(sut));
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), value);
    }
}

TEST_CASE_FIXTURE(argument_test_fixture, "set_value(any) should perform the specified action") {
    auto sut = sut_type(arg_name_primary);

    SUBCASE("observe action") {
        const auto is_power_of_two = [](const sut_value_type n) {
            if (not ((n > 0) and (n & (n - 1)) == 0)) {
                throw std::runtime_error(std::format("Value `{}` is not a power of 2", n));
            }
        };

        sut.action<ap::action_type::observe>(is_power_of_two);

        CHECK_THROWS_AS(set_value(sut, 3), std::runtime_error);

        sut_value_type valid_value = 16;
        REQUIRE_NOTHROW(set_value(sut, valid_value));
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), valid_value);
    }

    SUBCASE("transform action") {
        const auto double_action = [](const sut_value_type& value) { return 2 * value; };
        sut.action<ap::action_type::transform>(double_action);

        set_value(sut, value_1);

        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), double_action(value_1));
    }

    SUBCASE("modify action") {
        const auto double_action = [](sut_value_type& value) { value *= 2; };
        sut.action<ap::action_type::modify>(double_action);

        auto test_value = value_1;

        set_value(sut, test_value);

        double_action(test_value);
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), test_value);
    }
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "value() should throw if the argument's value has not been set"
) {
    auto sut = sut_type(arg_name_primary);

    REQUIRE_FALSE(has_value(sut));
    CHECK_THROWS_AS(static_cast<void>(get_value(sut)), std::logic_error);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "value() should return the default argument's value if it has been set and no values were "
    "parsed"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_value(default_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), default_value);
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "value() should return the argument's value if it has been set"
) {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, value_1);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "value() should return the argument's parsed value if it has been set (with a defined default "
    "value)"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_value(default_value);
    set_value(sut, value_1);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), value_1);
}

TEST_CASE_FIXTURE(argument_test_fixture, "nvalues_ordering() should return less by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK(std::is_lt(nvalues_ordering(sut)));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "nvalues_ordering() should return equivalent if a value has been set"
) {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, value_1);

    CHECK(std::is_eq(nvalues_ordering(sut)));
}
