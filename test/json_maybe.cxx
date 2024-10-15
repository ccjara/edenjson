#include "json_maybe.hxx"
#include <catch2/catch_test_macros.hpp>

struct complex {
    explicit complex(int value) : value(value) {}

    int value = 0;
};

struct uncopyable {
    explicit uncopyable(int value) : value(value) {}
    uncopyable(const uncopyable&) = delete;
    uncopyable& operator=(const uncopyable&) = delete;
    uncopyable(uncopyable&&) = default;
    uncopyable& operator=(uncopyable&&) = default;

    int value = 0;
};

TEST_CASE("value_or(const T&) returns default when no value is set", "[json_maybe][unit]") {
    const auto maybe_int = edenjson::json_maybe<int>{};
    REQUIRE(maybe_int.value_or(42) == 42);
}

TEST_CASE("value_or(const T&) returns stored value when set", "[json_maybe][unit]") {
    const int int_value = 16;
    const auto maybe_int = edenjson::json_maybe{int_value};
    REQUIRE(maybe_int.value_or(42) == 16);
}

TEST_CASE("value_or(T&&) returns default value when no value is set", "[json_maybe][unit]") {
    const auto maybe_complex = edenjson::json_maybe<complex>{};
    REQUIRE(maybe_complex.value_or(complex{42}).value == 42);
}

TEST_CASE("value_or(T&&) returns stored value when set", "[json_maybe][unit]") {
    const complex complex_instance{16};
    const auto maybe_complex = edenjson::json_maybe{complex_instance};
    REQUIRE(maybe_complex.value_or(complex{42}).value == 16);
}

TEST_CASE("value_or(T&&) does not copy the contained value", "[json_maybe][unit]") {
    const uncopyable uncopyable_instance{16};
    const auto maybe_complex = edenjson::json_maybe{uncopyable_instance};
    REQUIRE(maybe_complex.value_or(uncopyable{42}).value == 16);
}

TEST_CASE("value_or(T&&) does not the default value", "[json_maybe][unit]") {
    const auto maybe_complex = edenjson::json_maybe<uncopyable>{};
    REQUIRE(maybe_complex.value_or(uncopyable{42}).value == 42);
}
