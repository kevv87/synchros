#include "mesh.h"
#include "mesh_emitter.h"
#include "mesh_initializer.h"

#define SHM_NAME "/shared_mem"

#include <string.h>
#include <stdio.h>
#include <semaphore.h>

#include "shmem_handler.h"
#include "common/structures.c"

int shm_size = 10;

struct mesh_semaphores {
    sem_t emitter_sem;
    sem_t receptor_sem;
    sem_t buffer_idx_sem;
};

struct shm_context *get_shm_context(void *shm_ptr) {
    return (struct shm_context *) shm_ptr;
}

void *initialize_shared_memory(char *shared_memory, size_t size) {
    int shm_fd = open_shared_memory(shared_memory);
    give_size_to_shmem(shm_fd, size);
    return get_ptr_to_shared_memory(shm_fd, size);
}

void *mesh_get_shm_ptr() {
    int initial_shm_size = sizeof(struct shm_context) + sizeof(struct mesh_semaphores);
    void *shm_ptr = initialize_shared_memory(SHM_NAME, initial_shm_size);

    struct shm_context *context = get_shm_context(shm_ptr);
    int real_shm_size = 
        sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct shm_caracter) * context->size_of_buffer;
    shm_ptr = initialize_shared_memory(SHM_NAME, real_shm_size);
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
    return shm_ptr;
}

int close_shared_memory(char *shared_memory_name, void* shm_ptr, size_t size) {
    return shmem_close_shared_memory(shared_memory_name, shm_ptr, size);
}

void initialize_buffer(void *shm_ptr, int buffer_size) {
    struct shm_caracter *buffer =
        (struct shm_caracter *) (
            shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores)
        );

    for (int i = 0; i < buffer_size; i++) {
        buffer[i].buffer_idx = i;
    }

    memcpy(
        shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores),
        buffer,
        sizeof(struct shm_caracter *) * buffer_size);
}

void initialize_context(void *shm_ptr, int buffer_size, int input_file_size) {
    struct shm_context context = {
        .size_of_buffer = buffer_size,
        .size_of_input_file = input_file_size
    };
    memcpy(shm_ptr, &context, sizeof(struct shm_context));
}

void initialize_semaphores(void *shm_ptr, int buffer_size) {
    struct mesh_semaphores semaphores = {
        .emitter_sem = {},
        .receptor_sem = {},
        .buffer_idx_sem = {}
    };

    sem_init(&(semaphores.emitter_sem), 1, 13);
    sem_init(&(semaphores.receptor_sem), 1, 0);
    sem_init(&(semaphores.buffer_idx_sem), 1, 1);

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
    void *shm_ptr = initialize_shared_memory(SHM_NAME, shm_size);
    int input_file_size = 3; //TODO: get input file size

    initialize_context(shm_ptr, buffer_size, input_file_size);
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

int get_buffer_idx(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *buffer_idx_sem = mesh_get_buffer_idx_semaphore(shm_ptr);
    sem_wait(buffer_idx_sem);

    int buffer_idx = context->buffer_counter;
    context->buffer_counter = (context->buffer_counter + 1) % context->size_of_buffer;

    sem_post(buffer_idx_sem);
    return buffer_idx;
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

void mesh_finalize(void *shm_ptr) {
    close_shared_memory(SHM_NAME, shm_ptr, shm_size);
}
