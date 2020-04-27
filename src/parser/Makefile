COMPILER := g++
COMPILER_FLAGS :=-g -c
TAG := -i

all: y.tab.o lex.yy.o

y.tab.o: parser.y
	yacc -d parser.y
	sed -i y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	g++ -c y.tab.c

lex.yy.o: lexer.l
	lex lexer.l
	gcc  -c lex.yy.c

clean:
	rm -f ./*.o
	rm -f y.tab.*
	rm -f lex.yy.c
