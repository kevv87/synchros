CC = gcc
INCLUDE_FLAGS = -I../mesh/include
RM = rm -rf
PWD = $(shell pwd)
MESH_LIB = -L../mesh -lmesh

all: common main

common:
	${MAKE} -C ../mesh
	ls ../mesh

main: main.c
	${CC} ${INCLUDE_FLAGS} ${MESH_LIB} -o main.o main.c

clean: 
	${RM} *.o
