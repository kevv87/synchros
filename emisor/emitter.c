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

int mesh_get_file_idx(void *shm_ptr) {
    struct shm_caracter *shm_buf = (struct shm_caracter*) shm_ptr;
    int i;
    for (i = 0; i < MAX_BUFFER_SIZE; i++) {
        if (shm_buf[i].timestamp == 0) {
            // Si encontramos un espacio vacío, devolvemos su índice
            return i;
        }
    }
    // Si no hay espacios vacíos, devolvemos -1
    return -1;
}

int mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter, sem_t *sem) {
    struct shm_caracter *shm_buf = (struct shm_caracter*) shm_ptr;
    int idx = mesh_get_file_idx(shm_ptr);
    if (idx >= 0) {
        // Si hay espacio disponible, añadimos el caracter a la memoria compartida
        shm_buf[idx] = caracter;
        // Mostramos la información del caracter introducido
        printf("%-10d%-10d%-10ld%-10c\n", caracter.index, idx, caracter.timestamp, caracter.value);
        return idx;
    } else {
        // Si no hay espacio disponible, esperamos a que se libere algún espacio
        while (sem_wait(sem) == -1) {
            if (errno != EINTR) {
                perror("Error esperando semáforo");
                exit(EXIT_FAILURE);
            }
        }
        // Una vez que se libera el semáforo, intentamos añadir el caracter de nuevo
        return mesh_add_caracter(shm_ptr, caracter, sem);
    }
}