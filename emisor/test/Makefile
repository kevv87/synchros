PWD = $(shell pwd)
FOR_TESTING = -I${PWD}/../../common/test
override INCLUDE_FLAGS := ${INCLUDE_FLAGS} ${FOR_TESTING}

test: mesh test_basic_meshlib.c
	${CC} ${INCLUDE_FLAGS} -o test_basic_meshlib.o test_basic_meshlib.c ${MESH_LIB}
	./test_basic_meshlib.o

mesh: 
	${MAKE} -C ../../mesh
