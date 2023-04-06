#include "test_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "common/structures.c"

#include "mesh_emitter.h"

void * shm_ptr;
int buffer_size;

void first_setup() {
    buffer_size = 13;
    //mesh_initialize(buffer_size);
    shm_ptr = mesh_get_shm_ptr();
}

void last_teardown() {
    mesh_finalize(shm_ptr);
}

int main () {
    printf("### Running basic emitter meshlib tests!\n\n");    
    first_setup();

    

    last_teardown();
    return 0;
}
