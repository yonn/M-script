CC = g++
CFLAGS = -Wall -c -std=c++14 -g -Wno-unused-variable
LFLAGS = -Wall -std=c++14 -g -Wno-unused-variable

OBJS = mscript.o eval.o object.o lexer.o gc.o builtins.o error.o

mscript: main.cpp $(OBJS)
	$(CC) $(LFLAGS) main.cpp $(OBJS) -o mscript

mscript.o: mscript.cpp mscript.hpp eval.hpp object.hpp builtins.hpp error.hpp gc.hpp
	$(CC) $(CFLAGS) mscript.cpp

builtins.o: builtins.cpp builtins.hpp object.hpp eval.hpp error.hpp gc.hpp
	$(CC) $(CFLAGS) builtins.cpp
	
eval.o: eval.cpp eval.hpp object.hpp lexer.hpp error.hpp
	$(CC) $(CFLAGS) eval.cpp  

gc.o: gc.cpp gc.hpp error.hpp object.hpp
	$(CC) $(CFLAGS) gc.cpp

object.o: object.cpp object.hpp error.hpp
	$(CC) $(CFLAGS) object.cpp

lexer.o: lexer.cpp lexer.hpp
	$(CC) $(CFLAGS) lexer.cpp

error.o: error.cpp error.hpp
	$(CC) $(CFLAGS) error.cpp
