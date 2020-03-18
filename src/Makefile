COMPILER = g++
COMPILER_FLAGS := -g -c
LINKER_FLAGS := -lfl -lgtest -lpthread -lsodium -o

REL_OPS := lex/rel_ops/*.o
MEMS := mem/*.o
DBS := db/*.o
LEXS := lex/*.o
GLBLS := glbl/*.o
FSS := fs/*.o

TEST_OUT_OBJS := tests/test.o ${REL_OPS} ${MEMS} ${DBS} ${LEXS} ${GLBLS} ${FSS}
GTEST_OUT_OBJS := tests/gtest.o ${REL_OPS} ${MEMS} ${DBS} ${LEXS} ${GLBLS} ${FSS}
SORTED_FILE_OUT_OBJS := tests/test_sorted_file.o ${MEMS} ${DBS} ${LEXS} ${FSS} ${GLBLS}
HEAP_GEN_OUT_OBJS := tests/heap_gen.o ${MEMS} ${DBS} ${LEXS} ${FSS} ${GLBLS}
TEST_BIGQ_OUT_OBJS := tests/test_bigq.o ${MEMS} ${DBS} ${LEXS} ${FSS} ${GLBLS}

all: test.out

test.out: test.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} ${TEST_OUT_OBJS} ${LINKER_FLAGS} test.out
	make clean_objs

gtest.out: gtest.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	$(COMPILER) ${GTEST_OUT_OBJS} ${LINKER_FLAGS} gtest.out
	make clean_objs

heap_gen.out: heap_gen.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} ${HEAP_GEN_OUT_OBJS} ${LINKER_FLAGS} heap_gen.out
	make clean_objs

test_bigq.out: test_bigq.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} ${TEST_BIGQ_OUT_OBJS} ${LINKER_FLAGS} test_bigq.out
	make clean_objs

sorted_test_file.out: test_sorted_file.o db_objs fs_objs mem_objs lex_objs dbgen_bin glbl_objs
	${COMPILER} ${SORTED_FILE_OUT_OBJS} ${LINKER_FLAGS} test_sorted_file.out
	make clean_objs

test.o:
	make -C tests/ test.o

gtest.o:
	make -C tests/ gtest.o

heap_gen.o:
	make -C tests/ heap_gen.o

test_bigq.o:
	make -C tests/ test_bigq.o

test_sorted_file.o:
	make -C tests/ test_sorted_file.o

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
