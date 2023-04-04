#ifndef MESH_H
#define MESH_H

int open_shared_memory(char* shared_memory_name);
int close_shared_memory(char* shared_memory_name);
void *get_ptr_to_shared_memory(int shm_fd);

#endif