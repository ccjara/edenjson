#pragma once

#include "json_value.hxx"

namespace edenjson {
    struct parse_error {
        std::string message;
        int line = 0;

        explicit operator bool() const { return message.empty(); }
    };

    struct parse_result {
        json_value document;
        parse_error error;


        static parse_result success(json_value&& document) {
            parse_result result;
            result.document = std::move(document);
            return result;
        }

        static parse_result failure(std::string_view error_message, int line) {
            parse_result result;
            result.error = {std::string(error_message), line};
            return result;
        }

        explicit operator bool() const { return !!error; }
    };
} // namespace edenjson
