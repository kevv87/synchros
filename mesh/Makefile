$CC = gcc
$RM = rm
$AR = ar
$PWD = $(shell pwd)

all: mesh compile_static
LINKER_FLAGS += -lrt
COMPILER_FLAGS += -Werror -I${PWD}/include
MESH_LIB = -I${PWD}/include -L${PWD}/ -lmesh

FOR_TESTS += ${PWD}/../common/test/test_common.c -I${PWD}/../common/test ${MESH_LIB} ${LINKER_FLAGS}

MESH_SOURCES = mesh.c

mesh: mesh.c
	${CC} ${COMPILER_FLAGS} -o mesh.o -c ${MESH_SOURCES} ${LINKER_FLAGS}

test_shmem_handler: 
	make -C ./test/shmem_handler/

test: mesh compile_static test_shmem_handler

compile_static:
	${AR} rcs libmesh.a mesh.o

clean:
	rm -rf *.o *.a

