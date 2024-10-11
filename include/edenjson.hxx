#pragma once

#include "parse_result.hxx"

namespace edenjson {
	/**
	 * \brief Parses a JSON document from the given string
	 *
	 * \param json The JSON document to parse
	 *
	 * \return A parse_result object containing the parsed JSON document or an error message
	 */
	[[nodiscard]] parse_result parse(std::string_view json);
} // namespace edenjson
