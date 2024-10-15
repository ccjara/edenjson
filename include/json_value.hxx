#pragma once

#include <map>
#include <string>
#include <variant>
#include <vector>

#include "json_maybe.hxx"

namespace edenjson {
    class json_value;

    enum class json_type { object, array, string, number, boolean, null };

    using json_object = std::map<std::string, json_value>;
    using json_array = std::vector<json_value>;
    using json_variant = std::variant<std::nullptr_t, bool, double, std::string, json_object, json_array>;

    class json_value {
    public:
        constinit static const json_value json_null;
        static const json_value json_empty_object;
        static const json_value json_empty_array;


        constexpr explicit json_value() : type_(json_type::null), value_(nullptr) {}
        constexpr explicit json_value(bool value) : type_(json_type::boolean), value_(value) {}
        constexpr explicit json_value(double value) : type_(json_type::number), value_(value) {}
        constexpr explicit json_value(const std::string& value) : type_(json_type::string), value_(value) {}
        constexpr explicit json_value(std::string&& value) : type_(json_type::string), value_(std::move(value)) {}
        constexpr explicit json_value(const json_object& value) : type_(json_type::object), value_(value) {}
        constexpr explicit json_value(json_object&& value) : type_(json_type::object), value_(std::move(value)) {}
        constexpr explicit json_value(const json_array& value) : type_(json_type::array), value_(value) {}
        constexpr explicit json_value(json_array&& value) : type_(json_type::array), value_(std::move(value)) {}

        [[nodiscard]] constexpr json_type type() const { return type_; }

        [[nodiscard]] constexpr bool is_object() const { return std::holds_alternative<json_object>(value_); }
        [[nodiscard]] constexpr bool is_array() const { return std::holds_alternative<json_array>(value_); }
        [[nodiscard]] constexpr bool is_string() const { return std::holds_alternative<std::string>(value_); }
        [[nodiscard]] constexpr bool is_number() const { return std::holds_alternative<double>(value_); }
        [[nodiscard]] constexpr bool is_boolean() const { return std::holds_alternative<bool>(value_); }
        [[nodiscard]] constexpr bool is_null() const { return std::holds_alternative<std::nullptr_t>(value_); }

        [[nodiscard]] constexpr json_maybe<const std::string> as_string() const {
            if (is_string()) {
                return json_maybe(std::get<std::string>(value_));
            }
            return {};
        }

        [[nodiscard]] constexpr json_maybe<const double> as_number() const {
            if (is_number()) {
                return json_maybe(std::get<double>(value_));
            }
            return {};
        }

        [[nodiscard]] constexpr json_maybe<const bool> as_boolean() const {
            if (is_boolean()) {
                return json_maybe(std::get<bool>(value_));
            }
            return {};
        }

        [[nodiscard]] constexpr json_maybe<const json_object> as_object() const {
            if (is_object()) {
                return json_maybe(std::get<json_object>(value_));
            }
            return {};
        }

        [[nodiscard]] constexpr json_maybe<const json_array> as_array() const {
            if (is_array()) {
                return json_maybe(std::get<json_array>(value_));
            }
            return {};
        }

        constexpr const json_value& operator[](const std::string& key) const {
            if (is_object()) {
                const auto& obj = std::get<json_object>(value_);
                if (const auto it = obj.find(key); it != obj.cend()) {
                    return it->second;
                }
            }
            return json_null;
        }

        constexpr const json_value& operator[](size_t index) const {
            if (is_array()) {
                if (const auto& arr = std::get<json_array>(value_); index < arr.size()) {
                    return arr[index];
                }
            }
            return json_null;
        }

        [[nodiscard]] constexpr auto each_element() const {
            struct iterable {
                const json_array& arr;
                [[nodiscard]] constexpr auto begin() const { return arr.begin(); }
                [[nodiscard]] constexpr auto end() const { return arr.end(); }
            };

            if (is_array()) {
                return iterable{std::get<json_array>(value_)};
            }
            return iterable{std::get<json_array>(json_empty_array.value_)};
        }

        [[nodiscard]] auto each_property() const {
            struct iterable {
                const json_object& obj;
                [[nodiscard]] auto begin() const { return obj.begin(); }
                [[nodiscard]] auto end() const { return obj.end(); }
            };

            if (is_object()) {
                return iterable{std::get<json_object>(value_)};
            }
            return iterable{std::get<json_object>(json_empty_object.value_)};
        }
    private:
        json_type type_;
        json_variant value_;
    };
} // namespace edenjson
