COMPILER := g++
COMPILER_FLAGS :=-g -c

all: two_way_list.o pipe.o tournament.o bigq.o run_gen.o

two_way_list.o: two_way_list.cc two_way_list.h
	${COMPILER} ${COMPILER_FLAGS} two_way_list.cc

pipe.o: pipe.cc pipe.h
	${COMPILER} ${COMPILER_FLAGS} pipe.cc

tournament.o: tournament.cc tournament.h
	${COMPILER} ${COMPILER_FLAGS} tournament.cc

bigq.o: bigq.cc bigq.h
	${COMPILER} ${COMPILER_FLAGS} bigq.cc

run_gen.o: run_gen.cc run_gen.h
	${COMPILER} ${COMPILER_FLAGS} run_gen.cc

clean:
	rm ./*.o
