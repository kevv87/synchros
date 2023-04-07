#include "shmem_handler.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_SIZE 1024

int open_shared_memory(char* shared_memory_name, size_t size) {
    int shm_id = shmget(IPC_PRIVATE, size, IPC_CREAT | 0666);
    if (shm_id == -1) {
        perror("Error when trying to open memory");
        exit(1);
    }
    return shm_id;
}

void *get_ptr_to_shared_memory(int shm_id, size_t bytes_to_map) {
    printf("Mapping %ld bytes to shared memory\n", bytes_to_map);
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void *) -1) {
        perror("Error when trying to map shared memory");
        exit(1);
    }
    printf("Shared memory pointer: %p\n", shm_ptr);
    return shm_ptr;
}

int shmem_shmdt(void *shm_ptr) {
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
