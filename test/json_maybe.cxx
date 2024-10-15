#include "json_maybe.hxx"
#include <catch2/catch_test_macros.hpp>

struct complex {
    explicit complex(int value) : value(value) {}

    int value = 0;
};

TEST_CASE("value_or(const T&) returns default when no value is set", "[json_maybe][unit]") {
    const auto maybe_int = edenjson::json_maybe<int>{};
    REQUIRE(maybe_int.value_or(42) == 42);
}

TEST_CASE("value_or(const T&) returns stored value when set", "[json_maybe][unit]") {
    const int& int_ref = 16;
    const auto maybe_int = edenjson::json_maybe{int_ref};
    REQUIRE(maybe_int.value_or(42) == 16);
}

TEST_CASE("value_or(T&&) returns default value when no value is set", "[json_maybe][unit]") {
    const auto maybe_complex = edenjson::json_maybe<complex>{};
    REQUIRE(maybe_complex.value_or(complex{42}).value == 42);
}

TEST_CASE("value_or(T&&) returns stored value when set", "[json_maybe][unit]") {
    const complex& complex_ref = complex{16};
    const auto maybe_complex = edenjson::json_maybe{complex_ref};
    REQUIRE(maybe_complex.value_or(complex{42}).value == 16);
}
