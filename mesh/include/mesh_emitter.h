#ifndef MESH_EMITTER_H
#define MESH_EMITTER_H
#include <stdlib.h>
#include "common/structures.c"
#include <semaphore.h>

void *mesh_register_emitter();
int mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter);
void mesh_finalize(void *shm_ptr);

#endif
