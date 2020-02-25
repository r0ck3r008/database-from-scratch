COMPILER := g++
COMPILER_FLAGS :=-g -c

all: timer.o

timer.o: timer.cc timer.h
	${COMPILER} ${COMPILER_FLAGS} timer.cc

clean:
	rm -f ./*.o
