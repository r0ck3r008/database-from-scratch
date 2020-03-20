COMPILER := g++
COMPILER_FLAGS := -g -c

all: sel_file.o sel_pipe.o project.o sum.o join.o group_by.o rem_dup.o write_out.o

sel_file.o: sel_file.cc sel_file.h
	${COMPILER} ${COMPILER_FLAGS} sel_file.cc

sel_pipe.o: sel_pipe.cc sel_pipe.h
	${COMPILER} ${COMPILER_FLAGS} sel_pipe.cc

project.o: project.cc project.h
	${COMPILER} ${COMPILER_FLAGS} project.cc

sum.o: sum.cc sum.h
	${COMPILER} ${COMPILER_FLAGS} sum.cc

join.o: join.cc join.h
	${COMPILER} ${COMPILER_FLAGS} join.cc

group_by.o: group_by.cc group_by.h
	${COMPILER} ${COMPILER_FLAGS} group_by.cc

rem_dup.o: rem_dup.cc rem_dup.h
	${COMPILER} ${COMPILER_FLAGS} rem_dup.cc

write_out.o: write_out.cc write_out.h
	${COMPILER} ${COMPILER_FLAGS} write_out.cc

clean:
	rm -f ./*.o
