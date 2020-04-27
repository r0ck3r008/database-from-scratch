COMPILER := g++
COMPILER_FLAGS :=-g -c

all: statistics.o qp_tree.o qp_tree_helper.o operation.o tableinfo.o

statistics.o: statistics.cc statistics.h
	${COMPILER} ${COMPILER_FLAGS} statistics.cc

qp_tree.o: qp_tree.cc qp_tree.h
	${COMPILER} ${COMPILER_FLAGS} qp_tree.cc

qp_tree_helper.o: qp_tree_helper.cc qp_tree.h
	${COMPILER} ${COMPILER_FLAGS} qp_tree_helper.cc

operation.o: operation.cc operation.h
	${COMPILER} ${COMPILER_FLAGS} operation.cc

tableinfo.o:: tableinfo.cc tableinfo.h
	${COMPILER} ${COMPILER_FLAGS} tableinfo.cc

clean:
	rm -f *.o
