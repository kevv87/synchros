$CC = gcc
$RM = rm -rf
$AR = ar

all: helloworld compile_static

helloworld: helloworld.c
	${CC} -c helloworld.c -I./include

compile_static:
	${AR} rcs libmesh.a *.o

clean:
	${RM} *.o *.a

