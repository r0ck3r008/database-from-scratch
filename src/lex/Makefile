COMPILER := g++
COMPILER_FLAGS := -O2 -g -c
TAG := -i

ifdef linux
	TAG := -n
endif

all: comparison.o comparison_engine.o y.tab.o lex.yy.o

comparison.o: comparison.cc comparison.h
	${COMPILER} ${COMPILER_FLAGS} comparison.cc

comparison_engine.o: comparison_engine.cc comparison_engine.h
	${COMPILER} ${COMPILER_FLAGS} comparison_engine.cc

y.tab.o: parser.y
	yacc -d parser.y
	sed ${TAG} y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	${COMPILER} ${COMPILER_FLAGS} y.tab.c

lex.yy.o: lexer.l
	lex lexer.l
	gcc ${COMPILER_FLAGS} lex.yy.c

clean:
	rm ./*.o
	rm y.tab.*
	rm lex.yy.c
