#include "doctest.h"
#include "optional_argument_test_fixture.hpp"

#include <ap/detail/str_utility.hpp>

using namespace ap_testing;
using namespace ap::nargs;

using ap::parsing_failure;
using ap::argument::optional;
using ap::detail::argument_name;
using ap::detail::parameter_descriptor;

namespace {

constexpr char flag_char = '-';
constexpr std::string_view primary_name = "test";
constexpr std::string_view secondary_name = "t";

const argument_name arg_name(primary_name, secondary_name, flag_char);
const argument_name arg_name_primary(primary_name, std::nullopt, flag_char);

constexpr std::string_view help_msg = "test help msg";

using sut_value_type = int;
using invalid_value_type = double;
using sut_type = optional<sut_value_type>;

constexpr std::string empty_str = "";
constexpr std::string invalid_value_str = "invalid_value";

constexpr sut_value_type arbitrary_value = 1;
constexpr sut_value_type default_value = 0;
constexpr sut_value_type implicit_value = -1;

const std::vector<sut_value_type> choices{1, 2, 3};
constexpr sut_value_type invalid_choice = 4;

const range non_default_range = range{1ull, choices.size()};

} // namespace

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "name() should return the proper argument_name instance"
) {
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

TEST_CASE_FIXTURE(optional_argument_test_fixture, "help() should return nullopt by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(get_help(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "help() should return a massage set for the argument"
) {
    auto sut = sut_type(arg_name_primary);
    sut.help(help_msg);

    const auto stored_help_msg = get_help(sut);

    REQUIRE(stored_help_msg);
    CHECK_EQ(stored_help_msg, help_msg);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "desc(verbose=false) should return an argument_descriptor with no params"
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
    optional_argument_test_fixture,
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

    // with the required flag enabled
    sut.required();

    desc = get_desc(sut, verbose);
    const auto required_it =
        std::ranges::find(desc.params, "required", &parameter_descriptor::name);
    REQUIRE_NE(required_it, desc.params.end());
    CHECK_EQ(required_it->value, "true");

    // other parameters
    sut.bypass_required();
    sut.nargs(non_default_range);
    sut.choices(choices);
    sut.default_value(default_value);
    sut.implicit_value(implicit_value);

    // check the descriptor parameters
    desc = get_desc(sut, verbose);

    const auto bypass_required_it =
        std::ranges::find(desc.params, "bypass required", &parameter_descriptor::name);
    REQUIRE_NE(bypass_required_it, desc.params.end());
    CHECK_EQ(bypass_required_it->value, "true");

    const auto nargs_it = std::ranges::find(desc.params, "nargs", &parameter_descriptor::name);
    REQUIRE_NE(nargs_it, desc.params.end());
    CHECK_EQ(nargs_it->value, ap::detail::as_string(non_default_range));

    const auto choices_it = std::ranges::find(desc.params, "choices", &parameter_descriptor::name);
    REQUIRE_NE(choices_it, desc.params.end());
    CHECK_EQ(choices_it->value, ap::detail::join(choices, ", "));

    const auto default_value_it =
        std::ranges::find(desc.params, "default value", &parameter_descriptor::name);
    REQUIRE_NE(default_value_it, desc.params.end());
    CHECK_EQ(default_value_it->value, std::to_string(default_value));

    const auto implicit_value_it =
        std::ranges::find(desc.params, "implicit value", &parameter_descriptor::name);
    REQUIRE_NE(implicit_value_it, desc.params.end());
    CHECK_EQ(implicit_value_it->value, std::to_string(implicit_value));
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "is_required() should return false by default") {
    auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(is_required(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_required() should return the value set using the `required` param setter"
) {
    auto sut = sut_type(arg_name_primary);

    sut.required();
    CHECK(is_required(sut));

    sut.required(false);
    CHECK_FALSE(is_required(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "bypass_required() should return the value set using the `bypass_required` param setter"
) {
    auto sut = sut_type(arg_name_primary);

    sut.bypass_required(true);
    CHECK(is_bypass_required_enabled(sut));

    sut.bypass_required(false);
    CHECK_FALSE(is_bypass_required_enabled(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
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
    optional_argument_test_fixture,
    "required(true) should disable `bypass_required` option and bypass_required(true) should "
    "disable the `required` option"
) {
    auto sut = sut_type(arg_name_primary);

    REQUIRE_FALSE(is_required(sut));
    REQUIRE_FALSE(is_bypass_required_enabled(sut));

    sut.bypass_required();
    CHECK(is_bypass_required_enabled(sut));
    CHECK_FALSE(is_required(sut));

    sut.required();
    CHECK(is_required(sut));
    CHECK_FALSE(is_bypass_required_enabled(sut));

    sut.bypass_required();
    CHECK(is_bypass_required_enabled(sut));
    CHECK_FALSE(is_required(sut));
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "is_used() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(is_used(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_used() should return true if the argument's flag has been used"
) {
    auto sut = sut_type(arg_name_primary);
    REQUIRE_FALSE(is_used(sut));

    mark_used(sut);
    CHECK(is_used(sut));
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "count() should return 0 by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_EQ(get_count(sut), 0ull);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "is_used() should return the number of times the argument's flag has been used "
    "[number of mark_used() function calls]"
) {
    auto sut = sut_type(arg_name_primary);

    constexpr std::size_t count = 5ull;
    for (std::size_t n = 0ull; n < count; ++n)
        mark_used(sut);

    CHECK_EQ(get_count(sut), count);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "argument flag usage should trigger the on-flag actions"
) {
    auto sut = sut_type(arg_name_primary);

    const auto throw_action = []() { throw std::runtime_error("no reason"); };
    sut.action<ap::action_type::on_flag>(throw_action);

    CHECK_THROWS_AS(mark_used(sut), std::runtime_error);
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "has_value() should return false by default") {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(optional_argument_test_fixture, "has_value() should return true if value is set") {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, arbitrary_value);
    CHECK(has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return true if a default value is set and value() should return the "
    "default value"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_value(default_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), default_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return false if only the implicit value is set but the argument is not used"
) {
    auto sut = sut_type(arg_name_primary);
    sut.implicit_value(implicit_value);

    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_value() should return true if the implicit value is set and the agument is used"
) {
    auto sut = sut_type(arg_name_primary);
    sut.implicit_value(implicit_value);

    mark_used(sut);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), implicit_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "has_parsed_values() should return false by default"
) {
    const auto sut = sut_type(arg_name_primary);
    CHECK_FALSE(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "has_parsed_values() should return false regardles of the "
    "default_value and implicit_value parameters"
) {
    auto sut = sut_type(arg_name_primary);

    SUBCASE("default_value") {
        sut.default_value(default_value);
        CHECK_FALSE(has_parsed_values(sut));
    }

    SUBCASE("implicit_value") {
        sut.implicit_value(implicit_value);
        CHECK_FALSE(has_parsed_values(sut));
    }

    SUBCASE("default_value and implicit_value") {
        sut.default_value(default_value);
        sut.implicit_value(implicit_value);
        CHECK_FALSE(has_parsed_values(sut));
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "has_parsed_values() should true if the value is set"
) {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, arbitrary_value);
    CHECK(has_parsed_values(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "value() should throw if the argument's value has not been set"
) {
    auto sut = sut_type(arg_name_primary);

    REQUIRE_FALSE(has_value(sut));
    CHECK_THROWS_AS(static_cast<void>(get_value(sut)), std::logic_error);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "value() should return the argument's value if it has been set"
) {
    auto sut = sut_type(arg_name_primary);
    set_value(sut, arbitrary_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), arbitrary_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "value() should return the default value if one has been provided and argument is not used"
) {
    auto sut = sut_type(arg_name_primary);
    sut.default_value(arbitrary_value);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), arbitrary_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "value() should return the implicit value if one has been provided and argument is used"
) {
    auto sut = sut_type(arg_name_primary);
    sut.implicit_value(implicit_value);

    mark_used(sut);

    REQUIRE(has_value(sut));
    CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), implicit_value);
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
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
    optional_argument_test_fixture,
    "set_value(any) should throw when the choices set does not contain the parsed value"
) {
    auto sut = sut_type(arg_name_primary);
    sut.choices(choices);

    REQUIRE_THROWS_WITH_AS(
        set_value(sut, invalid_choice),
        parsing_failure::invalid_choice(arg_name_primary, as_string(invalid_choice)).what(),
        parsing_failure
    );
    CHECK_FALSE(has_value(sut));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "set_value(any) should accept any number of values by default"
) {
    auto sut = sut_type(arg_name_primary);
    sut.nargs(non_default_range);

    for (const auto value : choices) {
        REQUIRE_NOTHROW(set_value(sut, value));
    }

    const auto stored_values = get_values(sut);

    REQUIRE_EQ(stored_values.size(), choices.size());
    for (std::size_t i = 0; i < stored_values.size(); ++i) {
        REQUIRE_EQ(std::any_cast<sut_value_type>(stored_values[i]), choices[i]);
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "set_value(any) should throw when adding the given value would result in exceeding the maximum "
    "number of values specified by nargs"
) {
    auto sut = sut_type(arg_name_primary).nargs(non_default_range);

    for (const auto value : choices) {
        REQUIRE_NOTHROW(set_value(sut, value));
    }

    CHECK_THROWS_WITH_AS(
        set_value(sut, arbitrary_value),
        parsing_failure::invalid_nvalues(arg_name_primary, std::weak_ordering::greater).what(),
        parsing_failure
    );
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture, "set_value(any) should perform the specified value action"
) {
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

        set_value(sut, arbitrary_value);

        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), double_action(arbitrary_value));
    }

    SUBCASE("modify action") {
        const auto double_action = [](sut_value_type& value) { value *= 2; };
        sut.action<ap::action_type::modify>(double_action);

        auto test_value = arbitrary_value;

        set_value(sut, test_value);

        double_action(test_value);
        CHECK_EQ(std::any_cast<sut_value_type>(get_value(sut)), test_value);
    }
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "nvalues_ordering() should return equivalent for default nargs (any)"
) {
    const auto sut = sut_type(arg_name_primary);
    CHECK(std::is_eq(nvalues_ordering(sut)));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
    "nvalues_ordering() should return equivalent if a default value has been set"
) {
    auto sut = sut_type(arg_name_primary);
    sut.nargs(non_default_range);

    sut.default_value(default_value);

    CHECK(std::is_eq(nvalues_ordering(sut)));
}

TEST_CASE_FIXTURE(
    optional_argument_test_fixture,
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
