#ifndef MSCRIPT_GC_HPP
#define MSCRIPT_GC_HPP

#include <list>
#include <iostream>

#include "object.hpp"
#include "builtins.hpp"

namespace mscript {

	static size_t round_num = 0;
	
	void gc();

	void mark();
	void mark_impl(Object* o);
	
	void sweep();
	
	void clean_memory();
}

#endif /*MSCRIPT_GC_HPP*/
