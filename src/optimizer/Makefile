COMPILER := g++
COMPILER_FLAGS :=-g -c

all: statistics.o stat_helper.o qp_tree.o qp_tree_helper.o

statistics.o: statistics.cc statistics.h
	${COMPILER} ${COMPILER_FLAGS} statistics.cc

stat_helper.o: stat_helper.cc statistics.h
	${COMPILER} ${COMPILER_FLAGS} stat_helper.cc

qp_tree.o: qp_tree.cc qp_tree.h
	${COMPILER} ${COMPILER_FLAGS} qp_tree.cc

qp_tree_helper.o: qp_tree_helper.cc qp_tree.h
	${COMPILER} ${COMPILER_FLAGS} qp_tree_helper.cc

clean:
	rm -f *.o
