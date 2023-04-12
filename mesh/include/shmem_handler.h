#ifndef SHMEM_HANDLER_H
#define SHMEM_HANDLER_H

#include <stdlib.h>

int open_shared_memory(int project_id, size_t size);
void *get_ptr_to_shared_memory(int shm_id, size_t bytes_to_map);
int shmem_close_shared_memory(int shm_id);
int shm_unmap(void *shm_ptr, size_t size);
#endif
