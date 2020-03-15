COMPILER = g++
COMPILER_FLAGS := -g -c
LINKER_FLAGS := -lfl -lgtest -lpthread -o
ALL_OBJS := db/*.o fs/*.o lex/*.o lex/rel_ops/*.o mem/*.o glbl/*.o

all: test.out gtest.out heap_gen.out test_bigq.out sorted_test_file.out clean_objs

test.out: test.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} tests/test.o ${ALL_OBJS} ${LINKER_FLAGS} test.out

sorted_test_file.out: test_sorted_file.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} tests/test_sorted_file.o ${ALL_OBJS} ${LINKER_FLAGS} test_sorted_file.out

gtest.out: gtest.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	$(COMPILER) tests/gtest.o ${ALL_OBJS} ${LINKER_FLAGS} gtest.out

heap_gen.out: heap_gen.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} tests/heap_gen.o ${ALL_OBJS} ${LINKER_FLAGS} heap_gen.out

test_bigq.out: test_bigq.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} tests/test_bigq.o ${ALL_OBJS} ${LINKER_FLAGS} test_bigq.out

test.o:
	make -C tests/ test.o

gtest.o:
	make -C tests/ gtest.o

test_sorted_file.o:
	make -C tests/ test_sorted_file.o

heap_gen.o:
	make -C tests/ heap_gen.o

test_bigq.o:
	make -C tests/ test_bigq.o

db_objs:
	make -C db/

fs_objs:
	make -C fs/

mem_objs:
	make -C mem/

lex_objs:
	make -C lex/

glbl_objs:
	make -C glbl/

dbgen_bin:
	make -C tpch-dbgen/

clean_objs:
	make -C db/ clean
	make -C fs/ clean
	make -C mem/ clean
	make -C lex/ clean
	make -C glbl/ clean
	make -C tests/ clean
	rm -f ./*.o

distclean: clean
	rm -f tpch-dbgen/*.tbl
	rm -f bin/*.bin
	rm -f bin/*.bin.bigq

clean: clean_objs
	make -C tpch-dbgen clean
	rm -f ./*.out
