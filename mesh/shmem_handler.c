#include "include/shmem_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_SIZE 1024

int open_shared_memory(int project_id, size_t size) {
    key_t key = ftok("/dev/null", project_id);
    int shm_id = shmget(key, size, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Error when trying to open memory");
        exit(1);
    }
    printf("Created shared memory with id %d\n", shm_id);
    return shm_id;
}

void *get_ptr_to_shared_memory(int shm_id, size_t bytes_to_map) {
    printf("Attaching to shared memory with id %d\n", shm_id);
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *) -1) {
        perror("Error when trying to map shared memory");
        int *err_code = malloc(sizeof(int));
        *err_code = -1;
        return err_code;
    }
    printf("Shared memory pointer: %p\n", shm_ptr);
    return shm_ptr;
}

int shm_unmap(void *shm_ptr, size_t size) {
    printf("Detaching shared memory address %p\n", shm_ptr);
    int result = shmdt(shm_ptr);
    if (result == -1) {
        perror("Error when trying to detach shared memory");
        exit(1);
    }
    return result;
}

int shmem_close_shared_memory(int shm_id) {
    printf("Removing shared memory with id %d\n", shm_id);
    int result = shmctl(shm_id, IPC_RMID, NULL);
    if (result == -1) {
        perror("Error when trying to remove shared memory");
        exit(1);
    }
    return result;
}
