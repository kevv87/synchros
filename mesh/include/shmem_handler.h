#ifndef SHMEM_HANDLER_H
#define SHMEM_HANDLER_H

#include <stdlib.h>

int create_shared_memory(size_t size);
void* attach_shared_memory(int shmid, size_t size);
void *get_ptr_to_shared_memory(int shm_fd, size_t bytes_to_map);
int delete_shared_memory(int shmid) {
void detach_shared_memory(void* shm_ptr);
#endif
