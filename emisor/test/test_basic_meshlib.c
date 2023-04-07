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
    shm_ptr = mesh_register_emitter();
}

void test_emitter_should_register() {
    void *shm_ptr = mesh_register_emitter();
}

void test_emitter_should_get_file_idx_counter() {
    int file_idx = mesh_get_file_idx(shm_ptr);
    expect_equal(0, file_idx, "Should return 0 at start\n");
    file_idx = mesh_get_file_idx(shm_ptr);
    expect_equal(1, file_idx, "Should return next index to read\n");
}

void test_emitter_should_write_caracter_to_shm_buffer() {
    struct shm_caracter caracter;
    caracter.value = 'a';
    mesh_add_caracter(shm_ptr, caracter);
}

void test_when_file_idx_has_reached_file_length_should_return_minus_one() {
    printf("### test_when_file_idx_has_reached_file_length_should_return_minus_one\n");
    int idx = mesh_get_file_idx(shm_ptr);
    for (int i = 0; i < TEST_FILE_LENGTH; i++) {
        idx = mesh_get_file_idx(shm_ptr);
    }
    idx = mesh_get_file_idx(shm_ptr);
    assertm(idx == -1, "Should return -1 when file idx has reached file length\n");
}

int main () {
    printf("\n### Running basic emitter meshlib tests!\n\n");    
    first_setup();

    call_test(&test_emitter_should_register);
    call_test(&test_emitter_should_get_file_idx_counter);
    call_test(&test_emitter_should_write_caracter_to_shm_buffer);
    call_test(&test_when_file_idx_has_reached_file_length_should_return_minus_one);

    printf("### Finished basic emitter meshlib tests!\n\n");
    return 0;
}
