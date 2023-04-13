#ifndef COMMON_STRUCTURES
#define COMMON_STRUCTURES

#include <time.h>
#include <semaphore.h>

#define PROJECT_ID 42

struct shm_context {
    int size_of_buffer;
    int size_of_input_file;
    int buffer_counter;
    int heartbeat;
    int file_idx;
    int output_file_idx;
    int read_buffer_counter;
    int shm_id;
};

struct auditory_info {
    int transferred_characters;
    sem_t transferred_characters_semaphore;

    int characters_in_buffer;
    sem_t characters_in_buffer_semaphore;

    int alive_emitters;
    sem_t alive_emitters_semaphore;

    int total_emitters;
    sem_t total_emitters_semaphore;

    int alive_receptors;
    sem_t alive_receptors_semaphore;

    int total_receptors;
    sem_t total_receptors_semaphore;

    int shm_mem_used;
};

struct shm_caracter {
    char value;
    struct tm datetime; // from time.h
    int buffer_idx;
    int file_idx;
};

#endif
