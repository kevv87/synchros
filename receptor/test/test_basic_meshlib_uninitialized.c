#include "test_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "common/structures.c"

#include "mesh_receptor.h"

void * shm_ptr;
int buffer_size;

void test_when_shm_was_not_initialized_should_throw_error() {
    printf("### Running test_when_shm_was_not_initialized_should_throw_error\n");
    printf("Don't panic, next print should be error\n");
    void *shm_ptr = mesh_register_receptor();
    expect_equal(*((int *)shm_ptr), -1, "Should return -1 when shm was not initialized\n");
}

int main () {
    printf("\n### Running basic receptor meshlib without initializing tests!\n\n");    

    call_test(test_when_shm_was_not_initialized_should_throw_error);

    printf("### Finished basic receptor meshlib without initializing tests!\n\n");

    return 0;
}
