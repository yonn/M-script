#include "mscript.hpp"

int main(int argc, char* argv[])
{
	if (argc < 2) {
		mscript::repl();
	} else {
		mscript::execute_script(argv[1]);
	}
	return 0;
}
