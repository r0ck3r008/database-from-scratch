COMPILER := g++
COMPILER_FLAGS := -g -c

all: schema.o catalog.o query.o

schema.o: schema.cc schema.h
	${COMPILER} ${COMPILER_FLAGS} schema.cc

catalog.o: catalog.cc catalog.h
	${COMPILER} ${COMPILER_FLAGS} catalog.cc

query.o: query.cc query.h
	${COMPILER} ${COMPILER_FLAGS} query.cc

clean:
	rm -f ./*.o
