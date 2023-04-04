#include <time.h>
#include <semaphore.h>

struct shm_caracter {
    char value;
    struct tm datetime; // from time.h
    int buffer_idx;
    int file_idx;
};

struct transferred_characters{
    int value;
    sem_t semaphore;
};

struct characters_in_buffer {
    int value;
    sem_t semaphore;
};

struct emisor_info {
    int alive;
    int total;
    sem_t semaphore;
};

struct receptor_info {
    int alive;
    int total;
    sem_t semaphore;
};

struct shm_mem_used {
    int value;
    sem_t semaphore;
};

struct auditory_info {
    struct transferred_characters transferred_characters;
    struct characters_in_buffer characters_in_buffer;
    struct emisor_info emisor_info;
    struct receptor_info receptor_info;
    struct shm_mem_used shm_mem_used;
};