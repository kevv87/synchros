#include "test_common.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "mesh.h"

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

void test_should_give_a_size_to_shmem() {
    printf("### test_should_give_a_size_to_shmem\n");
    int shm_fd = open_shared_memory(TEST_SHM_NAME);
    
    int errcode = give_size_to_shmem(shm_fd, sizeof(int));

    assertm(errcode >=0, "Couldnt give a size to shared memory");
}

void test_should_retrieve_ptr_from_shmem() {
    printf("### test_should_retrieve_ptr_from_shmem\n");
    int shm_fd = open_shared_memory(TEST_SHM_NAME);
    give_size_to_shmem(shm_fd, sizeof(int));

    void *shm_ptr = get_ptr_to_shared_memory(shm_fd, sizeof(int));

    int err_code = *((int *) shm_ptr);
    assertm(err_code >= 0, "Error obtaining the shared memory pointer");
    munmap(shm_ptr, sizeof(int));
}

void test_should_write_various_structures_in_sh_mem() {
    printf("### test_should_write_various_structures_in_sh_mem\n");
    int shm_fd = open_shared_memory(TEST_SHM_NAME);

    struct st1 {
        int value1;
        char value2;
        int value3;
    };
    
    struct st2 {
        int value1;
        int value2;
        int value3;
    };

    struct st3 {
        char value1;
    };

    struct st1 st1 = {1, 'a', 2};
    struct st2 st2 = {3, 4, 5};
    struct st3 st3 = {'b'};

    size_t sizes = sizeof(st1) + sizeof(st2) + sizeof(st3);

    give_size_to_shmem(shm_fd, sizes);
    void *shm_ptr = get_ptr_to_shared_memory(shm_fd, sizes);

    memcpy(shm_ptr, &st1, sizeof(st1));
    memcpy(shm_ptr + sizeof(st1), &st2, sizeof(st2));
    memcpy(shm_ptr + sizeof(st1) + sizeof(st2), &st3, sizeof(st3));

    munmap(shm_ptr, sizes);
    close_shared_memory(TEST_SHM_NAME);
}

void teardown() {
    close_shared_memory(TEST_SHM_NAME);
    return;
}

int main() {
    printf("### Running tests!\n\n");

    call_test_teardown(&test_shmget_should_return_fd, &teardown);
    call_test_teardown(&test_shmem_should_close, &teardown);
    call_test_teardown(&test_should_give_a_size_to_shmem, &teardown);
    call_test_teardown(&test_should_retrieve_ptr_from_shmem, &teardown);
    call_test_teardown(&test_should_write_various_structures_in_sh_mem, &teardown);

    printf("\n### All tests were succesfull!\n");
    return 0;
}