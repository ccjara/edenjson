#include "edenjson.hxx"
#include "json_parser.hxx"

namespace edenjson {
    constinit const json_value json_value::json_null;
    const json_value json_value::json_empty_object = json_value(json_object{});
    const json_value json_value::json_empty_array = json_value(json_array{});

	parse_result parse(std::string_view json) {
		json_parser parser;
		return parser.parse(json);
	}
} // namespace edenjson
