#include "error.hpp"

namespace mscript {

	jmp_buf& get_repl_start()
	{
		return repl_start;
	}

	void init_error(Mode m)
	{
		mode = m;
	}

	void error(std::string fmt, ...)
	{
		fmt += '\n';
		fmt += '\n';
		std::va_list args;
		va_start(args, fmt.c_str());

		std::fprintf(stderr, "Error: \n");
		std::vfprintf(stderr, fmt.c_str(), args);

		va_end(args);

		if (mode == Mode::Script) {
			std::exit(1);
		} else if (mode == Mode::REPL) {
			longjmp(repl_start, 1);
		}
	}
	
	void error(int line_number, std::string line, std::string fmt, ...)
	{
		std::va_list args;
		va_start(args, fmt.c_str());
		
		std::fprintf(stderr, "In file: \"%s\"::%d: error: ", filename.c_str(), line_number);
		std::vfprintf(stderr, fmt.c_str(), args);
		std::fprintf(stderr, "\n\t%s\n\n", line.c_str());

		va_end(args);
		if (mode == Mode::Script) {
			std::exit(1);
		} else if (mode == Mode::REPL) {
			longjmp(repl_start, 1);
		}
	}
	
}
