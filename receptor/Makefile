CC = gcc
INCLUDE_FLAGS = -I../mesh/include
RM = rm -rf
PWD = $(shell pwd)

INCLUDE_FLAGS = -I${PWD}/../ -I${PWD}/../mesh/include
COMPILE_FLAGS = -Werror ${INCLUDE_FLAGS}
MESH_LIB = -L${PWD}/../mesh -lmesh

.PHONY: all

all: receptor run

receptor: receptor.o
	$(CC) $(COMPILE_FLAGS) $(MESH_LIB) receptor.o -o receptor

receptor.o: receptor.c
	$(CC) $(CFLAGS) -c receptor.c

test_receptor:
	${MAKE} -C ./test COMPILE_FLAGS='${COMPILE_FLAGS}' MESH_LIB='${MESH_LIB}'

run: receptor
	./receptor

clean:
	rm -f receptor receptor.o
