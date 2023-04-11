#include "emitter.h"

void *mesh_register_emitter() {
    // Creamos el semáforo para controlar el acceso a la memoria compartida
    sem_t *sem = sem_open("/mesh_semaphore", O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("Error creando semáforo");
        exit(EXIT_FAILURE);
    }

    // Creamos la memoria compartida para almacenar los datos
    int shm_id = shmget(SHM_KEY, sizeof(struct shm_caracter) * MAX_BUFFER_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id < 0) {
        perror("Error creando memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Unimos la memoria compartida a un puntero
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("Error uniendo memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Inicializamos los valores de la memoria compartida a cero
    memset(shm_ptr, 0, sizeof(struct shm_caracter) * MAX_BUFFER_SIZE);

    // Devolvemos el puntero a la memoria compartida
    return shm_ptr;
}

void mesh_finalize_emitter(void *shm_ptr) {
    // Liberamos la memoria compartida
    if (shmdt(shm_ptr) == -1) {
        perror("Error liberando memoria compartida");
        exit(EXIT_FAILURE);
    }
}