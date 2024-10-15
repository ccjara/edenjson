#include "json_parser.hxx"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("array elements can be iterated", "[json_value][unit]") {
    const auto result = edenjson::json_parser().parse("[1, 3, 2]");

    REQUIRE(!!result);
    const auto& doc = result.document;
    REQUIRE(doc.is_array());

    std::vector<double> expected = {1, 3, 2};
    std::vector<double> actual = {};

    for (const auto& element : doc.each_element()) {
        REQUIRE(element.is_number());
        REQUIRE(element.as_number().has_value());
        actual.push_back(element.as_number().value());
    }

    REQUIRE(expected == actual);
}

TEST_CASE("empty array iteration is skipped", "[json_value][unit]") {
    const auto result = edenjson::json_parser().parse("[]");

    REQUIRE(!!result);
    const auto& doc = result.document;
    REQUIRE(doc.is_array());

    for (const auto& _ : doc.each_element()) {
        FAIL();
    }
}

TEST_CASE("array element iteration on non-array is skipped", "[json_value][unit]") {
    const auto result = edenjson::json_parser().parse("{ \"0\": 1 }");

    REQUIRE(!!result);
    const auto& doc = result.document;
    REQUIRE(doc.is_object());

    for (const auto& _ : doc.each_element()) {
        FAIL();
    }
}

TEST_CASE("object properties can be iterated", "[json_value][unit]") {
    const auto result = edenjson::json_parser().parse(R"({ "first" : 1, "second" : 2, "third" : 3 })");

    REQUIRE(!!result);
    const auto& doc = result.document;
    REQUIRE(doc.is_object());

    const std::map<std::string, double> expected = {
        {"first", 1},
        {"second", 2},
        {"third", 3}
    };

    std::map<std::string, double> actual = {};

    for (const auto& [key, value] : doc.each_property()) {
        REQUIRE(value.is_number());
        REQUIRE(value.as_number().has_value());
        actual[key] = value.as_number().value();
    }

    REQUIRE(expected == actual);
}

TEST_CASE("empty object iteration is skipped", "[json_value][unit]") {
    const auto result = edenjson::json_parser().parse("{}");

    REQUIRE(!!result);
    const auto& doc = result.document;
    REQUIRE(doc.is_object());

    for (const auto& _ : doc.each_property()) {
        FAIL();
    }
}

TEST_CASE("property iteration on non object is skipped", "[json_value][unit]") {
    const auto result = edenjson::json_parser().parse("[]");

    REQUIRE(!!result);
    const auto& doc = result.document;
    REQUIRE(doc.is_array());

    for (const auto& _ : doc.each_property()) {
        FAIL();
    }
}
