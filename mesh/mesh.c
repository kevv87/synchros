#define SHM_NAME "/shared_mem"

#include <string.h>

#include "shmem_handler.h"
#include "common/structures.c"

int shm_size = 10;

void *initialize_shared_memory(char *shared_memory, size_t size) {
    int shm_fd = open_shared_memory(shared_memory);
    give_size_to_shmem(shm_fd, size);
    return get_ptr_to_shared_memory(shm_fd, size);
}

int close_shared_memory(char *shared_memory_name, void* shm_ptr, size_t size) {
    return shmem_close_shared_memory(shared_memory_name, shm_ptr, size);
}

void *mesh_initialize(int buffer_size) {
    void *shm_ptr = initialize_shared_memory(SHM_NAME, shm_size);
    struct shm_context context = {
        .size_of_buffer = buffer_size,
        .size_of_input_file = 10
    };
    memcpy(shm_ptr, &context, sizeof(struct shm_context));
    return shm_ptr;
}

struct shm_context get_shm_context(void *shm_ptr) {
    struct shm_context context = {};
    memcpy(&context, shm_ptr, sizeof(struct shm_context));
    return context;
}

struct shm_caracter *get_buffer(void *shm_ptr){
    struct shm_caracter *buffer =
        (struct shm_caracter *)shm_ptr;
    return buffer;
}

void mesh_finalize(void *shm_ptr) {
    close_shared_memory(SHM_NAME, shm_ptr, shm_size);
}
