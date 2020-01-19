COMPILER = g++
COMPILER_FLAGS := -O2 -g -c
LINKER_FLAGS := -lfl -o
ALL_OBJS := db/schema.o fs/file.o fs/record.o mem/two_way_list.o lex/comparison.o lex/comparison_engine.o lex/lex.yy.o lex/y.tab.o

test: test.o fs/dbfile.o ${DIR_TARGETS}
	$(COMPILER) ${LINKER_FLAGS} test.o fs/dbfile.o ${ALL_OBJS} test.bin

test.o: test.cc test.h
	${COMPILER} ${COMPILER_FLAGS} test.cc

main: main.o db_objs fs_objs mem_objs lex_objs tpch-dbgen
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

clean:
	make -C db clean
	make -C fs clean
	make -C mem clean
	make -C lex clean
	make -C tpch-dbgen clean
	rm ./*.o
