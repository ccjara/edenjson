#include "json_parser.hxx"
#include "json_samples.hxx"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("empty string is not valid json", "[json_parser][unit]") {
    REQUIRE(!edenjson::json_parser().parse(""));
}

TEST_CASE("parses empty objects", "[json_parser][unit]") {
    auto parser = edenjson::json_parser();
    const auto result = parser.parse("{}");

    REQUIRE(!!result);
    const auto& doc = result.document;
    REQUIRE(doc.is_object());
    const auto& obj = doc.as_object().value();
    REQUIRE(obj.empty());
}

TEST_CASE("parses simple JSON objects", "[json_parser][unit]") {
    auto parser = edenjson::json_parser();
    const auto result = parser.parse(R"({"key": "value"})");

    REQUIRE(!!result);

    const auto& doc = result.document;

    REQUIRE(doc.is_object());
    REQUIRE(!doc.as_object().value().empty());
    REQUIRE(doc["key"].is_string());
    REQUIRE(doc["key"].as_string().has_value());
    REQUIRE(doc["key"].as_string().value() == "value");
}

TEST_CASE("parses boolean", "[json_parser][unit]") {
    auto parser = edenjson::json_parser();

    SECTION("boolean false root") {
        const auto result = parser.parse("false");
        REQUIRE(!result.document.as_boolean().value());
    }

    SECTION("boolean true root") {
        const auto result = parser.parse("true");
        REQUIRE(result.document.as_boolean().value());
    }

    SECTION("boolean true property") {
        const auto result = parser.parse(R"({"boolean": true})");
        REQUIRE(result.document["boolean"].as_boolean().value());
    }

    SECTION("boolean false property") {
        const auto result = parser.parse(R"({"boolean": false})");
        REQUIRE(!result.document["boolean"].as_boolean().value());
    }
}

TEST_CASE("parses strings", "[json_parser][unit]") {
    auto parser = edenjson::json_parser();

    SECTION("simple string root") {
        REQUIRE(parser.parse("\"ergonomics\"").document.as_string().value() == "ergonomics");
    }

    SECTION("empty string root") {
        REQUIRE(parser.parse("\"\"").document.as_string().value().empty());
    }

    SECTION("empty property strings") {
        REQUIRE(parser.parse(R"({"key": ""})").document["key"].as_string().value().empty());
    }

    SECTION("escape sequences") {
        SECTION("escaped strings") {
            const auto result = parser.parse(R"({"key": "My \"string\" of luck is not yet over"})");

            REQUIRE(result.document["key"].as_string().value() == "My \"string\" of luck is not yet over");
        }

        SECTION("escaped strings at the end") {
            const auto result = parser.parse(R"({"key": "\"string\""})");

            REQUIRE(result.document["key"].as_string().value() == "\"string\"");
        }

        SECTION("\\u sequences") {
            SECTION("ASCII") {
                const auto result = parser.parse(R"({"key": "\u0041"})");
                REQUIRE(result.document["key"].as_string().value() == "A");
            }

            SECTION("UTF-8 <= 0x7FF") {
                const auto result = parser.parse(R"({"key": "\u00E9"})");
                REQUIRE(result.document["key"].as_string().value() == "é");
            }

            SECTION("UTF-8 <= 0xFFFF") {
                const auto result = parser.parse(R"({"key": "\u4E2D"})");
                REQUIRE(result.document["key"].as_string().value() == "中");
            }

            SECTION("UTF-8 surrogate pair") {
                const auto result = parser.parse(R"({"key": "\uD83D\uDE00"})");
                REQUIRE(result.document["key"].as_string().value() == "😀");
            }
        }
    }
}

TEST_CASE("parses numbers", "[json_parser][unit]") {
    auto parser = edenjson::json_parser();

    SECTION("number with trailing white space") {
        REQUIRE(parser.parse(" 42.67    ").document.as_number().value() == 42.67);
    }

    SECTION("non decimal root") {
        REQUIRE(parser.parse("42").document.as_number().value() == 42.0);
    }

    SECTION("decimal root") {
        REQUIRE(parser.parse("42.42").document.as_number().value() == 42.42);
    }

    SECTION("scientific notation number root") {
        REQUIRE(parser.parse("42e3").document.as_number().value() == 42000.0);
    }

    SECTION("negative number root") {
        REQUIRE(parser.parse("-42").document.as_number().value() == -42.0);
    }

    SECTION("negative decimal number root") {
        REQUIRE(parser.parse("-42.42").document.as_number().value() == -42.42);
    }

    SECTION("negative scientific notation number root") {
        REQUIRE(parser.parse("-42e3").document.as_number().value() == -42000.0);
    }

    SECTION("negative scientific notation with negative exponent number root") {
        REQUIRE(parser.parse("-42e-3").document.as_number().value() == -0.042);
    }

    SECTION("negative scientific notation number property") {
        REQUIRE(parser.parse(R"({"key": -42e-3})").document["key"].as_number().value() == -0.042);
    }
}

TEST_CASE("parses complex documents", "[json_parser][integration]") {
    auto parser = edenjson::json_parser();

    SECTION("creature template") {
        const auto result = parser.parse(json_samples::creature_template);

        REQUIRE(result);
        REQUIRE(result.document["components"][3]["root"]["child"]["children"][0]["type"].as_string().value() == "ClosestEntity");
    }

    SECTION("app state") {
        const auto result = parser.parse(json_samples::app_state);

        REQUIRE(result);
        REQUIRE(result.document["settings"]["languages"][3]["code"].as_string().value() == "de");
    }
}

TEST_CASE("provides ergonomic and failsafe access", "[json_parser][unit]") {
    auto parser = edenjson::json_parser();

    SECTION("out of bounds array access") {
        const auto result = parser.parse(R"({"array_of_objects": [{ "key": "value" }]})");
        REQUIRE(result);
        REQUIRE(!result.document["array_of_objects"][0].is_null());
        REQUIRE(result.document["array_of_objects"][1].is_null());
    }

    SECTION("arbitrary access on empty document") {
        const auto result = parser.parse("null");
        REQUIRE(result);
        REQUIRE(result.document.is_null());
        REQUIRE(result.document["a"][0]["b"][1].is_null());
    }
}

TEST_CASE("Communicates parsing error", "[json_parser][unit]") {
    auto parser = edenjson::json_parser();

    SECTION("unterminated string") {
        const auto result = parser.parse("\"str");
        REQUIRE(result.error.message == edenjson::error::unterminated_string);
        REQUIRE(result.error.line == 1);
    }

    SECTION("unterminated string over multiple lines") {
        const auto result = parser.parse("\"a\nb\"");
        REQUIRE(result.error.message == edenjson::error::unterminated_string);
        REQUIRE(result.error.line == 1);
    }

    SECTION("invalid escape sequence") {
        const auto result = parser.parse(R"("\M")");
        REQUIRE(result.error.message == edenjson::error::invalid_escape);
        REQUIRE(result.error.line == 1);
    }
}
