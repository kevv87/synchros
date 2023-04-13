CFLAGS = -I../../mesh/include -I../../ 
LDFLAGS = -L../../mesh/ -lmesh -lpthread

initializer: test_aux_initializer.c
	$(CC) $(CFLAGS) -o test_aux_initializer.o test_aux_initializer.c $(LDFLAGS)
	./test_aux_initializer.o

emitter: test_aux_emitter.c
	$(CC) $(CFLAGS) -o test_aux_emitter.o test_aux_emitter.c $(LDFLAGS)
	./test_aux_emitter.o

stuck_emitter: test_aux_stuck_emitter.c
	$(CC) $(CFLAGS) -o test_aux_stuck_emitter.o test_aux_stuck_emitter.c $(LDFLAGS)
	./test_aux_stuck_emitter.o

finalizer: test_aux_finalizer.c
	$(CC) $(CFLAGS) -o test_aux_finalizer.o test_aux_finalizer.c $(LDFLAGS)
	./test_aux_finalizer.o

clean:
	rm -f *.o

