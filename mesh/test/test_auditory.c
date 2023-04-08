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
    wait_all_threads();
    auditory_info = get_auditory_info(shm_ptr);
    expect_equal(alive_emitters + 1, auditory_info->alive_emitters, "alive_emitters should be incremented\n");
    expect_equal(total_emitters + 1, auditory_info->total_emitters, "total_emitters should be incremented\n");
}

void test_on_receptor_registration_its_counters_should_increment() {
    printf("### test_on_receptor_registration_its_counters_should_increment\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int alive_receptors = auditory_info->alive_receptors;
    int total_receptors = auditory_info->total_receptors;
    mesh_register_receptor();
    wait_all_threads();
    auditory_info = get_auditory_info(shm_ptr);
    expect_equal(alive_receptors + 1, auditory_info->alive_receptors, "alive_receptors should be incremented\n");
    expect_equal(total_receptors + 1, auditory_info->total_receptors, "total_receptors should be incremented\n");
}

void test_on_emitter_finalization_alive_counter_should_decrement() {
    printf("### test_on_emitter_finalization_alive_counter_should_decrement\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int alive_emitters = auditory_info->alive_emitters;
    int total_emitters = auditory_info->total_emitters;
    mesh_register_emitter();
    mesh_finalize_emitter(shm_ptr);
    wait_all_threads();
    auditory_info = get_auditory_info(shm_ptr);
    expect_equal(alive_emitters, auditory_info->alive_emitters, "alive_emitters should be decremented\n");
    expect_equal(total_emitters+1, auditory_info->total_emitters, "total_emitters should be the same\n");
}

void test_on_receptor_finalization_alive_counter_should_decrement() {
    printf("### test_on_receptor_finalization_alive_counter_should_decrement\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int alive_receptors = auditory_info->alive_receptors;
    int total_receptors = auditory_info->total_receptors;
    mesh_register_receptor();
    mesh_finalize_receptor(shm_ptr);
    wait_all_threads();
    auditory_info = get_auditory_info(shm_ptr);
    expect_equal(alive_receptors, auditory_info->alive_receptors, "alive_receptors should be decremented\n");
    expect_equal(total_receptors+1, auditory_info->total_receptors, "total_receptors should be the same\n");
}

void test_on_writes_should_increment_characters_in_buffer() {
    printf("### test_on_writes_should_increment_characters_in_buffer\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int characters_in_buffer = auditory_info->characters_in_buffer;
    struct shm_caracter shm_caracter = { .value = 'a' };

    mesh_add_caracter(shm_ptr, shm_caracter);

    wait_all_threads();
    auditory_info = get_auditory_info(shm_ptr);
    expect_equal(characters_in_buffer + 1, auditory_info->characters_in_buffer, "characters_in_buffer should be incremented\n");
}

void test_on_reads_should_decrement_characters_in_buffer() {
    printf("### test_on_reads_should_decrement_characters_in_buffer\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int characters_in_buffer = auditory_info->characters_in_buffer;
    struct shm_caracter shm_caracter = { .value = 'a' };

    mesh_add_caracter(shm_ptr, shm_caracter);
    mesh_get_caracter(shm_ptr);

    wait_all_threads();
    auditory_info = get_auditory_info(shm_ptr);
    expect_equal(characters_in_buffer, auditory_info->characters_in_buffer, "characters_in_buffer should be decremented\n");
}

void test_on_reads_should_increment_transferred_characters() {
    printf("### test_on_reads_should_increment_transferred_characters\n");
    struct auditory_info * auditory_info = get_auditory_info(shm_ptr);
    int transferred_characters = auditory_info->transferred_characters;
    struct shm_caracter shm_caracter = { .value = 'a' };

    mesh_add_caracter(shm_ptr, shm_caracter);
    mesh_get_caracter(shm_ptr);

    wait_all_threads();
    auditory_info = get_auditory_info(shm_ptr);
    expect_equal(transferred_characters + 1, auditory_info->transferred_characters, "transferred_characters should be incremented\n");
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
    call_test(test_on_receptor_registration_its_counters_should_increment);
    call_test(test_on_emitter_finalization_alive_counter_should_decrement);
    call_test(test_on_receptor_finalization_alive_counter_should_decrement);
    call_test(test_on_writes_should_increment_characters_in_buffer);
    call_test(test_on_reads_should_decrement_characters_in_buffer);
    call_test(test_on_reads_should_increment_transferred_characters);

    last_teardown();
    printf("\n### Auditory tests finished\n");
}
