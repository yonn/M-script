#include "lexer.hpp"

namespace mscript {
	
	TokenList tokenize(const std::string& program)
	{
		TokenList tokens;
		std::string buffer;
		bool comment = false;
		bool quote = false;
		for (auto c : program) {
			if (c == '"' and not comment) {
				quote = !quote;
			}
			if (comment and c == '\n') {
				comment = false;
			}
			if (c == ';' and not comment and not quote) { 
				comment = true;
			} else if (is_whitespace(c) and not comment and not quote) {
				tokens.push_back(buffer);
				buffer = "";
			} else if (is_symbol(c) and not comment and not quote) {
				tokens.push_back(buffer);
				tokens.push_back(std::string(1, c));
				buffer = "";
			} else if (not comment) {
				buffer += c;
			}
		}

		tokens.push_back(buffer);

		tokens.remove("");	

		return tokens;
	}

	bool is_whitespace(char c)
	{
		return c == ' '  or
		       c == '\t' or
		       c == '\n';
	}

	bool is_symbol(char c)
	{
		return c == '(' or
		       c == ')' or
		       c == '[' or
		       c == ']';
	}
}
