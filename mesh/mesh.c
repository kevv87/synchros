#include "shmem_handler.h"

void *initialize_shared_memory(char *shared_memory, size_t size) {
    int shm_fd = open_shared_memory(shared_memory);
    give_size_to_shmem(shm_fd, size);
    return get_ptr_to_shared_memory(shm_fd, size);
}

int close_shared_memory(char *shared_memory_name, void* shm_ptr, size_t size) {
    return shmem_close_shared_memory(shared_memory_name, shm_ptr, size);
}