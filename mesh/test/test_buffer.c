#include "test_common.h"

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>

#include "common/structures.c"

#include "mesh.h"

void * shm_ptr;
int buffer_size;

void test_initialize_should_start_heartbeat() {
    printf("### test_initialize_should_start_heartbeat\n");
    struct shm_context *context = get_shm_context(shm_ptr);
    assertm(context->heartbeat == 1, "Heartbeat is not started!");
}

void test_initialize_should_save_buffer_size_in_shmem() {
    printf("### test_initialize_should_save_buffer_size_in_shmem\n");

    struct shm_context *context = get_shm_context(shm_ptr);
    assertm(context->size_of_buffer == buffer_size, "Buffer size is not expected!");
}

void test_initialize_should_write_buffer_in_shmem() {
    printf("### test_should_write_buffer_in_shmem\n");
    struct shm_caracter *buffer = get_buffer(shm_ptr);
    
    printf("Buffer address: %p\n", buffer);
    for (int i = 0; i < buffer_size; i++) {
        printf("TEST: Buffer[%d] is in %p\n", i, &buffer[i]);
        printf("TEST: Buffer[%d].buffer_idx = %d\n", i, buffer[i].buffer_idx);
        expect_equal(i, buffer[i].buffer_idx, 
                "Buffer is not initialized correctly, index %d is wrong\n", i);
    }
}

void test_initialize_should_create_emitter_semaphore() {
    printf("### test_initialize_should_create_emitter_semaphore\n");
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(emitter_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(buffer_size, sem_value, "Semaphore value is not expected\n");
}

void test_initialize_should_create_receptor_semaphore() {
    printf("### test_initialize_should_create_receptor_semaphore\n");
    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(receptor_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(0, sem_value, "Semaphore value is not expected\n");
}

void test_initialize_should_create_buffer_idx_semaphore() {
    printf("### test_initialize_should_create_buffer_idx_semaphore\n");
    sem_t *buffer_idx_sem = mesh_get_buffer_idx_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(buffer_idx_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(1, sem_value, "Semaphore value is not expected\n");
}

void test_initialize_should_initialize_buffer_counter_to_zero() {
    printf("### test_initialize_should_initialize_buffer_counter_to_zero\n");
    struct shm_context *context = get_shm_context(shm_ptr);
    expect_equal(context->buffer_counter, 0, "Buffer counter is not zero\n");
}

void test_should_add_new_caracters_to_buffer() {
    printf("### test_should_add_new_caracters_to_buffer\n");
    struct shm_caracter caracter = {
        .value= 'a',
        .datetime = {},
        .buffer_idx = 0,
        .file_idx = 1
    };
 
    for (int i = 0; i < buffer_size; i++) {
        caracter.value = 'a' + i;
        int add_caracter = mesh_add_caracter(shm_ptr, caracter);
        assertm(add_caracter == 0, "Caracter was not added to buffer!");

        struct shm_caracter *buffer = get_buffer(shm_ptr);
        assertm(
            buffer[i].value == 'a' + i,
            "Caracter was not added to buffer! Index %d failed", i);
        assertm(
            buffer[i].buffer_idx == i,
            "Caracter was added with wrong index to buffer! Index %d failed", i);
    }
}

void test_writing_buffer_should_decrement_emitter_semaphore() {
    printf("### test_writing_buffer_should_decrement_emitter_semaphore\n");
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(emitter_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(buffer_size, sem_value, "Semaphore value is not expected\n");

    struct shm_caracter caracter = {
        .value= 'a',
        .datetime = {},
        .buffer_idx = 0,
        .file_idx = 1
    };
    mesh_add_caracter(shm_ptr, caracter);

    expect_equal(sem_getvalue(emitter_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(buffer_size - 1, sem_value, "Semaphore value was not lowered\n");
}

void test_writing_buffer_should_increment_receptor_semaphore() {
    printf("### test_writing_buffer_should_decrement_receptor_semaphore\n");
    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(receptor_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(0, sem_value, "Semaphore value is not expected\n");

    struct shm_caracter caracter = {
        .value= 'a',
        .datetime = {},
        .buffer_idx = 0,
        .file_idx = 1
    };
    mesh_add_caracter(shm_ptr, caracter);

    expect_equal(sem_getvalue(receptor_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(1, sem_value, "Semaphore value was not incremented\n");
}

void test_buffer_writes_should_be_circular() {
    printf("### test_buffer_writes_should_be_circular\n");
    struct shm_caracter caracter = {
        .value= 'a',
        .datetime = {},
        .buffer_idx = 0,
        .file_idx = 1
    };
    for (int i = 0; i < buffer_size; i++) {
        caracter.value = 'a' + i;
        mesh_add_caracter(shm_ptr, caracter);
    }

    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    sem_post(emitter_sem);

    caracter.value = 'a' + buffer_size+1;
    mesh_add_caracter(shm_ptr, caracter);

    struct shm_caracter *buffer = get_buffer(shm_ptr);
    assertm(
        buffer[0].value == ('a' + buffer_size+1),
        "Index buffer_size + 1 should write to index 0, so the buffer isnt displaying circularity");
}

void test_should_read_caracters_from_buffer() {
    printf("### test_should_read_caracters_from_buffer\n");
    struct shm_caracter caracter;
    
    struct shm_caracter *buffer = get_buffer(shm_ptr);
    for (int i = 0; i < buffer_size; i++) {
        caracter = buffer[i];
        assertm(
            caracter.value == 'a' + i,
            "Caracter was not added to buffer! Index %d failed", i);
        assertm(
            buffer[i].buffer_idx == i,
            "Caracter was added with wrong index to buffer! Index %d failed", i);
    }
}

void test_reading_should_be_circular() {
    printf("### test_reading_should_be_circular\n");
    struct shm_caracter caracter;

    for (int i = 0; i < buffer_size; i++) {
        caracter = mesh_get_caracter(shm_ptr);
        assertm(
            caracter.value == 'a' + i,
            "Caracter was not added to buffer! Index %d failed", i);
        assertm(
            caracter.buffer_idx == i,
            "Caracter was added with wrong index to buffer! Index %d failed", i);
    }

    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);
    sem_post(receptor_sem);

    caracter = mesh_get_caracter(shm_ptr);
    assertm(
        caracter.buffer_idx == 0,
        "Index buffer_size + 1 should be index 0, so the buffer isnt displaying circularity\n");
}

void test_reading_should_decrement_receptor_semaphore() {
    printf("### test_reading_should_decrement_receptor_semaphore\n");
    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(receptor_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(buffer_size, sem_value, "Semaphore value is not expected\n");

    mesh_get_caracter(shm_ptr);

    expect_equal(sem_getvalue(receptor_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(buffer_size - 1, sem_value, "Semaphore value was not lowered\n");
}

void test_reading_should_increment_emitter_semaphore() {
    printf("### test_reading_should_increment_emitter_semaphore\n");
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    int sem_value;
    expect_equal(sem_getvalue(emitter_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(0, sem_value, "Semaphore value is not expected\n");

    mesh_get_caracter(shm_ptr);

    expect_equal(sem_getvalue(emitter_sem, &sem_value), 0, "Error getting semaphore value\n");
    expect_equal(1, sem_value, "Semaphore value was not incremented\n");
}

void first_setup() {
    buffer_size = TEST_BUFFER_SIZE;
    shm_ptr = mesh_initialize(buffer_size);
    while(1);
    unmap_shared_memory(shm_ptr);

    shm_ptr = mesh_get_shm_ptr();
}

void last_teardown() {
    mesh_finalize(shm_ptr);
}

void reinit() {
    mesh_finalize(shm_ptr);
    shm_ptr = mesh_initialize(buffer_size);
}

void fill_buffer() {
    struct shm_caracter caracter = {
        .value= 'a',
        .datetime = {},
        .buffer_idx = 0,
        .file_idx = 1
    };
    for (int i = 0; i < buffer_size; i++) {
        caracter.value = 'a' + i;
        mesh_add_caracter(shm_ptr, caracter);
    }
}

int main() {
    printf("### Running buffer tests!\n\n");    
    first_setup();

    call_test(&test_initialize_should_start_heartbeat);
    call_test(&test_initialize_should_save_buffer_size_in_shmem);
    call_test(&test_initialize_should_write_buffer_in_shmem);
    call_test(&test_initialize_should_create_emitter_semaphore);
    call_test(&test_initialize_should_create_receptor_semaphore);
    call_test(&test_initialize_should_create_buffer_idx_semaphore);
    call_test(&test_initialize_should_initialize_buffer_counter_to_zero);

    call_test_teardown(&test_should_add_new_caracters_to_buffer, &reinit);
    call_test_teardown(&test_writing_buffer_should_decrement_emitter_semaphore, &reinit);
    call_test_teardown(&test_writing_buffer_should_increment_receptor_semaphore, &reinit);
    call_test_teardown(&test_buffer_writes_should_be_circular, &reinit);

    call_test_setup_teardown(&test_should_read_caracters_from_buffer, &fill_buffer, &reinit);
    call_test_setup_teardown(&test_reading_should_be_circular, &fill_buffer, &reinit);
    call_test_setup_teardown(&test_reading_should_decrement_receptor_semaphore, &fill_buffer, &reinit);
    call_test_setup_teardown(&test_reading_should_increment_emitter_semaphore, &fill_buffer, &reinit);

    last_teardown();
    printf("\n### All tests were succesfull!\n");
    return 0;
}
