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

TEST_OUT_OBJS := entrypoint/entrypoint.o ${OPTIMIZERS} ${PARSERS} ${DBS} ${COMPARATORS} ${FSS} ${MEMS} ${REL_OPS} ${GLBLS}
GTEST_OUT_OBJS := tests/gtest.o ${OPTIMIZERS} ${PARSERS} ${DBS} ${COMPARATORS} ${FSS} ${MEMS} ${REL_OPS} ${GLBLS}

all: a5.out

a5.out: entrypoint.o parser_objs optimizer_objs db_objs comparator_objs fs_objs mem_objs rel_ops_objs glbl_objs
	${COMPILER} ${TEST_OUT_OBJS} ${LINKER_FLAGS} a5.out
	make clean_objs

gtest.out: gtest.o parser_objs optimizer_objs db_objs comparator_objs fs_objs mem_objs rel_ops_objs glbl_objs
	$(COMPILER) ${GTEST_OUT_OBJS} ${LINKER_FLAGS} gtest.out
	make clean_objs

entrypoint.o:
	make -C entrypoint/ entrypoint.o

gtest.o:
	make -C tests/ gtest.o

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
	make -C entrypoint/ clean
	rm -f ./*.o

distclean: clean
	rm -f tpch-dbgen/*.tbl
	rm -f bin/*.bin
	rm -f bin/*.bin.bigq

clean: clean_objs
	make -C tpch-dbgen clean
	rm -f ./*.out
