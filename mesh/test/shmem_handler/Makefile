LINKER_FLAGS += -lrt -L./ -ltesting
FOR_TESTS = ../../../common/test/test_common.c -I../../../common/test ${LINKER_FLAGS}
INCLUDE_FLAGS = -I../../include
COMPILER_FLAGS += ${INCLUDE_FLAGS}

test_shmem_handler: compile_shmem_handler test_writer test_reader clean

compile_shmem_handler:
	${CC} ${COMPILER_FLAGS} -o test_c_shmem_handler.o -c ../../shmem_handler.c
	${AR} rcs libtesting.a test_c_shmem_handler.o

test_writer: test_writer.c
	${CC} ${COMPILER_FLAGS} -o test_writer.o test_writer.c ${FOR_TESTS}
	./test_writer.o

test_reader: test_reader.c
	${CC} ${COMPILER_FLAGS} -o test_reader.o test_reader.c ${FOR_TESTS}
	./test_reader.o

clean:
	rm -rf *.o *.a
