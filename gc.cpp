#include "gc.hpp"

namespace mscript {

	void gc()
	{
		round_num++;
		mark();
		sweep();
	}

	void mark()
	{
		int i = 0;
		auto variables = global::get_variables();
		for (auto it = variables->begin(); it != variables->end(); ++it) {
			mark_impl(it->second);
		}
	}
	
	void mark_impl(Object* o)
	{
		if (o and o->gc_tag == round_num) return;
		if (o and o->type == "ConsCell") {
			mark_impl(((ConsCell*)o)->car);
			mark_impl(((ConsCell*)o)->cdr);
		} else if (o and o->type == "Array") {
			for (auto& e: ((Array*)o)->array) {
				mark_impl(e);
			}
		} else if (o and o->type == "Func") {
			mark_impl(((Func*)o)->body);
		}
		if (o) o->gc_tag = round_num;
	}

	void sweep()
	{
		auto allocated_objects = get_allocated_objects();
		for (auto it = allocated_objects->begin(); it != allocated_objects->end(); ++it) {
			Object*& object = *it;
			if (object and object->gc_tag < round_num) {
				delete object;
				object = nullptr;
			}
		}
		allocated_objects->remove(nullptr);
	}
	
	void clean_memory()
	{
		auto allocated_objects = get_allocated_objects();
		for (auto it = allocated_objects->begin(); it != allocated_objects->end(); ++it) {
			if (*it) delete *it;
		}
	}

}
