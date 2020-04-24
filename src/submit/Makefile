COMPILER = g++
COMPILER_FLAGS :=-g -c
LINKER_FLAGS := -lfl -lgtest -lpthread -o
ALL_OBJS := schema.o file.o record.o dbfile.o heap.o sorted.o sort_helper.o two_way_list.o pipe.o tournament.o bigq.o run_gen.o run_merge.o comparison.o comparison_engine.o function.o lex.yy.o lex.yyfunc.o y.tab.o yyfunc.tab.o stat_helper.o statistics.o qp_tree.o qp_tree_helper.o

TAG := -i
ifdef linux
	TAG := -n
endif

all: test.out

test.out: test.o db_objs fs_objs mem_objs lex_objs
	${COMPILER} test.o ${ALL_OBJS} ${LINKER_FLAGS} a42.out
	make clean_objs

gtest.out: gtest.o db_objs fs_objs mem_objs lex_objs
	$(COMPILER) gtest.o ${ALL_OBJS} ${LINKER_FLAGS} gtest.out
	make clean_objs

heap_gen.out: heap_gen.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	${COMPILER} heap_gen.o ${ALL_OBJS} ${LINKER_FLAGS} heap_gen.out
	make clean_objs

test_bigq.out: test_bigq.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	${COMPILER} test_bigq.o ${ALL_OBJS} ${LINKER_FLAGS} test_bigq.out
	make clean_objs

sorted_test_file.out: test_sorted_file.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	${COMPILER} test_sorted_file.o ${ALL_OBJS} ${LINKER_FLAGS} test_sorted_file.out
	make clean_objs

test.o:
	${COMPILER} ${COMPILER_FLAGS} test.cc

gtest.o:
	${COMPILER} ${COMPILER_FLAGS} gtest.cc

fs_objs:
	${COMPILER} ${COMPILER_FLAGS} dbfile.cc sorted.cc sort_helper.cc heap.cc file.cc record.cc

db_objs:
	${COMPILER} ${COMPILER_FLAGS} schema.cc

mem_objs:
	${COMPILER} ${COMPILER_FLAGS} bigq.cc run_gen.cc run_merge.cc tournament.cc pipe.cc two_way_list.cc

lex_objs:
	${COMPILER} ${COMPILER_FLAGS} comparison_engine.cc comparison.cc function.cc stat_helper.cc statistics.cc qp_tree.cc qp_tree_helper.cc
	# parser.y
	yacc -d parser.y
	sed ${TAG} y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	${COMPILER} ${COMPILER_FLAGS} y.tab.c
	# parser_func.y
	yacc -p "yyfunc" -b "yyfunc" -d parser_func.y
	sed $(tag) yyfunc.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	g++ -c yyfunc.tab.c
	# lexer.l
	lex lexer.l
	gcc ${COMPILER_FLAGS} lex.yy.c
	# lexer_func.l
	lex -Pyyfunc lexer_func.l
	gcc  -c lex.yyfunc.c

clean_objs:
	rm -f ./*.o
	rm -f y.tab.*
	rm -f yyfunc.tab.*
	rm -f lex.yy.c
	rm -f lex.yyfunc.c

clean: clean_objs
	make -C tpch-dbgen clean
	rm -f *.out

distclean: clean
	rm -f *.tbl
	rm -f *.bin
	rm -f *.bin.bigq
	rm -f test.ps.w.tmp
