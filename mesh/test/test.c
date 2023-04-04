#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#define assertm(exp, msg) assert(((void)msg, exp))

#include "mesh.h"
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define TEST_SHM_NAME "/test_mem"

void test_shmget_should_return_fd() {
    printf("### test_shmget_should_return_0\n");
    int shm_fd = open_shared_memory(TEST_SHM_NAME);
    assertm(shm_fd>=0, "Reserve memory found an error");
}

void test_shmem_should_close() {
    printf("### test_shmem_should_close\n");
    open_shared_memory(TEST_SHM_NAME);

    int res = close_shared_memory(TEST_SHM_NAME);
    assertm(res==0, "Closing shared memory found an error");

    int reopening_errcode = shm_open(TEST_SHM_NAME, O_RDWR, 0666);
    assertm(reopening_errcode<0, "Shared memory could still be openned after supposedly closing it");
}

void test_should_retrieve_ptr_from_shmem() {
    printf("### test_should_retrieve_ptr_from_shmem");
    int shm_fd = open_shared_memory(TEST_SHM_NAME);
    void *shm_ptr = get_ptr_to_shared_memory(shm_fd);
    int err_code = *((int *) shm_ptr);
    assertm(err_code >= 0, "Error obtaining the shared memory pointer");
    free(shm_ptr);
}

void setup() {
    return;
}

void teardown() {
    close_shared_memory(TEST_SHM_NAME);
    return;
}

void call_test(void (*test_function)() ) {
    setup();
    test_function();
    teardown();
}

int main() {
    printf("### Running tests!\n\n");

    call_test(&test_shmget_should_return_fd);
    call_test(&test_shmem_should_close);
    call_test(&test_should_retrieve_ptr_from_shmem);

    printf("\n### All tests were succesfull!\n");
    return 0;
}