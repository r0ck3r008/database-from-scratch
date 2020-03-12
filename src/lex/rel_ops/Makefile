COMPILER := g++
COMPILER_FLAGS := -g -c

all: sel_file.o sel_pipe.o write_out.o

sel_file.o: sel_file.cc
	${COMPILER} ${COMPILER_FLAGS} sel_file.cc

sel_pipe.o: sel_pipe.cc
	${COMPILER} ${COMPILER_FLAGS} sel_pipe.cc

write_out.o: write_out.cc
	${COMPILER} ${COMPILER_FLAGS} write_out.cc

clean:
	rm -f ./*.o
