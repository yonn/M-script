#include "eval.hpp"

namespace mscript {
	
	Object* read(const std::string& program)
	{
		finished = false;
		tokens = tokenize(program);
		if (tokens.empty()) { 
			return nullptr;
		}
		return read_impl(fetch());
	}

	Object* read_impl(const std::string& next)
	{
		if (next == "(") {
			return read_list();
		} else if (next == "[") {
			return read_array();
		} else {
			return read_atom(next);
		}
	}
	
	Object* read_array()
	{
		auto arr = (Array*)new_object<Array>(std::vector<Object*>());
		while (not finished) {
			auto t = fetch();
			if (t == "]") {
				return arr;
			} else {
				arr->array.push_back(read_impl(t));
			}
		}
		error("Incomplete input!");
		return nullptr;
	}

	Object* read_list()
	{
		ConsCell* res = (ConsCell*)MSCRIPT_NEW_NIL;
		while (not finished) {
			auto t = fetch();
			if (t == ")") {
				return res;
			} else {
				push_back(res, read_impl(t));
			}
		}
		error("Incomplete input!");
		return nullptr;
	}

	Object* read_atom(const std::string& token)
	{
		if (token == "nil") {
			return MSCRIPT_NEW_NIL;
		} else if (token == "true") { 
			return new_object<Bool>(true);
		} else if (token == "false") {
			return new_object<Bool>(false);
		}

		try {
			return new_object<Number>(std::stold(token));
		} catch (std::invalid_argument e) { }

		if (token[0] == '"' and token[token.length() - 1] == '"') {
			return new_object<String>(token);
		}

		return new_object<Symbol>(token);
	}
	
	std::string fetch()
	{
		if (not tokens.empty()) { 
			auto s = tokens.front();
			tokens.pop_front();
			return s;
		}
		finished = true;
		return "";
	}

	Object* eval(Object* s_expression)
	{
		if (s_expression->type != "ConsCell") {
			if (s_expression->type == "Symbol") {
				std::string name = ((Symbol*)s_expression)->name;
				if (local::exists(name)) {
					auto tmp =  local::variable_get(name);
					return tmp;
				} else if (global::exists(name)) {
					auto tmp = global::variable_get(name);
					return tmp;
				}
			}
			return s_expression;
		} else {
			return eval_list(s_expression);
		}
	}

	Object* eval_list(Object* list)
	{
		ConsCell* l = (ConsCell*)list;
		if (l->is_empty()) {
			return l;
		}
		auto f = get_arg(l, "Object");
		return function_call(f, l);
	}
}
