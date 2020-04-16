COMPILER := g++
COMPILER_FLAGS := -g -c

test.o: test.cc mk_stats.cc
	${COMPILER} ${COMPILER_FLAGS} test.cc mk_stats.cc

test_stat.o: test_stat.cc
	${COMPILER} ${COMPILER_FLAGS} test_stat.cc

test_rel_ops.o: test_rel_ops.cc test.h
	${COMPILER} ${COMPILER_FLAGS} test_rel_ops.cc

gtest.o: gtest.cc
	${COMPILER} ${COMPILER_FLAGS} gtest.cc

test_bigq.o: test_bigq.cc test.h
	${COMPILER} ${COMPILER_FLAGS} test_bigq.cc

heap_gen.o: heap_gen.cc test.h
	${COMPILER} ${COMPILER_FLAGS} heap_gen.cc

clean:
	rm -f ./*.o
