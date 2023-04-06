#ifndef MESH_H
#define MESH_H
#include <stdlib.h>
#include "common/structures.c"

void *initialize_shared_memory(char* shared_memory_name, size_t size);

void *mesh_initialize();

struct shm_context get_shm_context();
struct shm_caracter *get_buffer(void *shm_ptr);

void mesh_finalize(void *shm_ptr);

int close_shared_memory(char* shared_memory_name);

#endif
