#include "mscript.hpp"

namespace mscript {

	void init(Mode m)
	{
		init_error(m);
		init_builtins();
	}

	void repl()
	{
		init(Mode::REPL);
		int index = -1;
		setjmp(get_repl_start());
		for ever {
			index++;
			std::string program;
			std::cout << "In [" << index << "]: ";
			std::getline(std::cin, program);
			if (program == ":q") { 
				break;
			} else if (program == ":cls") {
				system("clear");
				continue;
			} else if (program == "") {
				std::cout << std::endl;
				index--;
				continue;
			}
			auto o = read(program);
			if (o) o = eval(o);
			if (o) std::cout << "Out [" << index << "]: " << o->repr() << std::endl << std::endl;
			gc();
		}
		clean_memory();
	}

	void execute_script(const char* filename)
	{
		init(Mode::Script);
		std::ifstream f(filename);
		bool comment = false;
		while (f) {
			std::string program;
			char buffer;
			int count = 0;
			do {
				f.get(buffer);
				if (buffer == '\n') comment = false;
				if (buffer == ';') comment = true;
				if (not comment and buffer == '(') count++;
				if (not comment and buffer == ')') count--;
				if (not comment) program += buffer;
			} while (count > 0 and not comment);
			
			auto o = read(program);
			if (o) {
				o = eval(o);
			}
			gc();
		}
		clean_memory();
	}
}
