#include "test_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "common/structures.c"

#include "mesh_receptor.h"

void * shm_ptr;
int buffer_size;

void first_setup() {
    buffer_size = 13;
    shm_ptr = mesh_register_receptor();
}

void test_receptor_should_register() {
    printf("### test_receptor_should_register\n");
    shm_ptr = mesh_register_receptor();
}

void test_receptor_should_read_caracter_from_shm_buffer() {
    printf("### test_receptor_should_read_caracter_from_shm_buffer\n");
    struct shm_caracter caracter = mesh_get_caracter(shm_ptr);

    assertm(caracter.buffer_idx == 0, "Caracter does not have expected idx\n");
}

void test_receptor_should_get_output_file_idx_counter() {
    printf("### test_receptor_should_get_output_file_idx_counter\n");
    int idx = mesh_get_output_file_idx(shm_ptr);

    assertm(idx == 0, "First index to read should be zero\n");
    idx = mesh_get_output_file_idx(shm_ptr);
    assertm(idx == 1, "Second index to read should be one\n");
}

void test_when_file_idx_has_reached_file_length_should_return_minus_one() {
    printf("### test_when_file_idx_has_reached_file_length_should_return_minus_one\n");
    int idx = mesh_get_output_file_idx(shm_ptr);
    for (int i = 0; i < TEST_FILE_LENGTH; i++) {
        idx = mesh_get_output_file_idx(shm_ptr);
    }
    idx = mesh_get_output_file_idx(shm_ptr);
    assertm(idx == -1, "Should return -1 when file idx has reached file length\n");
}

void test_receptor_should_finalize() {
    printf("### test_receptor_should_finalize\n");
    mesh_finalize_receptor(shm_ptr);
}

int main () {
    printf("\n### Running basic receptor meshlib tests!\n\n");    

    call_test(&test_receptor_should_register);
    call_test(&test_receptor_should_read_caracter_from_shm_buffer);
    call_test(&test_receptor_should_get_output_file_idx_counter);
    call_test(&test_when_file_idx_has_reached_file_length_should_return_minus_one);
    call_test(&test_receptor_should_finalize);

    printf("\n### Finished basic receptor meshlib tests!\n\n");
    return 0;
}
