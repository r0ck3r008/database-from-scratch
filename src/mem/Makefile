COMPILER := g++
COMPILER_FLAGS := -O2 -g -c

all: two_way_list.o

two_way_list.o: two_way_list.cc two_way_list.h
	${COMPILER} ${COMPILER_FLAGS} two_way_list.cc

clean:
	rm ./*.o
