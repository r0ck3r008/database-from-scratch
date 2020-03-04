COMPILER = g++
COMPILER_FLAGS :=-g -c
LINKER_FLAGS := -lfl -lgtest -lpthread -o
ALL_OBJS := schema.o file.o record.o dbfile.o heap.o sorted.o sort_helper.o two_way_list.o pipe.o tournament.o bigq.o run_gen.o run_merge.o comparison.o comparison_engine.o lex.yy.o y.tab.o timer.o

TAG := -i
ifdef linux
	TAG := -n
endif

all: test.out gtest.out test_heap.out test_bigq.out clean_objs

test.out: test.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} test.o ${ALL_OBJS} ${LINKER_FLAGS} test.out

gtest.out: gtest.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	$(COMPILER) gtest.o ${ALL_OBJS} ${LINKER_FLAGS} gtest.out

test_heap.out: test_heap.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} test_heap.o ${ALL_OBJS} ${LINKER_FLAGS} test_heap.out

test_bigq.out: test_bigq.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} test_bigq.o ${ALL_OBJS} ${LINKER_FLAGS} test_bigq.out

test.o:
	${COMPILER} ${COMPILER_FLAGS} test.cc

gtest.o:
	${COMPILER} ${COMPILER_FLAGS} gtest.cc

test_heap.o:
	${COMPILER} ${COMPILER_FLAGS} test_heap.cc

test_bigq.o:
	${COMPILER} ${COMPILER_FLAGS} test_bigq.cc

fs_objs:
	${COMPILER} ${COMPILER_FLAGS} dbfile.cc sorted.cc sort_helper.cc heap.cc file.cc record.cc

db_objs:
	${COMPILER} ${COMPILER_FLAGS} schema.cc

mem_objs:
	${COMPILER} ${COMPILER_FLAGS} bigq.cc run_gen.cc run_merge.cc tournament.cc pipe.cc two_way_list.cc

glbl_objs:
	${COMPILER} ${COMPILER_FLAGS} timer.cc

lex_objs:
	${COMPILER} ${COMPILER_FLAGS} comparison_engine.cc comparison.cc
	yacc -d parser.y
	sed ${TAG} y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/"
	${COMPILER} ${COMPILER_FLAGS} y.tab.c
	lex lexer.l
	gcc ${COMPILER_FLAGS} lex.yy.c

dbgen_bin:
	make -C tpch-dbgen/

clean_objs:
	rm -f ./*.o

distclean: clean
	rm -f tpch-dbgen/*.tbl
	rm -f bin/*.bin
	rm -f bin/*.bin.bigq

clean: clean_objs
	make -C tpch-dbgen clean
	rm -f ./*.out
