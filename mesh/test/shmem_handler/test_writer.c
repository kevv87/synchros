#include "test_common.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "shmem_handler.h"

void test_shmget_should_return_fd() {
    printf("### test_shmget_should_return_0\n");
    int shm_fd = open_shared_memory(TEST_SHM_NAME);
    assertm(shm_fd>=0, "Reserve memory found an error");
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
    size_t size = sizeof(int);
    give_size_to_shmem(shm_fd, size);

    void *shm_ptr = get_ptr_to_shared_memory(shm_fd, size);

    int err_code = *((int *) shm_ptr);
    assertm(err_code >= 0, "Error obtaining the shared memory pointer");

    shmem_close_shared_memory(TEST_SHM_NAME, shm_ptr, size);
}

void test_shmem_should_close() {
    printf("### test_shmem_should_close\n");
    int shm_fd = open_shared_memory(TEST_SHM_NAME);
    size_t size = sizeof(int);
    give_size_to_shmem(shm_fd, size);

    void *shm_ptr = get_ptr_to_shared_memory(shm_fd, size);

    int res = shmem_close_shared_memory(TEST_SHM_NAME, shm_ptr, size);
    assertm(res==0, "Closing shared memory found an error");

    int reopening_errcode = shm_open(TEST_SHM_NAME, O_RDWR, 0666);
    assertm(reopening_errcode<0, "Shared memory could still be openned after supposedly closing it");
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

    size_t sizes = sizeof(struct st1) + sizeof(struct st2) + sizeof(struct st3);

    give_size_to_shmem(shm_fd, sizes);
    void *shm_ptr = get_ptr_to_shared_memory(shm_fd, sizes);

    memcpy(shm_ptr, &st1, sizeof(st1));
    memcpy(shm_ptr + sizeof(st1), &st2, sizeof(st2));
    memcpy(shm_ptr + sizeof(st1) + sizeof(st2), &st3, sizeof(st3));
}

void teardown() {
    return;
}

int main() {
    printf("### Running writer tests!\n\n");

    call_test(&test_shmget_should_return_fd);
    call_test(&test_shmem_should_close);
    call_test(&test_should_give_a_size_to_shmem);
    call_test(&test_should_retrieve_ptr_from_shmem);
    call_test(&test_should_write_various_structures_in_sh_mem);

    printf("\n### All tests were succesfull!\n");
    return 0;
}