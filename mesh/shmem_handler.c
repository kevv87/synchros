#include "shmem_handler.h"

#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/shm.h>

#include <unistd.h>

int open_shared_memory(char* shared_memory_name) {
    int shm_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error when trying to open memory");
        exit(1);
    }
    return shm_fd;
}

int give_size_to_shmem(int shm_fd, size_t size) {
    int error = ftruncate(shm_fd, size);
    if (error == -1) {
        perror("Error when trying to truncate file descriptor");
        exit(1);
    }
    return error;
}

void *get_ptr_to_shared_memory(int shm_fd, size_t bytes_to_map) {
     void *shm_ptr = mmap(
        NULL, bytes_to_map, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0); 

     return shm_ptr;
}

int shmem_close_shared_memory(char* shared_memory_name, void *shm_ptr, size_t size) {
    munmap(shm_ptr, size);
    return shm_unlink(shared_memory_name);
}
