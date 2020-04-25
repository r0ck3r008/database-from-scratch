COMPILER := g++
COMPILER_FLAGS := -g -c

all: schema.o catalog.o

schema.o: schema.cc schema.h
	${COMPILER} ${COMPILER_FLAGS} schema.cc

catalog.o: catalog.cc catalog.h
	${COMPILER} ${COMPILER_FLAGS} catalog.cc

clean:
	rm -f ./*.o
