#include "test_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "common/structures.c"

#include "mesh.h"

void * shm_ptr;
int buffer_size;

void test_auditory_should_be_initialized_on_startup() {
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    expect_equal(0, auditory_info->transferred_characters, "transferred_characters should be 0\n");
    expect_equal(0, auditory_info->characters_in_buffer, "characters_in_buffer should be 0\n");
    expect_equal(0, auditory_info->alive_emitters, "alive_emitters should be 0\n");
    expect_equal(0, auditory_info->total_emitters, "total_emitters should be 0\n");
    expect_equal(0, auditory_info->alive_receptors, "alive_receptors should be 0\n");
    expect_equal(0, auditory_info->total_receptors, "total_receptors should be 0\n");
}

void test_auditory_shm_used_should_be_initialized_on_startup() {
    printf("### test_auditory_shm_used_should_be_initialized_on_startup\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int size_expected = 
        sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct auditory_info) + sizeof(struct shm_caracter) * buffer_size;
    expect_equal(size_expected, auditory_info->shm_mem_used, "shm_mem_used should be 0\n");
}

void test_on_emitter_registration_its_counters_should_increment() {
    printf("### test_on_emitter_registration_its_counters_should_increment\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int alive_emitters = auditory_info->alive_emitters;
    int total_emitters = auditory_info->total_emitters;
    mesh_register_emitter();
    auditory_info = get_auditory_info(shm_ptr);
    wait_all_threads();
    expect_equal(alive_emitters + 1, auditory_info->alive_emitters, "alive_emitters should be incremented\n");
    expect_equal(total_emitters + 1, auditory_info->total_emitters, "total_emitters should be incremented\n");
}

void first_setup() {
    buffer_size = TEST_BUFFER_SIZE;
    shm_ptr = mesh_initialize(buffer_size);
    unmap_shared_memory(shm_ptr);

    shm_ptr = mesh_get_shm_ptr();
}

void last_teardown() {
    mesh_finalize(shm_ptr);
}

int main() {
    printf("### Starting test_auditory ###\n\n");
    first_setup();

    call_test(test_auditory_should_be_initialized_on_startup);
    call_test(test_auditory_shm_used_should_be_initialized_on_startup);
    call_test(test_on_emitter_registration_its_counters_should_increment);

    last_teardown();
    printf("\n### Auditory tests finished\n");
}
