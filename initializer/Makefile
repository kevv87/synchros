CC = gcc
RM = rm -rf
PWD = $(shell pwd)

MESH_LIB = -L../mesh -lmesh
LINKER_FLAGS += -lrt $(MESH_LIB)
COMPILER_FLAGS += -Werror -I../mesh/include -I../

all: common initializer

common:
	${MAKE} -C ../mesh
	ls ../mesh

initializer: common initializer.c
	${CC} ${COMPILER_FLAGS}  -o initializer.o initializer.c ${LINKER_FLAGS}

clean: 
	${RM} *.o
