#include "mesh.h"
#include "mesh_emitter.h"
#include "mesh_initializer.h"

#define SHM_NAME "/shared_mem"

#include <string.h>
#include <stdio.h>
#include <semaphore.h>

#include "shmem_handler.h"
#include "common/structures.c"
#include "common/debugging.c"

#define TEST_FILE_LENGTH 20

struct mesh_semaphores {
    sem_t emitter_sem;
    sem_t receptor_sem;
    sem_t buffer_idx_sem;
    sem_t file_idx_sem;
    sem_t output_file_idx_sem;
    sem_t read_buffer_idx_sem;
};

struct shm_context *get_shm_context(void *shm_ptr) {
    return (struct shm_context *) shm_ptr;
}

void *initialize_shared_memory(size_t size) {
    int shm_id = open_shared_memory(PROJECT_ID, size);
    void *shm_ptr = get_ptr_to_shared_memory(shm_id, size);
    get_shm_context(shm_ptr)->shm_id = shm_id;
    return shm_ptr;
}

void unmap_shared_memory(void *shm_ptr){
    size_t size = sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct shm_caracter) * get_shm_context(shm_ptr)->size_of_buffer;
    shm_unmap(shm_ptr, size);
}

void *mesh_get_shm_ptr() {
    int initial_shm_size = sizeof(struct shm_context) + sizeof(struct mesh_semaphores);
    void *shm_ptr = initialize_shared_memory(initial_shm_size);

    struct shm_context *context = get_shm_context(shm_ptr);
    int real_shm_size = 
        sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct shm_caracter) * context->size_of_buffer;
    shm_unmap(shm_ptr, initial_shm_size);

    shm_ptr = initialize_shared_memory(real_shm_size);
    return shm_ptr;
}

void *mesh_register_emitter() {
    void *shm_ptr = mesh_get_shm_ptr();
    struct shm_context *context = get_shm_context(shm_ptr);
    if (context->heartbeat == 0) {
        printf("Error! Heartbeat is 0, mesh was not initialized!\n");
        int *errcode = malloc(sizeof(int));
        *errcode = -1;
        shm_ptr = errcode;
    }
    return shm_ptr;
}

void *mesh_register_receptor() {
    void *shm_ptr = mesh_get_shm_ptr();
    struct shm_context *context = get_shm_context(shm_ptr);
    if (context->heartbeat == 0) {
        printf("Error! Heartbeat is 0, mesh was not initialized!\n");
        int *errcode = malloc(sizeof(int));
        *errcode = -1;
        shm_ptr = errcode;
    }
    return shm_ptr;
}

int close_shared_memory(int shm_id) {
    return shmem_close_shared_memory(shm_id);
}

void initialize_buffer(void *shm_ptr, int buffer_size) {
    struct shm_caracter *buffer =
        (struct shm_caracter *) (
            shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores)
        );

    for (int i = 0; i < buffer_size; i++) {
        buffer[i].buffer_idx = i;
    }
}

void initialize_context(void *shm_ptr, int buffer_size, int input_file_size, int shm_id) {
    struct shm_context context = {
        .size_of_buffer = buffer_size,
        .size_of_input_file = input_file_size,
        .buffer_counter = 0,
        .heartbeat = 0,
        .file_idx = 0,
        .shm_id = shm_id,
    };
    memcpy(shm_ptr, &context, sizeof(struct shm_context));
}

void initialize_semaphores(void *shm_ptr, int buffer_size) {
    struct mesh_semaphores semaphores = {
        .emitter_sem = {},
        .receptor_sem = {},
        .buffer_idx_sem = {},
        .read_buffer_idx_sem = {}
    };

    sem_init(&(semaphores.emitter_sem), 1, buffer_size);
    sem_init(&(semaphores.receptor_sem), 1, 0);
    sem_init(&(semaphores.buffer_idx_sem), 1, 1);
    sem_init(&(semaphores.file_idx_sem), 1, 1);
    sem_init(&(semaphores.output_file_idx_sem), 1, 1);
    sem_init(&(semaphores.read_buffer_idx_sem), 1, 1);

    memcpy(
        shm_ptr + sizeof(struct shm_context),
        &semaphores,
        sizeof(struct mesh_semaphores));
}

void initialize_heartbeat(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    context->heartbeat = 1;
}

void *mesh_initialize(int buffer_size) {
    int shm_size = sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct shm_caracter) * buffer_size;
    void *shm_ptr = initialize_shared_memory(shm_size);
    int shm_id = get_shm_context(shm_ptr)->shm_id;
    int input_file_size = TEST_FILE_LENGTH; //TODO: get input file size

    initialize_context(shm_ptr, buffer_size, input_file_size, shm_id);
    initialize_heartbeat(shm_ptr);
    initialize_semaphores(shm_ptr, buffer_size);
    initialize_buffer(shm_ptr, buffer_size);

    return shm_ptr;
}

struct shm_caracter *get_buffer(void *shm_ptr){
    struct shm_caracter *buffer = 
        (struct shm_caracter *) (
            shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores)
        );
    return buffer;
}

struct mesh_semaphores *mesh_get_all_semaphores(void *shm_ptr) {
    struct mesh_semaphores *semaphores =
        (struct mesh_semaphores *) (shm_ptr + sizeof(struct shm_context) );
    return semaphores;
}

sem_t *mesh_get_emitter_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->emitter_sem);
}

sem_t *mesh_get_receptor_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->receptor_sem);
}

sem_t *mesh_get_buffer_idx_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->buffer_idx_sem);
}

sem_t *mesh_get_file_idx_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->file_idx_sem);
}

sem_t *mesh_get_read_buffer_idx_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->read_buffer_idx_sem);
}

sem_t *mesh_get_output_file_idx_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->output_file_idx_sem);
}

int get_buffer_idx(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *buffer_idx_sem = mesh_get_buffer_idx_semaphore(shm_ptr);
    sem_wait(buffer_idx_sem);

    int buffer_idx = context->buffer_counter;
    context->buffer_counter = (context->buffer_counter + 1) % context->size_of_buffer;

    sem_post(buffer_idx_sem);
    return buffer_idx;
}

int mesh_get_file_idx(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *file_idx_sem = mesh_get_file_idx_semaphore(shm_ptr);

    sem_wait(file_idx_sem);
    int file_idx = context->file_idx;
    if (file_idx >= context->size_of_input_file) {
        file_idx = -1;
    } else {
        context->file_idx++;
    }
    sem_post(file_idx_sem);

    return file_idx;
}

int mesh_get_output_file_idx(void *shm_ptr){
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *file_idx_sem = mesh_get_output_file_idx_semaphore(shm_ptr);

    sem_wait(file_idx_sem);
    int file_idx = context->output_file_idx;
    if (file_idx >= context->size_of_input_file) {
        file_idx = -1;
    } else {
        context->output_file_idx++;
    }
    sem_post(file_idx_sem);

    return file_idx;
}

int mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter) {
    struct shm_caracter *buffer = get_buffer(shm_ptr);
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);

    sem_wait(emitter_sem);

    int buffer_idx = get_buffer_idx(shm_ptr);
    caracter.buffer_idx = buffer_idx;
    buffer[buffer_idx] = caracter;

    sem_post(receptor_sem);

    return 0;
}

int get_read_buffer_idx(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *read_buffer_idx_sem = mesh_get_read_buffer_idx_semaphore(shm_ptr);
    sem_wait(read_buffer_idx_sem);

    int buffer_idx = context->read_buffer_counter;
    context->read_buffer_counter = (context->read_buffer_counter + 1) % context->size_of_buffer;

    sem_post(read_buffer_idx_sem);
    return buffer_idx;
}

struct shm_caracter mesh_get_caracter(void *shm_ptr) {
    struct shm_caracter *buffer = get_buffer(shm_ptr);
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);

    sem_wait(receptor_sem);

    int buffer_idx = get_read_buffer_idx(shm_ptr);
    struct shm_caracter caracter = buffer[buffer_idx];

    sem_post(emitter_sem);

    return caracter;
}

void mesh_finalize(void *shm_ptr) {
    int shm_id = get_shm_context(shm_ptr)->shm_id;
    close_shared_memory(shm_id);
}
