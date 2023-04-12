#ifndef DEBUGGING
#define DEBUGGING
#include <stdio.h>

void dump_bytes(void *ptr, size_t num_bytes) {
    unsigned char *bytes = (unsigned char *) ptr;
    printf("Dumping %lu bytes from memory address %p:\n", num_bytes, ptr);
    for (size_t i = 0; i < num_bytes; i++) {
        if (i % 16 == 0) { // print address every 16 bytes
            printf("0x%08lx: ", (unsigned long) (bytes + i));
        }
        printf("%02x ", bytes[i]);
        if ((i + 1) % 16 == 0) { // jump to new line every 16 bytes
            printf("\n");
        }
    }
    if (num_bytes % 16 != 0) { // print final newline if necessary
        printf("\n");
    }
}

#endif
