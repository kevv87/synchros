#ifndef MESH_EMITTER_H
#define MESH_EMITTER_H
#include <stdlib.h>
#include "common/structures.c"
#include <semaphore.h>

void *mesh_register_receptor();
int mesh_get_output_file_idx(void *shm_ptr);
struct shm_caracter mesh_get_caracter(void *shm_ptr);

#endif
