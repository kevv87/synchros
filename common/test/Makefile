CFLAGS = -I../../mesh/include -I../../ 
LDFLAGS = -L../../mesh/ -lmesh

initializer: test_aux_initializer.c
	$(CC) $(CFLAGS) -o test_aux_initializer.o test_aux_initializer.c $(LDFLAGS)
	./test_aux_initializer.o

finalizer: test_aux_finalizer.c
	$(CC) $(CFLAGS) -o test_aux_finalizer.o test_aux_finalizer.c $(LDFLAGS)
	./test_aux_finalizer.o

clean:
	rm -f *.o

