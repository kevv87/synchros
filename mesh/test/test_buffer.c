#include "test_common.h"

#include <stdio.h>

#include "common/structures.c"

#include "mesh.h"

void * shm_ptr;
int buffer_size;

void test_initialize_should_save_buffer_size_in_shmem() {
    printf("### test_initialize_should_save_buffer_size_in_shmem\n");

    struct shm_context context = get_shm_context(shm_ptr);
    
    assertm(context.size_of_buffer == buffer_size, "Buffer size is not expected!");
}

void test_initialize_should_write_buffer_in_shmem() {
    printf("### test_should_write_buffer_in_shmem\n");
    struct shm_caracter *buffer = get_buffer(shm_ptr);
    
    for (int i = 0; i < buffer_size; i++) {
        assertm(buffer[i].buffer_idx == i, 
                "Buffer is not initialized correctly, index %d is wrong\n", i);
    }
}

void first_setup() {
    buffer_size = 13;
    shm_ptr = mesh_initialize(buffer_size);
}

void last_teardown() {
    mesh_finalize(shm_ptr);
}

int main() {
    printf("### Running buffer tests!\n\n");    
    first_setup();

    call_test(&test_initialize_should_save_buffer_size_in_shmem);
    call_test(&test_initialize_should_write_buffer_in_shmem);

    last_teardown();
    printf("\n### All tests were succesfull!\n");
    return 0;
}
