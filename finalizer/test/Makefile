PWD = $(shell pwd)

FOR_TESTING = -I${PWD}/../../common/test
COMMON_TEST_SRC = ${PWD}/../../common/test/test_common.c

override COMPILE_FLAGS := ${COMPILE_FLAGS} ${FOR_TESTING}

test: mesh auxiliary_initializer auxiliary_emitter test_basic_meshlib.c
	${CC} ${COMPILE_FLAGS} -o test_basic_meshlib.o test_basic_meshlib.c ${COMMON_TEST_SRC} ${MESH_LIB}
	./test_aux_initializer.o
	./test_aux_emitter.o
	./test_basic_meshlib.o

auxiliary_initializer: mesh ../../common/test/test_aux_initializer.c
	${CC} ${COMPILE_FLAGS} -o test_aux_initializer.o ../../common/test/test_aux_initializer.c ${COMMON_TEST_SRC} ${MESH_LIB}

auxiliary_emitter: mesh ../../common/test/test_aux_emitter.c
	${CC} ${COMPILE_FLAGS} -o test_aux_emitter.o ../../common/test/test_aux_emitter.c ${COMMON_TEST_SRC} ${MESH_LIB}

mesh: 
	${MAKE} -C ../../mesh
	
