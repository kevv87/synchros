#define SHM_NAME "/shared_mem"

#include <string.h>
#include <semaphore.h>

#include "shmem_handler.h"
#include "common/structures.c"

int shm_size = 10;

struct mesh_semaphores {
    sem_t emitter_sem;
    sem_t receptor_sem;
    sem_t buffer_idx_sem;
};

void *initialize_shared_memory(char *shared_memory, size_t size) {
    int shm_fd = open_shared_memory(shared_memory);
    give_size_to_shmem(shm_fd, size);
    return get_ptr_to_shared_memory(shm_fd, size);
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
        shm_ptr + sizeof(struct shm_context),
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

void *mesh_initialize(int buffer_size) {
    void *shm_ptr = initialize_shared_memory(SHM_NAME, shm_size);
    int input_file_size = 3; //TODO: get input file size
    initialize_context(shm_ptr, buffer_size, input_file_size);
    initialize_semaphores(shm_ptr, buffer_size);
    initialize_buffer(shm_ptr, buffer_size);

    return shm_ptr;
}

struct shm_context get_shm_context(void *shm_ptr) {
    struct shm_context context = {};
    memcpy(&context, shm_ptr, sizeof(struct shm_context));
    return context;
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

//int void mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter) {
//    struct shm_caracter *buffer = get_buffer(shm_ptr);

//}

void mesh_finalize(void *shm_ptr) {
    close_shared_memory(SHM_NAME, shm_ptr, shm_size);
}
