COMPILER := g++
COMPILER_FLAGS := -O2 -g -c

all: file.o record.o dbfile.o

file.o: file.cc file.h
	${COMPILER} ${COMPILER_FLAGS} file.cc

record.o: record.cc record.h
	${COMPILER} ${COMPILER_FLAGS} record.cc

dbfile.o: dbfile.cc dbfile.h
	${COMPILER} ${COMPILER_FLAGS} dbfile.cc

clean:
	rm ./*.o
