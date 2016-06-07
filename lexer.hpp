#ifndef MSCRIPT_LEXER_HPP
#define MSCRIPT_LEXER_HPP

#include <list>
#include <string>
#include <algorithm>

namespace mscript {

	typedef std::list<std::string> TokenList;

	TokenList tokenize(const std::string& program);

	bool is_whitespace(char c);
	bool is_symbol(char c);
}

#endif /*MSCRIPT_LEXER_HPP*/
