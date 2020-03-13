COMPILER := g++
COMPILER_FLAGS := -g -c

all: sel_file.o sel_pipe.o project.o write_out.o

sel_file.o: sel_file.cc sel_file.h
	${COMPILER} ${COMPILER_FLAGS} sel_file.cc

sel_pipe.o: sel_pipe.cc sel_pipe.h
	${COMPILER} ${COMPILER_FLAGS} sel_pipe.cc

project.o: project.cc project.h
	${COMPILER} ${COMPILER_FLAGS} project.cc

write_out.o: write_out.cc write_out.h
	${COMPILER} ${COMPILER_FLAGS} write_out.cc

clean:
	rm -f ./*.o
