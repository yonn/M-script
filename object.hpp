#ifndef MSCRIPT_OBJECT_HPP
#define MSCRIPT_OBJECT_HPP

#include <string>
#include <sstream>
#include <iostream>
#include <list>
#include <vector>

#include "error.hpp"

#define MSCRIPT_NEW_NIL ((mscript::ConsCell*)mscript::new_object<mscript::ConsCell>(nullptr, nullptr, true))

#define MSCRIPT_CONS_FOREACH(v, L) for(auto& v = L; v != nullptr; v = (mscript::ConsCell*)v->cdr)
				   

namespace mscript {


	class Object {
	public:
		size_t gc_tag;
		std::string type;

		virtual Object* init();
		virtual bool equals(Object* o);
		virtual Object* copy();
		virtual std::string repr();
		virtual std::string str();
		virtual bool logical();

		virtual ~Object();
	};

	static std::list<Object*> allocated_objects;

	std::list<Object*>* get_allocated_objects();
	void add_allocated_object(Object* o);

	template <typename O, typename... Args>
	Object* new_object(Args... args)
	{
		O* res = new O;
		add_allocated_object((Object*)res);
		return (Object*)res->init(args...);
	}

	class ConsCell : public Object {
	public:
		Object* car;
		Object* cdr;

		bool head;

		bool is_empty();

		virtual Object* init(Object* a, Object* d, bool h = false);
		virtual bool equals(Object* o);
		virtual Object* copy();
		virtual std::string repr();
		virtual bool logical();
	};

	void push_front(ConsCell*& c, Object* o);
	void push_back(ConsCell* c, Object* o);
	Object* pop_front(ConsCell*& c);
	Object* pop_back(ConsCell* c);

	class Number : public Object {
	public:
		long double value;

		virtual Object* init(long double v);
		virtual Object* copy();
		virtual std::string repr();
		virtual bool logical();
		virtual bool equals(Object* o);
	};

	class String : public Object {
	public:
		std::string value;

		virtual Object* init(std::string s);
		virtual bool equals(Object* o);
		virtual Object* copy();
		virtual std::string repr();
		virtual std::string str();
		virtual bool logical();
	};

	class Symbol : public Object {
	public:
		std::string name;

		virtual Object* init(std::string s);
		virtual Object* copy();
		virtual std::string repr();
	};

	class Bool : public Object {
	public:
		bool value;

		virtual Object* init(bool v);
		virtual bool equals(Object* o);
		virtual Object* copy();
		virtual std::string repr();
		virtual bool logical();
	};
	
	class Array : public Object {
	public:
		std::vector<Object*> array;
		
		virtual Object* init(std::vector<Object*> a = std::vector<Object*>());
		virtual bool equals(Object* o);
		virtual Object* copy();
		virtual std::string repr();
		virtual bool logical();
	};
	
	class Func : public Object {
	public:
		std::string name;
		ConsCell* body;
		std::vector<std::string> varnames;
		
		virtual Object* init();
		virtual std::string repr();
	};
}

#endif /*MSCRIPT_OBJECT_HPP*/
