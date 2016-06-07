#ifndef MSCRIPT_ERROR_HPP
#define MSCRIPT_ERROR_HPP

#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstdarg>
#include <csetjmp>

namespace mscript {

	enum class Mode { REPL, Script };
	
	static std::string filename;
	static Mode mode;
	static jmp_buf repl_start;

	jmp_buf& get_repl_start();

	void init_error(Mode m);

	void error(std::string fmt, ...);
	void error(int line_number, std::string line, std::string fmt, ...);

}

#endif /*MSCRIPT_ERROR_HPP*/
