#ifndef SHMEM_HANDLER_H
#define SHMEM_HANDLER_H

#include <stdlib.h>

int open_shared_memory(char* shared_memory_name);
int give_size_to_shmem(int shm_fd, size_t size); 
void *get_ptr_to_shared_memory(int shm_fd, size_t bytes_to_map);
int shmem_close_shared_memory(char* shared_memory_name, void* shm_ptr, size_t size);

#endif