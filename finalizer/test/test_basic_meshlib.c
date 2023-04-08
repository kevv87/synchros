#include "test_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "common/structures.c"

#include "mesh_finalizer.h"

void * shm_ptr;
int buffer_size;

void first_setup() {
    buffer_size = TEST_BUFFER_SIZE;
    shm_ptr = mesh_get_shm_ptr();
}

void test_finalizer_should_finalize_and_obtain_auditory_info() {
    printf("### test_finalizer_should_finalize_and_obtain_auditory_info\n");
    struct auditory_info auditory_info = mesh_finalize(shm_ptr);
}

int main() {
    printf("### Running bkill_heartbeatasic finalizer meshlib tests\n\n");
    first_setup();

    call_test(test_finalizer_should_finalize_and_obtain_auditory_info);

    printf("\n### Finished basic finalizer meshlib tests\n\n");
    return 0;
}
