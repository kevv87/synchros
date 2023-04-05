#ifndef MESH_H
#define MESH_H

int open_shared_memory(char* shared_memory_name);
int give_size_to_shmem(int shm_fd, size_t size); 
void *get_ptr_to_shared_memory(int shm_fd, size_t bytes_to_map);

void *initialize_shared_memory(char* shared_memory_name, size_t size);

int close_shared_memory(char* shared_memory_name);

#endif