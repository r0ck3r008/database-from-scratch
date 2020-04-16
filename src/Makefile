COMPILER = g++
COMPILER_FLAGS := -g -c
LINKER_FLAGS := -lfl -lgtest -lpthread -o

REL_OPS := rel_ops/*.o
OPTIMIZERS := optimizer/*.o
PARSERS := parser/*.o
COMPARATORS := comparator/*.o
MEMS := mem/*.o
DBS := db/*.o
GLBLS := glbl/*.o
FSS := fs/*.o

TEST_OUT_OBJS := tests/test.o ${REL_OPS} ${MEMS} ${DBS} ${OPTIMIZERS} ${PARSERS} ${COMPARATORS} ${GLBLS} ${FSS}
TEST_STAT_OUT_OBJS := tests/test_stat.o ${REL_OPS} ${MEMS} ${DBS} ${OPTIMIZERS} ${PARSERS} ${COMPARATORS} ${GLBLS} ${FSS}
REL_OPS_OUT_OBJS := tests/test_rel_ops.o ${REL_OPS} ${MEMS} ${DBS} ${PARSERS} ${COMPARATORS} ${GLBLS} ${FSS}
GTEST_OUT_OBJS := tests/gtest.o ${REL_OPS} ${MEMS} ${DBS} ${PARSERS} ${COMPARATORS} ${GLBLS} ${FSS}
SORTED_FILE_OUT_OBJS := tests/test_sorted_file.o ${REL_OPS} ${MEMS} ${DBS} ${PARSERS} ${COMPARATORS} ${GLBLS} ${FSS}
HEAP_GEN_OUT_OBJS := tests/heap_gen.o ${REL_OPS} ${MEMS} ${DBS} ${PARSERS} ${COMPARATORS} ${GLBLS} ${FSS}
TEST_BIGQ_OUT_OBJS := tests/test_bigq.o ${REL_OPS} ${MEMS} ${DBS} ${PARSERS} ${COMPARATORS} ${GLBLS} ${FSS}

all: test.out

test.out: test.o db_objs fs_objs mem_objs dbgen_bin glbl_objs comparator_objs parser_objs optimizer_objs comparator_objs rel_ops_objs
	${COMPILER} ${TEST_OUT_OBJS} ${LINKER_FLAGS} test.out
	make clean_objs

test_stat.out: test_stat.o db_objs fs_objs mem_objs dbgen_bin glbl_objs comparator_objs parser_objs optimizer_objs comparator_objs rel_ops_objs
	${COMPILER} ${TEST_STAT_OUT_OBJS} ${LINKER_FLAGS} test_stat.out
	make clean_objs

test_rel_ops.out: test_rel_ops.o db_objs fs_objs mem_objs dbgen_bin glbl_objs comparator_objs parser_objs comparator_objs rel_ops_objs
	${COMPILER} ${REL_OPS_OUT_OBJS} ${LINKER_FLAGS} test_rel_ops.out
	make clean_objs

gtest.out: gtest.o db_objs fs_objs mem_objs dbgen_bin glbl_objs comparator_objs parser_objs comparator_objs rel_ops_objs
	$(COMPILER) ${GTEST_OUT_OBJS} ${LINKER_FLAGS} gtest.out
	make clean_objs

heap_gen.out: heap_gen.o db_objs fs_objs mem_objs dbgen_bin glbl_objs comparator_objs parser_objs comparator_objs rel_ops_objs
	${COMPILER} ${HEAP_GEN_OUT_OBJS} ${LINKER_FLAGS} heap_gen.out
	make clean_objs

test_bigq.out: test_bigq.o db_objs fs_objs mem_objs dbgen_bin glbl_objs comparator_objs parser_objs comparator_objs rel_ops_objs
	${COMPILER} ${TEST_BIGQ_OUT_OBJS} ${LINKER_FLAGS} test_bigq.out
	make clean_objs

sorted_test_file.out: sorted_test_file.o db_objs fs_objs mem_objs dbgen_bin glbl_objs comparator_objs parser_objs comparator_objs rel_ops_objs
	${COMPILER} ${SORTED_FILE_OUT_OBJS} ${LINKER_FLAGS} test_sorted_file.out
	make clean_objs

test.o:
	make -C tests/ test.o

test_stat.o:
	make -C tests/ test_stat.o

test_rel_ops.o:
	make -C tests/ test_rel_ops.o

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

optimizer_objs:
	make -C optimizer/

comparator_objs:
	make -C comparator/

parser_objs:
	make -C parser/

rel_ops_objs:
	make -C rel_ops/

glbl_objs:
	make -C glbl/

dbgen_bin:
	make -C tpch-dbgen/

clean_objs:
	make -C db/ clean
	make -C fs/ clean
	make -C mem/ clean
	make -C comparator/ clean
	make -C optimizer/ clean
	make -C parser/ clean
	make -C rel_ops/ clean
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
