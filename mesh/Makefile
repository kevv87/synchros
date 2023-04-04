$CC = gcc
$RM = rm -rf
$AR = ar

all: mesh compile_static
LINKER_FLAGS += -lrt
COMPILER_FLAGS += -Werror

mesh: mesh.c
	${CC} ${COMPILER_FLAGS} -o mesh.o -c mesh.c -I./include ${LINKER_FLAGS}

test: test/test.c mesh compile_static
	${CC} ${COMPILER_FLAGS} -o test.o test/test.c -I./include -L./ -lmesh ${LINKER_FLAGS}
	./test.o

compile_static:
	${AR} rcs libmesh.a mesh.o

clean:
	${RM} *.o *.a

