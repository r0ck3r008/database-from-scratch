COMPILER = g++
COMPILER_FLAGS :=-g -c
LINKER_FLAGS := -lfl -lgtest -lpthread -o
ALL_OBJS := schema.o file.o record.o dbfile.o heap.o sorted.o sort_helper.o two_way_list.o pipe.o tournament.o bigq.o run_gen.o run_merge.o comparison.o comparison_engine.o function.o lex.yy.o lex.yyfunc.o y.tab.o yyfunc.tab.o sel_pipe.o sel_file.o project.o sum.o join.o group_by.o write_out.o rem_dup.o timer.o

TAG := -i
ifdef linux
	TAG := -n
endif

all: test.out

test.out: test.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	${COMPILER} test.o ${ALL_OBJS} ${LINKER_FLAGS} test.out
	make clean_objs

gtest.out: gtest.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	$(COMPILER) gtest.o ${ALL_OBJS} ${LINKER_FLAGS} gtest.out
	make clean_objs

heap_gen.out: heap_gen.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	${COMPILER} heap_gen.o ${ALL_OBJS} ${LINKER_FLAGS} heap_gen.out
	make clean_objs

test_bigq.out: test_bigq.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	${COMPILER} test_bigq.o ${ALL_OBJS} ${LINKER_FLAGS} test_bigq.out
	make clean_objs

sorted_test_file.out: test_sorted_file.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs rel_op_objs
	${COMPILER} test_bigq.o ${ALL_OBJS} ${LINKER_FLAGS} test_bigq.out
	make clean_objs

test.o:
	${COMPILER} ${COMPILER_FLAGS} test.cc

gtest.o:
	${COMPILER} ${COMPILER_FLAGS} gtest.cc

heap_gen.o:
	${COMPILER} ${COMPILER_FLAGS} heap_gen.cc

test_bigq.o:
	${COMPILER} ${COMPILER_FLAGS} test_bigq.cc

test_sorted_file.o:
	${COMPILER} ${COMPILER_FLAGS} test_sorted_file.cc

fs_objs:
	${COMPILER} ${COMPILER_FLAGS} dbfile.cc sorted.cc sort_helper.cc heap.cc file.cc record.cc

db_objs:
	${COMPILER} ${COMPILER_FLAGS} schema.cc

mem_objs:
	${COMPILER} ${COMPILER_FLAGS} bigq.cc run_gen.cc run_merge.cc tournament.cc pipe.cc two_way_list.cc

glbl_objs:
	${COMPILER} ${COMPILER_FLAGS} timer.cc

lex_objs:
	${COMPILER} ${COMPILER_FLAGS} comparison_engine.cc comparison.cc function.cc
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

rel_op_objs:
	${COMPILER} ${COMPILER_FLAGS} sel_pipe.cc sel_file.cc project.cc sum.cc join.cc rem_dup.cc group_by.cc write_out.cc

dbgen_bin:
	make -C tpch-dbgen/

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
