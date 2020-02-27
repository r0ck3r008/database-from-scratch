COMPILER := g++
COMPILER_FLAGS := -g -c

all: gtest.o test.o test_bigq.o heap_gen.o

gtest.o: gtest.cc
	${COMPILER} ${COMPILER_FLAGS} gtest.cc

test.o: test.cc test.h
	${COMPILER} ${COMPILER_FLAGS} test.cc

test_bigq.o: test_bigq.cc test.h
	${COMPILER} ${COMPILER_FLAGS} test_bigq.cc

heap_gen.o: heap_gen.cc test.h
	${COMPILER} ${COMPILER_FLAGS} heap_gen.cc

clean:
	rm -f ./*.o
