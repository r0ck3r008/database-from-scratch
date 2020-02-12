COMPILER := g++
COMPILER_FLAGS :=-g -c

all: two_way_list.o pipe.o tournament.o

two_way_list.o: two_way_list.cc two_way_list.h
	${COMPILER} ${COMPILER_FLAGS} two_way_list.cc

pipe.o: pipe.cc pipe.h
	${COMPILER} ${COMPILER_FLAGS} pipe.cc

tournament.o: tournament.cc tournament.h
	${COMPILER} ${COMPILER_FLAGS} tournament.cc

clean:
	rm ./*.o
