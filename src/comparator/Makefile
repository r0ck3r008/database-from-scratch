COMPILER := g++
COMPILER_FLAGS := -g -c

all: comparison.o comparison_engine.o function.o

comparison.o: comparison.cc comparison.h
	${COMPILER} ${COMPILER_FLAGS} comparison.cc

comparison_engine.o: comparison_engine.cc comparison_engine.h
	${COMPILER} ${COMPILER_FLAGS} comparison_engine.cc

function.o: function.cc function.h
	${COMPILER} ${COMPILER_FLAGS} function.cc

clean:
	rm -f *.o
