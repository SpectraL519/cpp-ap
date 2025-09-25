#include "argument_test_fixture.hpp"
#include "doctest.h"
#include "utility.hpp"

using namespace ap_testing;
using namespace ap::nargs;

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
constexpr sut_value_type valid_value = 1;

const std::vector<sut_value_type> choices{1, 2, 3};
constexpr sut_value_type invalid_choice = 4;

const range non_default_range = range{1ull, choices.size()};

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
    argument_test_fixture,
    "help_builder(verbose=false) should return an help_builder with no params"
) {
    constexpr bool verbose = false;

    auto sut = sut_type(arg_name);

    auto bld = get_help_builder(sut, verbose);
    REQUIRE_EQ(bld.name, get_name(sut).str());
    CHECK_FALSE(bld.help);
    CHECK(bld.params.empty());

    // with a help msg
    sut.help(help_msg);
    bld = get_help_builder(sut, verbose);
    REQUIRE_EQ(bld.name, get_name(sut).str());
    CHECK(bld.help);
    CHECK_EQ(bld.help.value(), help_msg);
    CHECK(bld.params.empty());
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "help_builder(verbose=true) should return an help_builder with non-default params"
) {
    constexpr bool verbose = true;

    auto sut = sut_type(arg_name);

    auto bld = get_help_builder(sut, verbose);
    REQUIRE_EQ(bld.name, get_name(sut).str());
    CHECK_FALSE(bld.help);
    CHECK(bld.params.empty());

    // with a help msg
    sut.help(help_msg);

    bld = get_help_builder(sut, verbose);
    REQUIRE(bld.help);
    CHECK_EQ(bld.help.value(), help_msg);
    CHECK(bld.params.empty());

    // other parameters
    sut.bypass_required();
    sut.nargs(non_default_range);
    sut.choices(choices);
    sut.default_values(default_value);

    // check the descriptor parameters
    bld = get_help_builder(sut, verbose);

    const auto bypass_required_it =
        std::ranges::find(bld.params, "bypass required", &parameter_descriptor::name);
    REQUIRE_NE(bypass_required_it, bld.params.end());
    CHECK_EQ(bypass_required_it->value, "true");

    // automatically set to false with bypass_required
    const auto required_it = std::ranges::find(bld.params, "required", &parameter_descriptor::name);
    REQUIRE_NE(required_it, bld.params.end());
    CHECK_EQ(required_it->value, "false");

    const auto nargs_it = std::ranges::find(bld.params, "nargs", &parameter_descriptor::name);
    REQUIRE_NE(nargs_it, bld.params.end());
    CHECK_EQ(nargs_it->value, ap::util::as_string(non_default_range));

    const auto choices_it = std::ranges::find(bld.params, "choices", &parameter_descriptor::name);
    REQUIRE_NE(choices_it, bld.params.end());
    CHECK_EQ(choices_it->value, ap::util::join(choices, ", "));

    const auto default_value_it =
        std::ranges::find(bld.params, "default value(s)", &parameter_descriptor::name);
    REQUIRE_NE(default_value_it, bld.params.end());
    CHECK_EQ(default_value_it->value, std::to_string(default_value));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "is_hidden() should return false by default or the value passed in the attribute setter"
) {
    auto sut = sut_type(arg_name_primary);
    REQUIRE_FALSE(sut.is_hidden());

    sut.hidden();
    CHECK(sut.is_hidden());
}

TEST_CASE_FIXTURE(argument_test_fixture, "is_required() should return true by default") {
    auto sut = sut_type(arg_name_primary);
    CHECK(sut.is_required());
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "is_required() should return the value set using the `required` param setter"
) {
    auto sut = sut_type(arg_name_primary);

    sut.required(false);
    CHECK_FALSE(sut.is_required());

    sut.required();
    CHECK(sut.is_required());
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "bypass_required() should return the value set using the `bypass_required` param setter"
) {
    auto sut = sut_type(arg_name_primary);

    sut.bypass_required(true);
    CHECK(sut.is_bypass_required_enabled());

    sut.bypass_required(false);
    CHECK_FALSE(sut.is_bypass_required_enabled());
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "is_bypass_required_enabled() should return true only if the `required` flag is set to false "
    "and "
    "the `bypass_required` flags is set to true"
) {
    auto sut = sut_type(arg_name_primary);

    // disabled
    set_required(sut, false);
    set_bypass_required(sut, false);
    CHECK_FALSE(sut.is_bypass_required_enabled());

    set_required(sut, true);
    set_bypass_required(sut, false);
    CHECK_FALSE(sut.is_bypass_required_enabled());

    set_required(sut, true);
    set_bypass_required(sut, true);
    CHECK_FALSE(sut.is_bypass_required_enabled());

    // enabled
    set_required(sut, false);
    set_bypass_required(sut, true);
    CHECK(sut.is_bypass_required_enabled());
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "required(true) should disable `bypass_required` option and bypass_required(true) should "
    "disable the `required` option"
) {
    auto sut = sut_type(arg_name_primary);

    REQUIRE(sut.is_required());
    REQUIRE_FALSE(sut.is_bypass_required_enabled());

    sut.bypass_required();
    CHECK(sut.is_bypass_required_enabled());
    CHECK_FALSE(sut.is_required());

    sut.required();
    CHECK(sut.is_required());
    CHECK_FALSE(sut.is_bypass_required_enabled());
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

    set_value(sut, valid_value);
    CHECK(is_used(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "count() should return 0 by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_EQ(get_count(sut), 0ull);
}

TEST_CASE_FIXTURE(argument_test_fixture, "count() should return 1 when argument contains a value") {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, valid_value);

    CHECK_EQ(get_count(sut), 1ull);
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_value() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_value() should return true if the value is set") {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, valid_value);

    CHECK(has_value(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "has_value() should return true if the default value is set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_values(default_value);

    CHECK(has_value(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_parsed_values() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "has_parsed_values() should return false regardless of the default value parameter"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_values(default_value);

    CHECK_FALSE(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_parsed_values() should true if the value is set") {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, valid_value);

    CHECK(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(argument_test_fixture, "has_predefined_values() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(has_predefined_values(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "has_predefined_values() should return true if the default value is set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_values(default_value);

    CHECK(has_predefined_values(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "value() should throw if the argument's value has not been set"
) {
    auto sut = sut_type(arg_name_primary);

    REQUIRE_FALSE(has_value(sut));
    CHECK_THROWS_AS(static_cast<void>(get_value(sut)), std::logic_error);
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "value() should return the argument's value if it has been set"
) {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, valid_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), valid_value);
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "value() should return the default argument's default value if it has been set and no values "
    "were parsed"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_values(default_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), default_value);
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "value() should return the argument's parsed value if it has been set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_values(default_value);
    set_value(sut, valid_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), valid_value);
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
            invalid_value_msg(arg_name_primary, empty_str).c_str(),
            parsing_failure
        );
        CHECK_FALSE(has_value(sut));
    }

    SUBCASE("given string is non-convertible to value_type") {
        REQUIRE_THROWS_WITH_AS(
            set_value(sut, invalid_value_str),
            invalid_value_msg(arg_name_primary, invalid_value_str).c_str(),
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
        invalid_choice_msg(arg_name_primary, std::to_string(invalid_choice)).c_str(),
        parsing_failure
    );
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "set_value(any) should throw when adding the given value would result in exceeding the maximum "
    "number of values specified by nargs"
) {
    auto sut = sut_type(arg_name_primary).nargs(non_default_range);

    for (const auto value : choices)
        REQUIRE_NOTHROW(set_value(sut, value));

    const auto stored_values = get_values(sut);
    REQUIRE_EQ(stored_values.size(), choices.size());
    for (std::size_t i = 0; i < stored_values.size(); ++i)
        REQUIRE_EQ(std::any_cast<sut_value_type>(stored_values[i]), choices[i]);

    CHECK_THROWS_WITH_AS(
        set_value(sut, valid_value),
        parsing_failure::invalid_nvalues(arg_name_primary, std::weak_ordering::greater).what(),
        parsing_failure
    );
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

        set_value(sut, valid_value);

        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), double_action(valid_value));
    }

    SUBCASE("modify action") {
        const auto double_action = [](sut_value_type& value) { value *= 2; };
        sut.action<ap::action_type::modify>(double_action);

        auto test_value = valid_value;

        set_value(sut, test_value);

        double_action(test_value);
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), test_value);
    }
}

TEST_CASE_FIXTURE(
    argument_test_fixture, "nvalues_ordering() should return less for default nargs (1)"
) {
    const auto sut = sut_type(arg_name_primary);
    CHECK(std::is_lt(nvalues_ordering(sut)));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "nvalues_ordering() should return equivalent if a default value has been set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.nargs(non_default_range);

    sut.default_values(default_value);

    CHECK(std::is_eq(nvalues_ordering(sut)));
}

TEST_CASE_FIXTURE(
    argument_test_fixture,
    "nvalues_ordering() should return equivalent only when the number of values "
    "is in the specified range"
) {
    auto sut = sut_type(arg_name_primary);
    sut.nargs(non_default_range);

    REQUIRE(std::is_lt(nvalues_ordering(sut)));

    for (const auto value : choices) {
        REQUIRE_NOTHROW(set_value(sut, value));
        CHECK(std::is_eq(nvalues_ordering(sut)));
    }

    set_value_force(sut, invalid_choice);
    CHECK(std::is_gt(nvalues_ordering(sut)));
}
