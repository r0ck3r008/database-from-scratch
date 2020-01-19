COMPILER := g++
COMPILER_FLAGS := -O2 -g -c

all: schema.o

schema.o: schema.cc schema.h
	${COMPILER} ${COMPILER_FLAGS} schema.cc

clean:
	rm ./*.o
