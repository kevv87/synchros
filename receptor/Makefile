CC=gcc
CFLAGS=-Wall 

.PHONY: all

all: receptor run

receptor: receptor.o
	$(CC) $(CFLAGS) receptor.o -o receptor

receptor.o: receptor.c
	$(CC) $(CFLAGS) -c receptor.c

run: receptor
	./receptor

clean:
	rm -f receptor receptor.o