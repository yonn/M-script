#include "builtins.hpp"

namespace mscript {

	namespace global {
		bool exists(const std::string& varname)
		{
			 return variables.count(varname) == 1;
		}
		
		std::map<std::string, Object*>* get_variables()
		{
			return &variables;
		}

		void variable_set(const std::string& name, Object* value)
		{
			variables[name] = value;
		}

		Object* variable_get(const std::string& name)
		{
			return variables[name];
		}
	}
	
	namespace local {
		
		bool exists(const std::string& varname)
		{
			for (auto scope: scopes) {
				if (scope.size() >= 1) {
					if (scope.count(varname) == 1) return true;
				}
			}
			return false;
		}
		
		void add_scope(std::map<std::string, Object*> vars)
		{
			scopes.push_front(vars);
		}
		
		void remove_scope()
		{
			scopes.pop_front();
		}
		
		void variable_set(const std::string& name, Object* value)
		{
			for (auto& scope: scopes) {
				if (scope.count(name) == 1) {
					scope[name] = value;
					break;
				}
			}
		}
		
		Object* variable_get(const std::string& name)
		{
			for (auto scope: scopes) {
				if (scope.count(name) == 1) {
					return scope[name];
				}
			}
			error("Could not find variable '%s' in local scope.", name.c_str());
			return nullptr;
		}
		
	}
	
	void variable_set(const std::string& name, Object* value)
	{
		if (local::exists(name)) {
			local::variable_set(name, value);
		} else if (global::exists(name)) {
			global::variable_set(name, value);
		}
	}
	
	Object* variable_get(const std::string& name)
	{
		if (local::exists(name)) {
			return local::variable_get(name);
		} else if (global::exists(name)) {
			return global::variable_get(name);
		} else {
			error("Variable \"%s\" could not be found.");
			return MSCRIPT_NEW_NIL;
		}
	}

	Function get_function(const std::string& name)
	{
		if (builtin_functions.count(name) != 1) {
			error("Could not find function \"%s\"", name.c_str());
		}
		return builtin_functions[name];
	}

	void init_builtins()
	{
		MSCRIPT_ADD_BUILTIN("defvar", defvar);
		MSCRIPT_ADD_BUILTIN("set", set);
		
		MSCRIPT_ADD_BUILTIN("if", if_);
		MSCRIPT_ADD_BUILTIN("let", let);
		MSCRIPT_ADD_BUILTIN("lambda", lambda);
		MSCRIPT_ADD_BUILTIN("fn", fn);
		MSCRIPT_ADD_BUILTIN("while", while_);
		MSCRIPT_ADD_BUILTIN("cond", cond);
		MSCRIPT_ADD_BUILTIN("foreach", foreach);
		MSCRIPT_ADD_BUILTIN("map", map);
		MSCRIPT_ADD_BUILTIN("elt", elt);
		MSCRIPT_ADD_BUILTIN("set-elt", set_elt);
		MSCRIPT_ADD_BUILTIN("range", range);
		MSCRIPT_ADD_BUILTIN("do", do_);
		MSCRIPT_ADD_BUILTIN("~", string_concat);
		//MSCRIPT_ADD_BUILTIN("len", len);
		MSCRIPT_ADD_BUILTIN("push", array_push_back);
		MSCRIPT_ADD_BUILTIN("pop", array_pop_back);
		MSCRIPT_ADD_BUILTIN("reversed", array_reversed);
		MSCRIPT_ADD_BUILTIN("sorted", array_sorted);
		
		MSCRIPT_ADD_BUILTIN("=", equals);
		MSCRIPT_ADD_BUILTIN("not", not_);
		MSCRIPT_ADD_BUILTIN("or", or_);
		MSCRIPT_ADD_BUILTIN("and", and_);
		MSCRIPT_ADD_BUILTIN("<", less_than);
		MSCRIPT_ADD_BUILTIN(">", greater_than);
		MSCRIPT_ADD_BUILTIN("<=", less_than_equals);
		MSCRIPT_ADD_BUILTIN(">=", greater_than_equals);

		MSCRIPT_ADD_BUILTIN("+", add);
		MSCRIPT_ADD_BUILTIN("-", subtract);
		MSCRIPT_ADD_BUILTIN("*", multiply);
		MSCRIPT_ADD_BUILTIN("/", divide);
		MSCRIPT_ADD_BUILTIN("^", power);
		MSCRIPT_ADD_BUILTIN("sqrt", sqrt);
		MSCRIPT_ADD_BUILTIN("rem", rem);
		MSCRIPT_ADD_BUILTIN("sin", sin);
		MSCRIPT_ADD_BUILTIN("cos", cos);
		MSCRIPT_ADD_BUILTIN("tan", tan);
		MSCRIPT_ADD_BUILTIN("asin", asin);
		MSCRIPT_ADD_BUILTIN("acos", acos);
		MSCRIPT_ADD_BUILTIN("atan", atan);

		MSCRIPT_ADD_BUILTIN("printf", printf);
		MSCRIPT_ADD_BUILTIN("printfln", printfln);
		MSCRIPT_ADD_BUILTIN("input", input);
		
		MSCRIPT_ADD_BUILTIN("s->n", s_to_n);
		MSCRIPT_ADD_BUILTIN("!", shell_command);
		
		global::variable_set("pi", new_object<Number>(M_PI));
	}
	
	Object* type_check(Object* o, const std::string& type)
	{
		if (type == "Object") {
			return o;
		}

		if (o->type != type) {
			error("Type Error!\nRecieved \"%s\", when expecting \"%s\".", o->type.c_str(), type.c_str());
		}

		return o;
	}

	Object* get_arg(ConsCell*& c, const std::string& type)
	{
		if (c->is_empty()) {
			error("Attempted to get argument from an empty list");
		}
		return type_check(eval(pop_front(c)), type);
	}
	
	Object* get_raw_arg(ConsCell*& c, const std::string& type)
	{
		if (c->is_empty()) {
			error("Attempted to get argument from an empty list");
		}
		return type_check(pop_front(c), type);
	}
	
	Object* call(Func* f, ConsCell* args)
	{
		std::map<std::string, Object*> vars;
		size_t i = 0;
		while (not args->is_empty() and i < f->varnames.size()) {
			vars[f->varnames[i]] = get_arg(args, "Object")->copy();
			i++;
		}
		local::add_scope(vars);
		auto body = (ConsCell*)f->body->copy();
		Object* res;
		MSCRIPT_CONS_FOREACH(expr, body) {
			res = eval(expr->car);
		}
		local::remove_scope();
		return res;
	}
	
	Object* function_call(Object* f, ConsCell* args)
	{
		if (f->type == "Symbol") {
			auto name = ((Symbol*)f)->name;
			Function fun = get_function(name);
			if (not args->is_empty()) {
				return fun(args);
			} else {
				return fun(MSCRIPT_NEW_NIL);
			}
		} else if (f->type == "Func") {
			if (not args->is_empty()) {
				return call((Func*)f, args);
			} else {
				return call((Func*)f, MSCRIPT_NEW_NIL);
			}
		} else {
			error("Expected Func or Symbol instead of '%s'", f->type.c_str());
			return nullptr;
		}
	}

	/*---------------------------------------------------------------------
	 *                         **Builtin Functions**                      |
	 *---------------------------------------------------------------------
	 */

	Object* defvar(ConsCell* args)
	{
		Symbol* var = (Symbol*)get_raw_arg(args, "Symbol");
		if (global::exists(var->name)) {
			error("Variable \"%s\" already defined", var->name.c_str());
		}

		Object* res = get_arg(args, "Object");
		global::variable_set(var->name, res);
		return res;

	}

	Object* set(ConsCell* args)
	{
		Symbol* var = (Symbol*)get_raw_arg(args, "Symbol");
		Object* res = get_arg(args, "Object");
		variable_set(var->name, res);
		return res;
	}
	
	
	Object* if_(ConsCell* args)
	{
		bool condition = (get_arg(args, "Object"))->logical();
		if (condition) {
			return get_arg(args, "Object");
		} else {
			get_raw_arg(args, "Object");
			if (not args->is_empty()) {
				return get_arg(args, "Object");
			} else {
				return MSCRIPT_NEW_NIL;
			}
		}
	}
	
	Object* let(ConsCell* args)
	{
		auto varlist = (ConsCell*)get_raw_arg(args, "ConsCell");
		auto vars = bind_args(varlist);
		Object* res = MSCRIPT_NEW_NIL;
		local::add_scope(vars);
		MSCRIPT_CONS_FOREACH(e, args) {
			res = eval(e->car);
		}
		local::remove_scope();
		return res;
	}
	
	Object* lambda(ConsCell* args)
	{
		auto f = (Func*)new_object<Func>();
		f->name = "";
		auto vars = (ConsCell*)get_raw_arg(args, "ConsCell");
		MSCRIPT_CONS_FOREACH(v, vars) {
			f->varnames.push_back(((Symbol*)type_check(v->car, "Symbol"))->name);
		}
		f->body = args;
		return f;
	}
	
	Object* fn(ConsCell* args)
	{
		auto f = (Func*)new_object<Func>();
		f->name = ((Symbol*)get_raw_arg(args, "Symbol"))->name;
		auto vars = (ConsCell*)get_raw_arg(args, "ConsCell");
		MSCRIPT_CONS_FOREACH(v, vars) {
			if (v->car) f->varnames.push_back(((Symbol*)type_check(v->car, "Symbol"))->name);
		}
		f->body = args;
		global::variable_set(f->name, f);
		return f;
	}
	
	Object* while_(ConsCell* args)
	{
		auto pred = get_raw_arg(args, "Object");
		Object* res = MSCRIPT_NEW_NIL;
		auto saved_pred = pred->copy();
		auto saved_args = args->copy();
		while (eval(pred)->logical()) {
			MSCRIPT_CONS_FOREACH(e, args) {
				res = eval(e->car);
			}
			args = (ConsCell*)saved_args->copy();
			pred = saved_pred->copy();
		}
		return res;
	}
	
	Object* cond(ConsCell* args)
	{
		MSCRIPT_CONS_FOREACH(pair, args) {
			auto p = (ConsCell*)pair->car;
			if ((get_arg(p, "Object"))->logical()) {
				return get_arg(p, "Object");
			}
		}
		return MSCRIPT_NEW_NIL;
	}
	
	Object* foreach(ConsCell* args)
	{
		std::map<std::string, Object*> vars;
		auto init = (ConsCell*)get_raw_arg(args, "ConsCell");
		auto var = (Symbol*)get_raw_arg(init, "Symbol");
		auto saved_args = (ConsCell*)args->copy();
		vars[var->name] = nullptr;
		local::add_scope(vars);
		auto array = (Array*)get_arg(init, "Array");
		for (auto e: array->array) {
			variable_set(var->name, e);
			MSCRIPT_CONS_FOREACH(inst, args) {
				eval(inst->car);
			}
			args = (ConsCell*)saved_args->copy();
		}
		local::remove_scope();
		return MSCRIPT_NEW_NIL;
	}
	
	Object* map(ConsCell* args)
	{
		auto f = get_arg(args, "Object");
		auto arr = (Array*)get_arg(args, "Array");
		auto new_arr = (Array*)new_object<Array>();
		for (auto e: arr->array) {
			auto l = MSCRIPT_NEW_NIL;
			push_back(l, e);
			new_arr->array.push_back(function_call(f, l));
		}
		return new_arr;
	}
	
	Object* elt(ConsCell* args)
	{
		auto index = (Number*)get_arg(args, "Number");
		size_t i = (size_t)index->value;
		auto array = (Array*)get_arg(args, "Array");
		if (array->array.size() <= i or index->value < 0) {
			error("'%zu' invalid index in array.", i);
			return nullptr;
		}
		return array->array[i];
	}

	Object* set_elt(ConsCell* args)
	{
		auto index = (Number*)get_arg(args, "Number");
		size_t i = (size_t)index->value;
		auto array = (Array*)get_arg(args, "Array");
		auto o = get_arg(args, "Object");
		if (array->array.size() <= i or index->value < 0) {
			error("'%zu' invalid index in array.", i);
			return nullptr;
		}
		array->array[i] = o;
		return o;
	}
	
	Object* range(ConsCell* args)
	{
		auto arr = (Array*)new_object<Array>(std::vector<Object*>());
		Number* limits[3];
		int i = -1;
		do {
			i++;
			limits[i] = (Number*)get_arg(args, "Number");
		} while (not args->is_empty() and i < 3);
		
		long start, stop, step;
		
		switch (i) {
		case 0: {
			start = 0;
			stop = (long)limits[0]->value;
			step = 1;
			break;
		}
		case 1: {
			start = (long)limits[0]->value;
			stop = (long)limits[1]->value;
			step = 1;
			break;
		}
		case 2: {
			start = (long)limits[0]->value;
			stop = (long)limits[1]->value;
			step = std::abs((long)limits[2]->value);
			break;
		}	
		};
		
		if (start > stop) {
			for (long i = start; i >= stop; i -= step) {
				arr->array.push_back(new_object<Number>(i));
			}
		} else {
			for (long i = start; i < stop; i += step) {
				arr->array.push_back(new_object<Number>(i));
			}
		}
		
		return arr;
	}
	
	Object* do_(ConsCell* args)
	{
		Object* res = MSCRIPT_NEW_NIL;
		MSCRIPT_CONS_FOREACH(e, args) {
			res = eval(e->car);
		}
		return res;
	}

	Object* string_concat(ConsCell* args)
	{
		auto res = (String*)new_object<String>("\"\"");	
		while (not args->is_empty()) {
			res->value += (((String*)get_arg(args, "String"))->value);
		}
		return res;
	}


	
	Object* array_push_back(ConsCell* args)
	{
		auto arr = (Array*)get_arg(args, "Array");
		auto o = get_arg(args, "Object");
		arr->array.push_back(o);
		return o;
	}
	
	Object* array_pop_back(ConsCell* args)
	{
		auto arr = (Array*)get_arg(args, "Array");
		auto o = arr->array.back();
		arr->array.pop_back();
		return o;
	}
	
	Object* array_reversed(ConsCell* args)
	{
		auto arr = (Array*)get_arg(args, "Array");
		auto new_arr = (Array*)arr->copy();
		std::reverse(new_arr->array.begin(), new_arr->array.end());
		return new_arr;
	}
	
	Object* array_sorted(ConsCell* args)
	{
		auto arr = (Array*)get_arg(args, "Array");
		auto new_arr = (Array*)arr->copy();
		std::sort(new_arr->array.begin(), new_arr->array.end(), [](Object* o1, Object* o2) {
			auto args_ = MSCRIPT_NEW_NIL;
			push_back(args_, o1);
			push_back(args_, o2);
			return ((Bool*)less_than(args_))->value;
		});
		return new_arr;
	}
	
	std::map<std::string, Object*> bind_args(ConsCell* args)
	{
		std::map<std::string, Object*> variables;
		Object* res = MSCRIPT_NEW_NIL;
		do {
			auto pair = (ConsCell*)get_raw_arg(args, "ConsCell");
			variables[((Symbol*)get_raw_arg(pair, "Symbol"))->name] = get_arg(pair, "Object");
		} while (not args->is_empty());
		return variables;
	}
	
	Object* equals(ConsCell* args)
	{
		Bool* res = (Bool*)new_object<Bool>(true);
		Object* o = get_arg(args, "Object");
		do {
			bool r = o->equals(get_arg(args, "Object"));
			res->value = res->value and r;
		} while (not args->is_empty());
		return res;
	}
	
	Object* not_(ConsCell* args)
	{
		return new_object<Bool>(!(get_arg(args, "Object")->logical()));
	}
	
	Object* or_(ConsCell* args)
	{
		do {
			auto o = get_arg(args, "Object");
			if (o->logical()) {
				return o;
			}
		} while (not args->is_empty());
		return MSCRIPT_NEW_NIL;
	}
	
	Object* and_(ConsCell* args)
	{
		Object* o;
		do {
			o = get_arg(args, "Object");
			if (not o->logical()) {
				return MSCRIPT_NEW_NIL;
			}
		} while (not args->is_empty());
		return o;
	}
	
	Object* less_than(ConsCell* args)
	{
		Object* o = get_arg(args, "Object");
		bool res = true;
		if (o->type == "Number") {
			Number* n = (Number*)o;
			while (not args->is_empty()) {
				auto tmp = (Number*)get_arg(args, "Number");
				if (n->value >= tmp->value) {
					res = false;
				}
				n = tmp;
			}
		} else if (o->type == "String") {
			String* s = (String*)o;
			while (not args->is_empty()) {
				auto tmp = (String*)get_arg(args, "String");
				if (s->value >= tmp->value) {
					res = false;
				}
				s = tmp;
			}
		} else {
			error("This type does not support ordered comparisons.");
		}
		return new_object<Bool>(res);
	}
	
	Object* greater_than(ConsCell* args)
	{
		Object* o = get_arg(args, "Object");
		bool res = true;
		if (o->type == "Number") {
			Number* n = (Number*)o;
			while (not args->is_empty()) {
				auto tmp = (Number*)get_arg(args, "Number");
				if (n->value <= tmp->value) {
					res = false;
				}
				n = tmp;
			}
		} else if (o->type == "String") {
			String* s = (String*)o;
			while (not args->is_empty()) {
				auto tmp = (String*)get_arg(args, "String");
				if (s->value <= tmp->value) {
					res = false;
				}
				s = tmp;
			}
		} else {
			error("This type does not support ordered comparisons.");
		}
		return new_object<Bool>(res);
	}
	
	Object* less_than_equals(ConsCell* args)
	{
		Object* o = get_arg(args, "Object");
		bool res = true;
		if (o->type == "Number") {
			Number* n = (Number*)o;
			while (not args->is_empty()) {
				auto tmp = (Number*)get_arg(args, "Number");
				if (n->value > tmp->value) {
					res = false;
				}
				n = tmp;
			}
		} else if (o->type == "String") {
			String* s = (String*)o;
			while (not args->is_empty()) {
				auto tmp = (String*)get_arg(args, "String");
				if (s->value > tmp->value) {
					res = false;
				}
				s = tmp;
			}
		} else {
			error("This type does not support ordered comparisons.");
		}
		return new_object<Bool>(res);
	}
	
	Object* greater_than_equals(ConsCell* args)
	{
		Object* o = get_arg(args, "Object");
		bool res = true;
		if (o->type == "Number") {
			Number* n = (Number*)o;
			while (not args->is_empty()) {
				auto tmp = (Number*)get_arg(args, "Number");
				if (n->value < tmp->value) {
					res = false;
				}
				n = tmp;
			}
		} else if (o->type == "String") {
			String* s = (String*)o;
			while (not args->is_empty()) {
				auto tmp = (String*)get_arg(args, "String");
				if (s->value < tmp->value) {
					res = false;
				}
				s = tmp;
			}
		} else {
			error("This type does not support ordered comparisons.");
		}
		return new_object<Bool>(res);
	}

	/*---------------------------------------------------------------------
	 *                       Mathematical Functions                       |
	 * --------------------------------------------------------------------
	 */

	Object* add(ConsCell* args)
	{
		Number* res = (Number*)new_object<Number>(0);	
		while (not args->is_empty()) {
			res->value += (((Number*)get_arg(args, "Number"))->value);
		}
		return res;
	}

	Object* subtract(ConsCell* args)
	{
		Number* a = (Number*)get_arg(args, "Number");
		if (args->is_empty()) {
			return new_object<Number>(-a->value);
		} else {
			Number* b = (Number*)get_arg(args, "Number");
			return new_object<Number>(a->value - b->value);
		}
	}

	Object* multiply(ConsCell* args)
	{
		Number* res = (Number*)new_object<Number>(1);
		while (not args->is_empty()) {
			res->value *= (((Number*)get_arg(args, "Number"))->value);
		}
		return res;
	}

	Object* divide(ConsCell* args)
	{
		Number* a = (Number*)get_arg(args, "Number");
		if (args->is_empty()) {
			return new_object<Number>(1 / a->value);
		} else {
			Number* b = (Number*)get_arg(args, "Number");
			if (b->value == 0) {
				error("Math Error!\nCannot divide by zero.");
			}
			return new_object<Number>(a->value / b->value);
		}
	}

	Object* power(ConsCell* args)
	{
		Number* b = (Number*)get_arg(args, "Number");
		Number* e = (Number*)get_arg(args, "Number");

		return new_object<Number>(std::pow(b->value, e->value));
	}

	Object* sqrt(ConsCell* args)
	{
		Number* x = (Number*)get_arg(args, "Number");
		return new_object<Number>(std::sqrt(x->value));
	}
	
	Object* rem(ConsCell* args)
	{
		Number* a = (Number*)get_arg(args, "Number");
		Number* b = (Number*)get_arg(args, "Number");
		if (b->value == 0) {
			error("Math Error!\nCannot divide by zero.");
			return nullptr;
		}
		return new_object<Number>(std::fmod(a->value, b->value));
	}
	
	Object* sin(ConsCell* args)
	{
		Number* x = (Number*)get_arg(args, "Number");
		return new_object<Number>(std::sin(x->value));
	}
	
	Object* cos(ConsCell* args)
	{
		Number* x = (Number*)get_arg(args, "Number");
		return new_object<Number>(std::cos(x->value));
	}
	
	Object* tan(ConsCell* args)
	{
		Number* x = (Number*)get_arg(args, "Number");
		return new_object<Number>(std::tan(x->value));
	}
	
	Object* asin(ConsCell* args)
	{
		Number* x = (Number*)get_arg(args, "Number");
		return new_object<Number>(std::asin(x->value));
	}
	
	Object* acos(ConsCell* args)
	{
		Number* x = (Number*)get_arg(args, "Number");
		return new_object<Number>(std::acos(x->value));
	}
	
	Object* atan(ConsCell* args)
	{
		Number* x = (Number*)get_arg(args, "Number");
		return new_object<Number>(std::atan(x->value));
	}

	/*---------------------------------------------------------------------
	 *                             IO Functions                           |
	 * --------------------------------------------------------------------
	 */

	Object* printf(ConsCell* args)
	{
		std::cout << format__(args);
		return MSCRIPT_NEW_NIL;
	}

	Object* printfln(ConsCell* args)
	{
		std::cout << format__(args) << std::endl;
		return MSCRIPT_NEW_NIL;
	}
	
	Object* input(ConsCell* args)
	{
		if (not args->is_empty()) {
			printf(args);
		}
		auto s = (String*)new_object<String>("\"\"");
		std::getline(std::cin, s->value);
		return s;
	}

	
	Object* s_to_n(ConsCell* args)
	{
		auto n = (Number*)new_object<Number>(0);
		auto s = (String*)get_arg(args, "String");
		try {
			n->value = std::stod(s->value);
		} catch (std::invalid_argument e) {
			error("Could not convert '%s' to a number.", s->value.c_str());
			return nullptr;
		}
		return n;
	}

	Object* shell_command(ConsCell* args)
	{
		auto com = (String*)get_arg(args, "String");
		std::system(com->value.c_str());
		return MSCRIPT_NEW_NIL;
	}

	std::string format__(ConsCell* args)
	{
		std::string buffer;
		bool flag = false;
		String* format = (String*)get_arg(args, "String");
		for (auto c: format->value) {
			if (flag) {
				switch (c) {
				case '~': {
					buffer += '~';
					break;
				}
				case '%': {
					buffer += '\n';
					break;
				}
				case 'a': {
					Object* o = get_arg(args, "Object");
					buffer += o->str();
					break;
				}
				default: {
					error("Found unknown printf directive: \"~%c\"", c);
					break;
				}
				}
				flag = false;
				continue;
			}

			if (c == '~') {
				flag = true;
				continue;
			}

			buffer += c;
		}
		return buffer;
	}
}
