#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <unistd.h>

int open_shared_memory(char* shared_memory_name) {
    int shm_fd = shm_open(shared_memory_name, O_CREAT | O_RDWR, 0666);
    return shm_fd;
}

void *get_ptr_to_shared_memory(int shm_fd) {
    int *res = malloc(sizeof(int));
    *res = 1;
    return (void *)res;
}

int close_shared_memory(char* shared_memory_name) {
    return shm_unlink(shared_memory_name);
}