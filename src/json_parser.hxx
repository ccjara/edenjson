#pragma once

#include "json_value.hxx"
#include "parse_result.hxx"

namespace edenjson {
    namespace error {
        constexpr const char* unterminated_string = "Unterminated string";
        constexpr const char* unexpected_character = "Unexpected character";
        constexpr const char* open_brace_expected = "'{' expected";
        constexpr const char* open_bracket_expected = "'[' expected";
        constexpr const char* closing_bracket_expected = "']' expected";
        constexpr const char* double_quote_expected = "'\"' expected";
        constexpr const char* colon_expected = "':' expected";
        constexpr const char* comma_expected = "',' expected";
        constexpr const char* trailing_comma = "Trailing comma";
        constexpr const char* invalid_number = "Invalid number";
        constexpr const char* invalid_escape = "Invalid escape sequence";
        constexpr const char* invalid_unicode_escape = "Invalid unicode escape sequence";
    } // namespace error

    [[nodiscard]] static bool try_parse_double(const std::string& str, double& result) {
        char* end;
        errno = 0;
        const auto value = std::strtod(str.c_str(), &end);

        if (errno == ERANGE) {
            return false;
        }
        if (*end != '\0') {
            return false;
        }

        result = value;
        return true;
    }

    [[nodiscard]] static bool try_parse_ulong(const std::string& hex, int base, unsigned long int& result) {
        char* end;
        errno = 0;
        const auto codepoint = std::strtoul(hex.c_str(), &end, base);

        if (errno == ERANGE) {
            return false;
        }
        if (*end != '\0') {
            return false;
        }

        result = codepoint;
        return true;
    }

    class json_parser {
    public:
        json_parser() = default;

        /**
         * \brief Parses a JSON document from the given string
         */
        parse_result parse(std::string_view json) {
            json_ = json;
            error_ = "";
            skip_whitespace();

            if (json_value root; parse_value(root) == 0) {
                return parse_result::success(std::move(root));
            }
            return parse_result::failure(error_, line_number_);
        }

    private:
        std::string_view json_;
        std::string error_;
        int line_number_ = 1;

        void skip_whitespace() {
            while (!json_.empty() && std::isspace(json_.front())) {
                if (json_.front() == '\n') {
                    ++line_number_;
                }
                json_.remove_prefix(1);
            }
        }

        [[nodiscard]] char peek() const {
            if (json_.empty()) {
                return '\0';
            }
            return json_.front();
        }

        char consume() {
            if (json_.empty()) {
                return '\0';
            }
            const char c = json_.front();
            json_.remove_prefix(1);

            return c;
        }

        bool try_consume(char expected) {
            skip_whitespace();
            if (!json_.empty() && json_.front() == expected) {
                json_.remove_prefix(1);
                return true;
            }
            return false;
        }

        int parse_value(json_value& result) {
            skip_whitespace();
            const char c = peek();

            if (c == '{') {
                return parse_object(result);
            }
            if (c == '[') {
                return parse_array(result);
            }
            if (c == '"') {
                return parse_string(result);
            }
            if (std::isdigit(c) || c == '-') {
                return parse_number(result);
            }
            if (c == 't' || c == 'f') {
                return parse_boolean(result);
            }
            if (c == 'n') {
                return parse_null(result);
            }
            error_ = error::unexpected_character;
            return -1;
        }

        int parse_object(json_value& result) {
            if (!try_consume('{')) {
                error_ = error::open_brace_expected;
                return -1;
            }

            json_object object;

            if (try_consume('}')) {
                result = json_value(std::move(object));
                return 0;
            }

            while (true) {
                json_value key, value;

                if (parse_string(key) != 0) {
                    return -1;
                }

                if (!try_consume(':')) {
                    error_ = error::colon_expected;
                    return -1;
                }

                if (parse_value(value) != 0) {
                    return -1;
                }

                object[key.as_string().value()] = value;

                if (try_consume('}')) {
                    result = json_value(std::move(object));
                    return 0;
                }

                if (!try_consume(',')) {
                    error_ = error::comma_expected;
                    return -1;
                }

                if (try_consume('}')) {
                    error_ = error::trailing_comma;
                    return -1;
                }
            }
        }

        int parse_array(json_value& result) {
            if (!try_consume('[')) {
                error_ = error::open_bracket_expected;
                return -1;
            }

            skip_whitespace();

            json_array array;

            if (try_consume(']')) {
                result = json_value(std::move(array));
                return 0;
            }

            while (true) {
                json_value element;

                if (parse_value(element) != 0) {
                    return -1;
                }

                array.push_back(element);

                if (try_consume(']')) {
                    result = json_value(std::move(array));
                    return 0;
                }

                if (!try_consume(',')) {
                    error_ = error::comma_expected;
                    return -1;
                }
            }
        }

        int parse_string(json_value& result) {
            if (!try_consume('"')) {
                error_ = error::double_quote_expected;
                return -1;
            }

            std::string buffer;
            while (true) {
                char c = consume();
                if (c == '"') {
                    break;
                }

                if (c == '\0') {
                    error_ = error::unterminated_string;
                    return -1;
                }

                if (c == '\n') {
                    error_ = error::unterminated_string;
                    return -1;
                }

                if (c == '\\') {
                    c = consume();
                    switch (c) {
                    case '"':
                        buffer += '"';
                        break;
                    case '\\':
                        buffer += '\\';
                        break;
                    case '/':
                        buffer += '/';
                        break;
                    case 'b':
                        buffer += '\b';
                        break;
                    case 'f':
                        buffer += '\f';
                        break;
                    case 'n':
                        buffer += '\n';
                        break;
                    case 'r':
                        buffer += '\r';
                        break;
                    case 't':
                        buffer += '\t';
                        break;
                    case 'u': {
                        std::string hex;
                        for (int i = 0; i < 4; ++i) {
                            const char hex_digit = consume();
                            if (!isxdigit(hex_digit)) {
                                error_ = error::invalid_unicode_escape;
                                return -1;
                            }
                            hex += hex_digit;
                        }

                        auto code_unit = 0ul;

                        if (!try_parse_ulong(hex, 16, code_unit)) {
                            error_ = error::invalid_unicode_escape;
                            return -1;
                        }

                        if (code_unit >= 0xD800 && code_unit <= 0xDBFF) {
                            if (consume() != '\\' || consume() != 'u') {
                                error_ = error::invalid_unicode_escape;
                                return -1;
                            }

                            std::string low_hex;
                            for (int i = 0; i < 4; ++i) {
                                char hex_digit = consume();
                                if (!isxdigit(hex_digit)) {
                                    error_ = error::invalid_unicode_escape;
                                    return -1;
                                }
                                low_hex += hex_digit;
                            }

                            unsigned long int low_surrogate = 0;

                            if (!try_parse_ulong(low_hex, 16, low_surrogate)) {
                                error_ = error::invalid_unicode_escape;
                                return -1;
                            }

                            if (low_surrogate < 0xDC00 || low_surrogate > 0xDFFF) {
                                error_ = error::invalid_unicode_escape;
                                return -1;
                            }

                            // combine surrogate pair to form the full code point
                            const auto code_point = 0x10000 + ((code_unit - 0xD800) << 10) + (low_surrogate - 0xDC00);

                            if (code_point <= 0x7F) {
                                buffer += static_cast<char>(code_point);
                            }
                            else if (code_point <= 0x7FF) {
                                buffer += static_cast<char>(0xC0 | ((code_point >> 6) & 0x1F));
                                buffer += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            else if (code_point <= 0xFFFF) {
                                buffer += static_cast<char>(0xE0 | ((code_point >> 12) & 0x0F));
                                buffer += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                                buffer += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            else if (code_point <= 0x10FFFF) {
                                buffer += static_cast<char>(0xF0 | ((code_point >> 18) & 0x07));
                                buffer += static_cast<char>(0x80 | ((code_point >> 12) & 0x3F));
                                buffer += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                                buffer += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                        }
                        else {
                            const auto code_point = code_unit;

                            if (code_point <= 0x7F) {
                                buffer += static_cast<char>(code_point);
                            }
                            else if (code_point <= 0x7FF) {
                                buffer += static_cast<char>(0xC0 | ((code_point >> 6) & 0x1F));
                                buffer += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                            else {
                                buffer += static_cast<char>(0xE0 | ((code_point >> 12) & 0x0F));
                                buffer += static_cast<char>(0x80 | ((code_point >> 6) & 0x3F));
                                buffer += static_cast<char>(0x80 | (code_point & 0x3F));
                            }
                        }

                        break;
                    }
                    default:
                        error_ = error::invalid_escape;
                        return -1;
                    }
                }
                else {
                    buffer += c;
                }
            }

            result = json_value(std::move(buffer));
            return 0;
        }

        int parse_number(json_value& result) {
            const std::string_view number_start = json_;

            while (true) {
                const char c = peek();
                if (std::isdigit(c) || c == '-' || c == '.' || c == 'e' || c == 'E') {
                    consume();
                }
                else {
                    break;
                }
            }

            const std::string number_str(number_start.data(), json_.data() - number_start.data());

            double number = 0;

            if (!try_parse_double(number_str, number)) {
                error_ = error::invalid_number;
                return -1;
            }

            result = json_value(number);

            return 0;
        }

        int parse_boolean(json_value& result) {
            if (json_.size() >= 4 && json_.substr(0, 4) == "true") {
                result = json_value(true);
                json_.remove_prefix(4);
                return 0;
            }
            if (json_.size() >= 5 && json_.substr(0, 5) == "false") {
                result = json_value(false);
                json_.remove_prefix(5);
                return 0;
            }
            error_ = error::unexpected_character;
            return -1;
        }

        int parse_null(json_value& result) {
            if (json_.size() >= 4 && json_.substr(0, 4) == "null") {
                result = json_value();
                json_.remove_prefix(4);
                return 0;
            }
            error_ = error::unexpected_character;
            return -1;
        }
    };

} // namespace edenjson
