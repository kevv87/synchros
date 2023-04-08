CC = gcc
RM = rm -rf
PWD = $(shell pwd)

INCLUDE_FLAGS = -I${PWD}/../ -I${PWD}/../mesh/include
COMPILE_FLAGS = -Werror ${INCLUDE_FLAGS}
MESH_LIB = -L${PWD}/../mesh -lmesh

all: common main

common:
	${MAKE} -C ../mesh
	ls ../mesh

main: main.c
	${CC} ${INCLUDE_FLAGS} ${MESH_LIB} -o main.o main.c

test_finalizer:
	${MAKE} -C ./test COMPILE_FLAGS='${COMPILE_FLAGS}' MESH_LIB='${MESH_LIB}'

clean: 
	${RM} *.o
