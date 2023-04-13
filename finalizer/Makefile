CC = gcc
RM = rm -rf
PWD = $(shell pwd)


INCLUDE_FLAGS = -I${PWD}/../ -I${PWD}/../mesh/include
COMPILE_FLAGS = -Werror ${INCLUDE_FLAGS}
MESH_LIB = -L${PWD}/../mesh -lmesh
LINKER_FLAGS += -lrt ${MESH_LIB}

all: common main

common:
	${MAKE} -C ../mesh
	ls ../mesh

main: finalizer.c
	${CC} ${INCLUDE_FLAGS} ${MESH_LIB} -o finalizer.o finalizer.c ${LINKER_FLAGS} 

test_finalizer:
	${MAKE} -C ./test COMPILE_FLAGS='${COMPILE_FLAGS}' MESH_LIB='${MESH_LIB}'

clean: 
	${RM} *.o
