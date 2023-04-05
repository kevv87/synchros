#include "test_common.h"

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include "shmem_handler.h"

void test_should_read_wrote_structures() {
    printf("### test_should_read_wrote_structures\n");
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

    size_t sizes = sizeof(struct st1) + sizeof(struct st2) + sizeof(struct st3);

    struct st1 *st1 = malloc(sizeof(struct st1));
    struct st2 *st2;
    struct st3 *st3;

    give_size_to_shmem(shm_fd, sizes);
    void *shm_ptr = get_ptr_to_shared_memory(shm_fd, sizes);

    st1 = shm_ptr;
    st2 = shm_ptr + sizeof(struct st1);
    st3 = shm_ptr + sizeof(struct st1) + sizeof(struct st2);

    assertm(st1->value1 == 1,   "Memory read 1 was not expected");
    assertm(st1->value2 == 'a', "Memory read 2 was not expected");
    assertm(st1->value3 == 2,   "Memory read 3 was not expected");
    
    assertm(st2->value1 == 3,   "Memory read 1 was not expected");
    assertm(st2->value2 == 4,   "Memory read 2 was not expected");
    assertm(st2->value3 == 5,   "Memory read 2 was not expected");
    
    assertm(st3->value1 == 'b',   "Memory read 2 was not expected");

    munmap(shm_ptr, sizes);
}

int main() {
    printf("### Running reader tests!\n\n");

    call_test(&test_should_read_wrote_structures);

    printf("\n### All tests were succesfull!\n");
    return 0;
}