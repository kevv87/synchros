CC = gcc
INCLUDE_FLAGS = -I../mesh/include
RM = rm -rf
PWD = $(shell pwd)

INCLUDE_FLAGS = -I${PWD}/../ -I${PWD}/../mesh/include
COMPILE_FLAGS = -Werror ${INCLUDE_FLAGS}
MESH_LIB = -L${PWD}/../mesh -lmesh

.PHONY: all

all: emisor

common:
	${MAKE} -C ../mesh
	ls ../mesh

emisor: common emisor.c
	$(CC) $(COMPILE_FLAGS) $(INCLUDE_FLAGS) -o emisor.o emisor.c $(MESH_LIB)

# emisor.o: emisor.c
# 	$(CC) $(CFLAGS) -c emisor.c

test_emitter:
	${MAKE} -C ./test COMPILE_FLAGS='${COMPILE_FLAGS}' MESH_LIB='${MESH_LIB}'

run: emisor
	./emisor.o

clean:
	rm -f emisor emisor.o
