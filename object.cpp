#include "object.hpp"

namespace mscript {


	/*----------------------------------------------------------------------------
	 *                                    Object                                 |
	 * ---------------------------------------------------------------------------
	 */

	Object* Object::init()
	{
		this->type = "Object";
		this->gc_tag = 0;
		return this;
	}

	bool Object::equals(Object* o)
	{
		return this == o;
	}

	Object* Object::copy()
	{
		return this;
	}

	std::string Object::repr()
	{
		std::ostringstream s;
		s << "Object:" << this->type << "<" << this << ">";
		return s.str();
	}

	std::string Object::str()
	{
		return this->repr();
	}

	bool Object::logical()
	{
		return true;
	}

	Object::~Object()
	{
	}
	
	void add_allocated_object(Object* o)
	{
		allocated_objects.push_back(o);
	}

	std::list<Object*>* get_allocated_objects()
	{
		return &allocated_objects;
	}

	/*----------------------------------------------------------------------------
	 *                                   ConsCell                                |
	 * ---------------------------------------------------------------------------
	 */

	bool ConsCell::is_empty()
	{
		if (this->car == nullptr and this->cdr == nullptr) {
			return true;
		}
		return false;
	}

	Object* ConsCell::init(Object* a, Object* d, bool h)
	{
		this->car = a;
		this->cdr = d;
		this->type = "ConsCell";
		this->gc_tag = 0;
		this->head = h;
		return this;
	}

	bool ConsCell::equals(Object* o)
	{
		bool d;
		if (this->type != o->type) {
			return false;
		}
		ConsCell* c = (ConsCell*)o;
		if (this->cdr == nullptr) {
			d = (this->cdr == c->cdr);
		} else {
			d = this->cdr->equals(c->cdr);
		}

		return (this->car->equals(c->car) and d);
	}

	Object* ConsCell::copy()
	{
		ConsCell* c;
		if (this->is_empty()) {
			c = (ConsCell*)new_object<ConsCell>(nullptr, nullptr, this->head);
		} else if (this->cdr == nullptr) {
			c = (ConsCell*)new_object<ConsCell>(this->car->copy(), nullptr, this->head);
		} else {
			c = (ConsCell*)new_object<ConsCell>(this->car->copy(), this->cdr->copy(), this->head);
		}
		return c;
	}

	std::string ConsCell::repr()
	{
		std::ostringstream s;
		if (this->is_empty()) {
			s << "nil";
		} else {
			s << (this->head? "(" : "") << ((this->car)? this->car->repr() : "") << " " << ((this->cdr == nullptr)? "\b)" : this->cdr->repr());
		}
		return s.str();
	}
	
	bool ConsCell::logical()
	{
		return not this->is_empty();
	}

	void push_front(ConsCell*& c, Object* o)
	{
		if (c->is_empty()) {
			c->car = o;
			return;
		}
		c->head = false;
		c = (ConsCell*)new_object<ConsCell>(o, c, true);
	}

	void push_back(ConsCell* c, Object* o)
	{
		ConsCell* i;
		if (c->is_empty()) {
			c->car = o;
			return;
		}
		for (i = c; i->cdr != nullptr; i = (ConsCell*)i->cdr) { }
		i->cdr = new_object<ConsCell>(o, nullptr);
	}

	Object* pop_front(ConsCell*& c)
	{
		if (c->is_empty()) {
			error("Attempted to pop_front on an empty list.");
		}

		if (c->cdr and c->cdr->type == "ConsCell") {
			((ConsCell*)c->cdr)->head = true;
			ConsCell* next = (ConsCell*)c->cdr;
			Object* res = c->car;
			c->init(nullptr, nullptr);
			c = next;
			return res;
		} else {
			Object* res = ((ConsCell*)c)->car;
			((ConsCell*)c)->init(nullptr, nullptr);
			return res;
			
		}
	}

	Object* pop_back(ConsCell* c)
	{
		if (c->is_empty()) {
			error("Attempted to pop_back on an empty list.");
		}
		ConsCell* i;
		ConsCell* prev;
		for (i = c; i->cdr != nullptr; i = (ConsCell*)i->cdr) { prev = i; }
		prev->cdr = nullptr;
		Object* res = i->car;
		i->init(new Object, nullptr);
		return res;
	}


	/*----------------------------------------------------------------------------
	 *                                    Number                                 |
	 * ---------------------------------------------------------------------------
	 */

	bool Number::logical()
	{
		return (bool)this->value;
	}

	bool Number::equals(Object* o)
	{
		if (this->type != o->type) return false;
		return this->value == ((Number*)o)->value;
	}

	Object* Number::init(long double v)
	{
		this->type = "Number";
		this->gc_tag = 0;
		this->value = v;
		return this;
	}

	Object* Number::copy()
	{
		return new_object<Number>(this->value);
	}

	std::string Number::repr()
	{
		std::ostringstream s;
		s << this->value;
		return s.str();
	}

	/*---------------------------------------------------------------------
	 *                                 String                             |
	 *---------------------------------------------------------------------
	 */

	Object* String::init(std::string s)
	{
		this->type = "String";
		this->gc_tag = 0;
		this->value = "";
		for (size_t i = 1; i < s.length() - 1; i++) {
			this->value += s[i];
		}
		return this;
	}

	bool String::equals(Object* o)
	{
		if (this->type != o->type) return false;
		return this->value == ((String*)o)->value;
	}

	Object* String::copy()
	{
		return new_object<String>("\"" + this->value + "\"");
	}

	std::string String::repr()
	{
		return "\"" + this->value + "\"";
	}
	
	std::string String::str()
	{
		return this->value;
	}

	bool String::logical()
	{
		return this->value != "";
	}

	/*---------------------------------------------------------------------
	 *                                 Symbol                             |
	 *---------------------------------------------------------------------
	 */

	Object* Symbol::init(std::string s)
	{
		this->type = "Symbol";
		this->gc_tag = 0;
		this->name = s;
		return this;
	}

	Object* Symbol::copy()
	{
		return new_object<Symbol>(this->name);
	}

	std::string Symbol::repr()
	{
		return "#" + this->name;
	}
	
	/*---------------------------------------------------------------------
	 *                                 Bool                               |
	 *---------------------------------------------------------------------
	 */

	Object* Bool::init(bool v)
	{
		this->type = "Bool";
		this->gc_tag = 0;
		this->value = v;
		return this;
	}

	bool Bool::equals(Object* o)
	{
		return this->value == o->logical();
	}

	Object* Bool::copy()
	{
		return new_object<Bool>(this->value);
	}

	std::string Bool::repr()
	{
		return this->value? "true" : "false";
	}

	bool Bool::logical()
	{
		return this->value;
	}
	
	/*---------------------------------------------------------------------
	*                              Array                                   |
	*----------------------------------------------------------------------
	*/
	
	Object* Array::init(std::vector<Object*> a)
	{
		this->gc_tag = 0;
		this->array = a;
		this->type = "Array";
		return this;
	}
	
	bool Array::equals(Object* o)
	{
		if (this->type != o->type) return false;
		Array* a = (Array*)o;
		if (a->array.size() != this->array.size()) return false;
		for (size_t i = 0; i < this->array.size(); i++) {
			if (not (this->array[i]->equals(a->array[i]))) {
				return false;
			}
		}
		return true;
	}
	
	Object* Array::copy()
	{
		return new_object<Array>(this->array);
	}
	
	std::string Array::repr()
	{
		std::ostringstream s;
		bool flag;
		s << "[";
		for (auto e: this->array) {
			flag = true;
			s << e->repr() << " ";
		}
		s << (flag? "\b]" : "]");
		return s.str();
	}
	
	bool Array::logical()
	{
		return (bool)this->array.size();
	}
	
	/*---------------------------------------------------------------------
	*                             Func                                    |
	*----------------------------------------------------------------------
	*/
	
	Object* Func::init()
	{
		this->type = "Func";
		this->gc_tag = 0;
		return this;
	}
	
	std::string Func::repr()
	{
		std::ostringstream s;
		s << "<Function: " << this->name << ">";
		return s.str();
	}
}
