#ifndef MSCRIPT_EVAL_HPP
#define MSCRIPT_EVAL_HPP

#include <string>
#include <map>

#include "object.hpp"
#include "builtins.hpp"
#include "lexer.hpp"

namespace mscript {

	static bool finished = false;
	static TokenList tokens;

	Object* read(const std::string& program);
	Object* eval(Object* s_expression);

	Object* read_impl(const std::string& next);
	Object* read_list();
	Object* read_array();
	Object* read_atom(const std::string& token);

	Object* eval_list(Object* list);

	std::string fetch();
}	

#endif /*MSCRIPT_EVAL_HPP*/
