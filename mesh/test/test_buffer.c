#include "test_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

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

void test_initialize_should_create_emitter_semaphore() {
    printf("### test_initialize_should_create_emitter_semaphore\n");
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(emitter_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(buffer_size, sem_value, "Semaphore value is not expected\n");
}

void test_initialize_should_create_receptor_semaphore() {

}

void test_initialize_should_create_buffer_idx_semaphore() {

}

void test_should_add_new_caracter_to_buffer() {
//    printf("### test_should_add_new_caracter_to_buffer\n");
//    struct shm_caracter caracter = {
//        .value= 'a',
//        .datetime = {},
//        .buffer_idx = 0,
//        .file_idx = 1
//    };
//
//    int add_caracter = mesh_add_caracter(shm_ptr, caracter);
//    assertm(add_caracter == 0, "Caracter was not added to buffer!");
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

    //call_test(&test_initialize_should_save_buffer_size_in_shmem);
    //call_test(&test_initialize_should_write_buffer_in_shmem);
    call_test(&test_initialize_should_create_emitter_semaphore);

    last_teardown();
    printf("\n### All tests were succesfull!\n");
    return 0;
}
