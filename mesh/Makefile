$CC = gcc
$RM = rm
$AR = ar
$PWD = $(shell pwd)

all: mesh compile_static
LINKER_FLAGS += -lrt -lpthread
COMPILER_FLAGS += -Werror -I./include -I../
MESH_LIB = -I./include -L./ -lmesh

FOR_TESTS +=\
	${PWD}/../common/test/test_common.c -I${PWD}/../common/test ${MESH_LIB} ${LINKER_FLAGS}

MESH_SOURCES = mesh.c shmem_handler.c

mesh: mesh.c
	${CC} ${COMPILER_FLAGS} -c ${MESH_SOURCES} ${LINKER_FLAGS}

test_shmem_handler: 
	make -C ./test/shmem_handler/

test_buffer: mesh compile_static test/test_buffer.c
	${CC} ${COMPILER_FLAGS} -o test/test_buffer.o test/test_buffer.c ${FOR_TESTS}
	./test/test_buffer.o

test_auditory: mesh compile_static test/test_auditory.c
	${CC} ${COMPILER_FLAGS} -o test/test_auditory.o test/test_auditory.c ${FOR_TESTS}
	./test/test_auditory.o

test: mesh compile_static test_shmem_handler test_buffer test_auditory

compile_static:
	${AR} rcs libmesh.a *.o
	rm -rf *.o

clean:
	rm -rf *.o *.a

