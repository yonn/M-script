#ifndef MSCRIPT_MSCRIPT_HPP
#define MSCRIPT_MSCRIPT_HPP

#include <iostream>
#include <fstream>
#include <list>
#include <string>
#include <csetjmp>

#include "eval.hpp"
#include "builtins.hpp"
#include "object.hpp"
#include "lexer.hpp"
#include "gc.hpp"
#include "error.hpp"

#define ever (;;)

namespace mscript {

	void init(Mode m);

	void repl();

	void execute_script(const char* filename);
}

#endif /*MSCRIPT_MSCRIPT_HPP*/
