COMPILER = g++
COMPILER_FLAGS := -O2 -g -c
LINKER_FLAGS := -lfl -o
ALL_OBJS := db/schema.o fs/file.o fs/record.o mem/two_way_list.o lex/comparison.o lex/comparison_engine.o lex/lex.yy.o lex/y.tab.o

all: test_bin clean_objs

main_all: main_bin clean_objs

test_bin: test.o db_objs fs_objs mem_objs lex_objs dbgen_bin
	$(COMPILER) test.o fs/dbfile.o ${ALL_OBJS} ${LINKER_FLAGS} test.bin

test.o: test.cc test.h
	${COMPILER} ${COMPILER_FLAGS} test.cc

main_bin: main.o db_objs fs_objs mem_objs lex_objs dbgen_bin
	${COMPILER} main.o ${ALL_OBJS} ${LINKER_FLAGS} main.bin

main.o: main.cc
	${COMPILER} ${COMPILER_FLAGS} main.cc

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

clean_objs:
	make -C db clean
	make -C fs clean
	make -C mem clean
	make -C lex clean
	rm ./*.o

clean:
	make -C tpch-dbgen clean
	rm ./*.bin
	rm tpch-dbgen/*.tbl
	rm out/*.bin
