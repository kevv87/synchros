$CC = gcc
$RM = rm -rf
$AR = ar

all: mesh compile_static
LINKER_FLAGS += -lrt
COMPILER_FLAGS += -Werror
MESH_LIB = -I./include -L./ -lmesh

FOR_TESTS += ../common/test/test_common.c -I../common/test ${MESH_LIB} ${LINKER_FLAGS}

mesh: mesh.c
	${CC} ${COMPILER_FLAGS} -o mesh.o -c mesh.c -I./include ${LINKER_FLAGS}

test_writer: test/test_writer.c
	${CC} ${COMPILER_FLAGS} -o test_writer.o test/test_writer.c ${FOR_TESTS}
	./test_writer.o

test: mesh compile_static test_writer

compile_static:
	${AR} rcs libmesh.a mesh.o

clean:
	${RM} *.o *.a

