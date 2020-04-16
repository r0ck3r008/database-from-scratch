COMPILER := g++
COMPILER_FLAGS :=-g -c
TAG := -i

ifdef linux
	TAG := -n
endif

all: y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o

y.tab.o: parser.y
	yacc -d parser.y
	sed $(tag) y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	g++ -c y.tab.c

yyfunc.tab.o: parser_func.y
	yacc -p "yyfunc" -b "yyfunc" -d parser_func.y
	sed $(tag) yyfunc.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	g++ -c yyfunc.tab.c

lex.yy.o: lexer.l
	lex lexer.l
	gcc  -c lex.yy.c

lex.yyfunc.o: lexer_func.l
	lex -Pyyfunc lexer_func.l
	gcc  -c lex.yyfunc.c

clean:
	rm -f ./*.o
	rm -f y.tab.*
	rm -f lex.yy.c
	rm -f lex.yyfunc.c
	rm -f yyfunc.tab*
