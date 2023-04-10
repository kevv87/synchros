#ifndef MESH_H
#define MESH_H

#include <stdlib.h>
#include "common/structures.c"
#include <semaphore.h>

struct mesh_semaphores {
    sem_t emitter_sem;
    sem_t receptor_sem;
    sem_t buffer_idx_sem;
    sem_t file_idx_sem;
    sem_t output_file_idx_sem;
    sem_t read_buffer_idx_sem;
};

void *initialize_shared_memory(size_t size);
void unmap_shared_memory(void *shm_ptr);

void *mesh_initialize();
void *mesh_register_emitter();
void mesh_finalize_emitter(void *shm_ptr);
void *mesh_register_receptor();
void mesh_finalize_receptor(void *shm_ptr);
void wait_all_threads();
struct shm_context *get_shm_context();
struct auditory_info *get_auditory_info(void *shm_ptr);
void *mesh_get_shm_ptr();
struct shm_caracter *get_buffer(void *shm_ptr);
sem_t *mesh_get_emitter_semaphore(void *shm_ptr);
sem_t *mesh_get_receptor_semaphore(void *shm_ptr);
sem_t *mesh_get_buffer_idx_semaphore(void *shm_ptr);
int get_heartbeat(void *shm_ptr);

int mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter);

struct shm_caracter mesh_get_caracter(void *shm_ptr);

struct auditory_info mesh_finalize(void *shm_ptr);
int close_shared_memory(int shm_id);

#endif
