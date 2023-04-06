PWD = $(shell pwd)
FOR_TESTING = -I${PWD}/../../common/test
COMMON_TEST_SRC = ${PWD}/../../common/test/test_common.c
override INCLUDE_FLAGS := ${INCLUDE_FLAGS} ${FOR_TESTING}

test: mesh auxiliary_initializer test_basic_meshlib.c
	${CC} ${INCLUDE_FLAGS} -o test_basic_meshlib.o test_basic_meshlib.c ${COMMON_TEST_SRC} ${MESH_LIB}
	./test_aux_initializer.o
	./test_basic_meshlib.o

auxiliary_initializer: mesh ../../common/test/test_aux_initializer.c
	${CC} ${INCLUDE_FLAGS} -o test_aux_initializer.o ../../common/test/test_aux_initializer.c ${COMMON_TEST_SRC} ${MESH_LIB}

mesh: 
	${MAKE} -C ../../mesh
