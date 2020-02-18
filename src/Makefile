COMPILER = g++
COMPILER_FLAGS :=-g -c
LINKER_FLAGS := -lfl -lgtest -lpthread -o
ALL_OBJS := db/schema.o fs/file.o fs/record.o fs/dbfile.o mem/two_way_list.o mem/pipe.o mem/tournament.o mem/bigq.o mem/run_gen.o mem/run_merge.o lex/comparison.o lex/comparison_engine.o lex/lex.yy.o lex/y.tab.o glbl/timer.o

all: test_bin clean_objs

main_all: main_bin clean_objs

gtest_all: gtest_bin clean_objs

test_bin: test.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_bin
	$(COMPILER) test.o ${ALL_OBJS} ${LINKER_FLAGS} test.out

test.o: test.cc test.h
	${COMPILER} ${COMPILER_FLAGS} test.cc

main_bin: main.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_bin
	${COMPILER} main.o ${ALL_OBJS} ${LINKER_FLAGS} main.out

main.o: main.cc
	${COMPILER} ${COMPILER_FLAGS} main.cc

gtest_bin: gtest.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_bin
	$(COMPILER) gtest.o ${ALL_OBJS} ${LINKER_FLAGS} gtest.out

gtest.o: gtest.cc
	${COMPILER} ${COMPILER_FLAGS} gtest.cc

db_objs:
	make -C db

fs_objs:
	make -C fs

mem_objs:
	make -C mem

lex_objs:
	make -C lex

dbgen_bin:
	make -C tpch-dbgen

glbl_bin:
	make -C glbl

clean_objs:
	make -C db clean
	make -C fs clean
	make -C mem clean
	make -C lex clean
	make -C glbl clean
	rm ./*.o

distclean: clean
	rm tpch-dbgen/*.tbl
	rm out/*.bin

clean:
	make -C tpch-dbgen clean
	rm ./*.out
