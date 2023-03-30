CC = gcc
INCLUDE_FLAGS = -I../mesh/include
MESH_LIB = -L../mesh -l mesh

all: common main

common:
	${MAKE} ../mesh

main: main.c
	${CC} ${INCLUDE_FLAGS} ${MESH_LIB} -o main.o main.c
