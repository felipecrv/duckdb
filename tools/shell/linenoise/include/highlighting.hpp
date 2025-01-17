//===----------------------------------------------------------------------===//
//                         DuckDB
//
// highlighting.hpp
//
//
//===----------------------------------------------------------------------===//

#pragma once

#include "duckdb/common/common.hpp"

namespace duckdb {
struct searchMatch;

enum class tokenType : uint8_t {
	TOKEN_IDENTIFIER,
	TOKEN_NUMERIC_CONSTANT,
	TOKEN_STRING_CONSTANT,
	TOKEN_OPERATOR,
	TOKEN_KEYWORD,
	TOKEN_COMMENT,
	TOKEN_CONTINUATION,
	TOKEN_CONTINUATION_SELECTED
};

struct highlightToken {
	tokenType type;
	size_t start = 0;
	bool search_match = false;
};

class Highlighting {
public:
	static void Enable();
	static void Disable();
	static bool IsEnabled();
	static const char *GetColorOption(const char *option);
	static void SetKeyword(const char *color);
	static void SetConstant(const char *color);

	static vector<highlightToken> Tokenize(char *buf, size_t len, searchMatch *match = nullptr);
	static string HighlightText(char *buf, size_t len, size_t start_pos, size_t end_pos,
	                            const vector<highlightToken> &tokens);
};

} // namespace duckdb
