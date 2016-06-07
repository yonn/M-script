#ifndef MSCRIPT_BUILTINS_HPP
#define MSCRIPT_BUILTINS_HPP

#include <string>
#include <map>
#include <cmath>
#include <iostream>
#include <deque>
#include <algorithm>

#include "object.hpp"
#include "eval.hpp"
#include "error.hpp"

namespace mscript {

#define MSCRIPT_ADD_BUILTIN(s, f) (builtin_functions[s] = &f)

	namespace global {

		static std::map<std::string, Object*> variables;

		bool exists(const std::string& varname);
		
		std::map<std::string, Object*>* get_variables();
		void variable_set(const std::string& name, Object* value);
		Object* variable_get(const std::string& name);

	}
	
	namespace local {
	
		static::std::deque<std::map<std::string, Object*>> scopes;
		
		bool exists(const std::string& varname);
		
		void add_scope(std::map<std::string, Object*> vars);
		void remove_scope();
		
		void variable_set(const std::string& name, Object* value);
		Object* variable_get(const std::string& name);
		
	}
	
	void variable_set(const std::string& name, Object* value);
	Object* variable_get(const std::string& name);

	typedef Object* (*Function)(ConsCell*);

	static std::map<std::string, Function> builtin_functions;

	Function get_function(const std::string& name);

	void init_builtins();

	Object* type_check(Object* o, const std::string& type);
	Object* get_arg(ConsCell*& c, const std::string& type);
	Object* get_raw_arg(ConsCell*& c, const std::string& type);
	
	Object* call(Func* f, ConsCell* args);
	Object* function_call(Object* f, ConsCell* args);

	/*---------------------------------------------------------------------
	 *                         **Builtin Functions**                      |
	 *---------------------------------------------------------------------
	 */

	Object* defvar(ConsCell* args);
	Object* set(ConsCell* args);
	
	Object* if_(ConsCell* args);
	Object* let(ConsCell* args);
	Object* lambda(ConsCell* args);
	Object* fn(ConsCell* args);
	Object* while_(ConsCell* args);
	Object* cond(ConsCell* args);
	Object* foreach(ConsCell* args);
	Object* map(ConsCell* args);
	Object* elt(ConsCell* args);
	Object* set_elt(ConsCell* args);
	Object* range(ConsCell* args);
	Object* do_(ConsCell* args);
	Object* string_concat(ConsCell* args);
	Object* len(ConsCell* args);
	Object* array_push_back(ConsCell* args);
	Object* array_pop_back(ConsCell* args);
	Object* array_reversed(ConsCell* args);
	Object* array_sorted(ConsCell* args);
	
	std::map<std::string, Object*> bind_args(ConsCell* args);
	
	Object* equals(ConsCell* args);
	Object* not_(ConsCell* args);
	Object* or_(ConsCell* args);
	Object* and_(ConsCell* args);
	Object* less_than(ConsCell* args);
	Object* greater_than(ConsCell* args);
	Object* less_than_equals(ConsCell* args);
	Object* greater_than_equals(ConsCell* args);

	/*---------------------------------------------------------------------
	 *                       Mathematical Functions                       |
	 *---------------------------------------------------------------------
	 */

	Object* add(ConsCell* args);
	Object* subtract(ConsCell* args);
	Object* multiply(ConsCell* args);
	Object* divide(ConsCell* args);
	Object* power(ConsCell* args);
	Object* sqrt(ConsCell* args);
	Object* rem(ConsCell* args);
	Object* sin(ConsCell* args);
	Object* cos(ConsCell* args);
	Object* tan(ConsCell* args);
	Object* asin(ConsCell* args);
	Object* acos(ConsCell* args);
	Object* atan(ConsCell* args);

	/*---------------------------------------------------------------------
	 *                             IO Functions                           |
	 *---------------------------------------------------------------------
	 */

	Object* printf(ConsCell* args);
	Object* printfln(ConsCell* args);
	Object* input(ConsCell* args);

	Object* s_to_n(ConsCell* args);
	Object* shell_command(ConsCell* args);
	
	std::string format__(ConsCell* args);

}

#endif /*MSCRIPT_BUILTINS_HPP*/
