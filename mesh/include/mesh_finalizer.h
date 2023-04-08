#ifndef MESH_FINALIZER_H
#define MESH_FINALIZER_H
#include "common/structures.c"

void *mesh_get_shm_ptr();
struct auditory_info mesh_finalize(void *shm_ptr);

#endif
