CC = gcc
INCLUDE_FLAGS = -I../mesh/include
RM = rm -rf
PWD = $(shell pwd)

INCLUDE_FLAGS = -I${PWD}/../ -I${PWD}/../mesh/include
COMPILE_FLAGS = -Werror ${INCLUDE_FLAGS}
MESH_LIB = -L${PWD}/../mesh -lmesh -lpthread

.PHONY: all

all: receptor

receptor: receptor.o
	$(CC) $(COMPILE_FLAGS) $(INCLUDE_FLAGS) -o receptor.o receptor.c $(MESH_LIB)


receptor.o: receptor.c
	$(CC) $(CFLAGS) -c receptor.c

test_receptor:
	${MAKE} -C ./test COMPILE_FLAGS='${COMPILE_FLAGS}' MESH_LIB='${MESH_LIB}'

run: receptor
	./receptor.o

clean:
	rm -f receptor receptor.o
