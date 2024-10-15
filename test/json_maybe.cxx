#include "json_maybe.hxx"
#include <catch2/catch_test_macros.hpp>

TEST_CASE("value_or returns default when no value is set", "[json_maybe][unit]") {
    const auto maybe_int = edenjson::json_maybe<int>{};
    REQUIRE(maybe_int.value_or(42) == 42);
}

TEST_CASE("value_or returns stored value when set", "[json_maybe][unit]") {
    const int& int_ref = 16;
    const auto maybe_int = edenjson::json_maybe{int_ref};
    REQUIRE(maybe_int.value_or(42) == 16);
}
