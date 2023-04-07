#include "test_common.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "shmem_handler.h"

void test_shmget_should_return_id() {
    printf("### test_shmget_should_return_0\n");
    int shm_id = open_shared_memory(PROJECT_ID, sizeof(int));
    assertm(shm_id>=0, "Reserve memory found an error");

    shmem_close_shared_memory(shm_id);
}

void test_should_retrieve_ptr_from_shmem() {
    printf("### test_should_retrieve_ptr_from_shmem\n");
    size_t size = sizeof(int);
    int shm_id = open_shared_memory(PROJECT_ID, size);

    void *shm_ptr = get_ptr_to_shared_memory(shm_id, size);

    int err_code = *((int *) shm_ptr);
    assertm(err_code >= 0, "Error obtaining the shared memory pointer");

    shmem_close_shared_memory(shm_id);
}

void test_should_write_various_structures_in_sh_mem() {
    printf("### test_should_write_various_structures_in_sh_mem\n");

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
    int shm_fd = open_shared_memory(PROJECT_ID, sizes);

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

    call_test(&test_shmget_should_return_id);
    call_test(&test_should_retrieve_ptr_from_shmem);
    call_test(&test_should_write_various_structures_in_sh_mem);

    printf("\n### All tests were succesfull!\n");
    return 0;
}
