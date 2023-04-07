#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>

#define SHM_NAME "/test-shm"
#define SHM_SIZE 1024

int main() {
    int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Error creating shared memory");
        exit(1);
    }

    if (ftruncate(shm_fd, SHM_SIZE) == -1) {
        perror("Error setting size of shared memory");
        exit(1);
    }

    void *shm_ptr = mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("Error mapping shared memory");
        exit(1);
    }

    printf("Shared memory created with name '%s' and size %d\n", SHM_NAME, SHM_SIZE);

    while (1) {
        // Keep the program running
        sleep(10);
    }

    return 0;
}

