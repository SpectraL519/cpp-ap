#include "doctest.h"

#include <ap/detail/help_builder.hpp>

#include <cstdint>

TEST_SUITE_BEGIN("test_help_builder");

using sut_type = argon::detail::help_builder;

namespace {

const std::string arg_name = "test-arg";
const std::string help_msg = "test-arg help message";

constexpr uint8_t indent_width = 2;
constexpr std::size_t align_to = 15ull;

} // namespace

TEST_CASE("help_builder should construct with name and optional help correctly") {
    sut_type no_help(arg_name, std::nullopt);
    CHECK_EQ(no_help.name, arg_name);
    CHECK_FALSE(no_help.help.has_value());

    sut_type with_help(arg_name, help_msg);
    CHECK_EQ(with_help.name, arg_name);
    CHECK(with_help.help.has_value());
    CHECK_EQ(with_help.help.value(), help_msg);
}

TEST_CASE("add_param should add string parameters correctly") {
    const std::string param_name = "param";
    const std::string param_value = "value";

    sut_type sut(arg_name, std::nullopt);
    sut.add_param(param_name, param_value);

    REQUIRE_EQ(sut.params.size(), 1ull);
    CHECK_EQ(sut.params.front().name, param_name);
    CHECK_EQ(sut.params.front().value, param_value);
}

TEST_CASE("add_param<T> should add writable parameters correctly") {
    const std::string bool_param = "bool-param";
    const bool bool_value = true;

    const std::string int_param = "int-param";
    const int int_value = 42;

    const std::string double_param = "double-param";
    const double double_value = 3.14;

    sut_type sut(arg_name, std::nullopt);

    sut.add_param(bool_param, bool_value);
    sut.add_param(int_param, int_value);
    sut.add_param(double_param, double_value);

    REQUIRE(sut.params.size() == 3ull);
    CHECK_EQ(sut.params[0].name, bool_param);
    CHECK_EQ(sut.params[0].value, "true");
    CHECK_EQ(sut.params[1].name, int_param);
    CHECK_EQ(sut.params[1].value, "42");
    CHECK_EQ(sut.params[2].name, double_param);
    CHECK_EQ(sut.params[2].value, "3.14");
}

TEST_CASE("add_range_param should properly add a joined range parameter") {
    const std::string param_name = "range-param";
    const std::vector<int> values = {1, 2, 3};
    const std::string_view delimiter = "; ";

    sut_type sut("range_test", std::nullopt);
    sut.add_range_param(param_name, values, delimiter);

    REQUIRE(sut.params.size() == 1);
    CHECK_EQ(sut.params[0].name, param_name);
    CHECK_EQ(sut.params[0].value, "1; 2; 3");
}

TEST_CASE("get_basic should return only the argument name if no help is provided") {
    sut_type sut(arg_name, std::nullopt);

    std::string basic = sut.get_basic(indent_width);
    CHECK_EQ(basic, std::string(indent_width, ' ') + arg_name);

    std::ostringstream expected_aligned;
    expected_aligned
        << std::string(indent_width, ' ') << std::setw(align_to) << std::left << arg_name;
    CHECK_EQ(sut.get_basic(indent_width, align_to), expected_aligned.str());
}

TEST_CASE("get_basic should include help message if provided") {
    sut_type sut(arg_name, help_msg);

    // Unaligned output (no align_to)
    std::string basic = sut.get_basic(indent_width);
    std::string expected_basic = std::string(indent_width, ' ') + arg_name + " : " + help_msg;
    CHECK_EQ(basic, expected_basic);

    // Aligned output (with align_to)
    std::ostringstream expected_aligned;
    expected_aligned
        << std::string(indent_width, ' ') << std::setw(align_to) << std::left << arg_name << " : "
        << help_msg;

    CHECK_EQ(sut.get_basic(indent_width, align_to), expected_aligned.str());
}

TEST_CASE("get should return single-line string if within max_line_width") {
    const std::string param_name = "param";
    const std::string param_value = "value";

    constexpr std::size_t max_line_width = std::numeric_limits<std::size_t>::max();

    sut_type sut(arg_name, help_msg);
    sut.add_param(param_name, param_value);

    CHECK_EQ(
        sut.get(indent_width, max_line_width),
        std::format(
            "{}{} : {} ({}: {})",
            std::string(indent_width, ' '),
            arg_name,
            help_msg,
            param_name,
            param_value
        )
    );
}

TEST_CASE("get should fall back to multiline output if string is too wide") {
    constexpr std::size_t max_line_width = 0; // force multi-line
    const std::string param1_name = "flag";
    const std::string param1_value = "on";
    const std::string param2_name = "mode";
    const std::string param2_value = "expert";

    sut_type sut(arg_name, help_msg);
    sut.add_param(param1_name, param1_value);
    sut.add_param(param2_name, param2_value);

    std::size_t max_param_len = std::max(param1_name.length(), param2_name.length());
    std::ostringstream expected;
    expected << std::string(indent_width, ' ') << arg_name << " : " << help_msg;

    expected
        << "\n"
        << std::string(indent_width * 2, ' ') << "- " << std::setw(static_cast<int>(max_param_len))
        << std::left << param1_name << " = " << param1_value;

    expected
        << "\n"
        << std::string(indent_width * 2, ' ') << "- " << std::setw(static_cast<int>(max_param_len))
        << std::left << param2_name << " = " << param2_value;

    CHECK_EQ(sut.get(indent_width, max_line_width), expected.str());
}

TEST_SUITE_END(); // test_help_builder
