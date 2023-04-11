#ifndef EMITTER
#define EMITTER

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

#define MAX_BUFFER_SIZE 1024
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define SHM_SIZE 1024
#define SEMAPHORE_NAME "/mesh_semaphore"

struct shm_caracter {
    char value;
    int index;
    time_t timestamp;
};

void *mesh_register_emitter();
void mesh_finalize_emitter(void *shm_ptr);
int mesh_get_file_idx(void *shm_ptr);
int mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter, sem_t *sem);
void mesh_send_file(char *file_path, char key);
void mesh_receive_file(char *output_file_path, char key);

#endif /* EMITTER */
