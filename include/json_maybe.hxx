#pragma once

#include <cstdlib>
#include <utility>

namespace edenjson {

    template <typename T>
    class json_maybe {
    public:
        constexpr json_maybe() = default;
        explicit constexpr json_maybe(T& ref) : ref_(&ref) {}

        [[nodiscard]] constexpr bool has_value() const { return ref_ != nullptr; }

        constexpr T& value() const {
            if (!ref_) {
                std::abort();
            }
            return *ref_;
        }

        constexpr const T& value_or(const T& default_value) const {
            if (has_value()) {
                return *ref_;
            }
            return default_value;
        }

        constexpr T value_or(T&& default_value) const {
            if (has_value()) {
                return *ref_;
            }
            return std::move(default_value);
        }

        explicit constexpr operator bool() const { return has_value(); }

        constexpr T& operator*() const { return value(); }
        constexpr T* operator->() const { return &value(); }

    private:
        T* ref_ = nullptr;
    };

} // namespace edenjson
