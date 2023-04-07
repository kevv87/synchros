CC = gcc
RM = rm -rf
PWD = $(shell pwd)

INCLUDE_FLAGS = -I${PWD}/../ -I${PWD}/../mesh/include
COMPILE_FLAGS = -Werror ${INCLUDE_FLAGS}
MESH_LIB = -L${PWD}/../mesh -lmesh

all: main

common:
	${MAKE} -C ../mesh

main: common main.c
	${CC} ${COMPILE_FLAGS} ${MESH_LIB} -o main.o main.c

test_emitter:
	${MAKE} -C ./test COMPILE_FLAGS='${COMPILE_FLAGS}' MESH_LIB='${MESH_LIB}'

clean: 
	${RM} *.o
	${RM} */*.o
