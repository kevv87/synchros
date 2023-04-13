#include "include/mesh.h"
#include "include/mesh_emitter.h"
#include "include/mesh_initializer.h"

#define SHM_NAME "/shared_mem"

#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

#include "include/shmem_handler.h"
#include "../common/structures.c"
#include "../common/debugging.c"

#define TEST_FILE_LENGTH 20

typedef struct thread_list_node {
    pthread_t *thread_id;
    struct thread_list_node* next;
} thread_list_node_t;

thread_list_node_t *thread_list;

void add_thread_to_list(pthread_t *thread) {
    thread_list_node_t* new_node = malloc(sizeof(thread_list_node_t));
    new_node->thread_id = thread;
    new_node->next = thread_list;
    thread_list = new_node;
}

struct shm_context *get_shm_context(void *shm_ptr) {
    return (struct shm_context *) shm_ptr;
}

void *initialize_shared_memory(size_t size) {
    int shm_id = open_shared_memory(PROJECT_ID, size);
    void *shm_ptr = get_ptr_to_shared_memory(shm_id, size);
    get_shm_context(shm_ptr)->shm_id = shm_id;
    return shm_ptr;
}

void unmap_shared_memory(void *shm_ptr){
    size_t size = sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct shm_caracter) * get_shm_context(shm_ptr)->size_of_buffer;
    shm_unmap(shm_ptr, size);
}

int get_heartbeat(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    return context->heartbeat;
}

void *mesh_get_shm_ptr() {
    int initial_shm_size = sizeof(struct shm_context) + sizeof(struct mesh_semaphores);
    void *shm_ptr = initialize_shared_memory(initial_shm_size);

    struct shm_context *context = get_shm_context(shm_ptr);
    int real_shm_size = 
        sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct shm_caracter) * context->size_of_buffer;
    shm_unmap(shm_ptr, initial_shm_size);

    shm_ptr = initialize_shared_memory(real_shm_size);
    return shm_ptr;
}

struct auditory_info *get_auditory_info(void *shm_ptr) {
    struct auditory_info *auditory_info = 
        shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores);
    return auditory_info;
}

void * increment_alive_emitters(void *shm_ptr) {
    printf("Registering one alive emitter\n");
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);
    sem_wait(&auditory_info->alive_emitters_semaphore);
    auditory_info->alive_emitters++;
    sem_post(&auditory_info->alive_emitters_semaphore);
}

void * increment_total_emitters(void *shm_ptr) {
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);
    sem_wait(&auditory_info->total_emitters_semaphore);
    auditory_info->total_emitters++;
    sem_post(&auditory_info->total_emitters_semaphore);
}

void * increment_alive_receptors(void *shm_ptr) {
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);
    sem_wait(&auditory_info->alive_receptors_semaphore);
    auditory_info->alive_receptors++;
    sem_post(&auditory_info->alive_receptors_semaphore);
}

void * increment_total_receptors(void *shm_ptr) {
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);
    sem_wait(&auditory_info->total_receptors_semaphore);
    auditory_info->total_receptors++;
    sem_post(&auditory_info->total_receptors_semaphore);
}

void wait_all_threads() {
    printf("Waiting for all threads to finish\n");
    thread_list_node_t* current = thread_list;
    while (current != NULL) {
        pthread_join(*current->thread_id, NULL);
        current = current->next;
    }
}

void create_thread(void *(*thread_function)(void *), void *shm_ptr) {
    pthread_t *thread = malloc(sizeof(pthread_t));
    pthread_create(thread, NULL, thread_function, shm_ptr);
    add_thread_to_list(thread);
}

void *mesh_register_emitter() {
    void *shm_ptr = mesh_get_shm_ptr();
    struct shm_context *context = get_shm_context(shm_ptr);
    if (context->heartbeat == 0) {
        printf("Error! Heartbeat is 0, mesh was not initialized!\n");
        int *errcode = malloc(sizeof(int));
        *errcode = -1;
        shm_ptr = errcode;
        return shm_ptr;
    }

    create_thread(&increment_alive_emitters, shm_ptr);
    create_thread(&increment_total_emitters, shm_ptr);

    return shm_ptr;
}

void *mesh_register_receptor() {
    void *shm_ptr = mesh_get_shm_ptr();
    struct shm_context *context = get_shm_context(shm_ptr);
    if (context->heartbeat == 0) {
        printf("Error! Heartbeat is 0, mesh was not initialized!\n");
        int *errcode = malloc(sizeof(int));
        *errcode = -1;
        shm_ptr = errcode;
    }

    create_thread(&increment_alive_receptors, shm_ptr);
    create_thread(&increment_total_receptors, shm_ptr);

    return shm_ptr;
}

int close_shared_memory(int shm_id) {
    return shmem_close_shared_memory(shm_id);
}

void initialize_buffer(void *shm_ptr, int buffer_size) {
    printf("Starting to initialize buffer\n");
    struct shm_caracter *buffer =
        (struct shm_caracter *) (
            shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
            sizeof(struct auditory_info) );

    for (int i = 0; i < buffer_size; i++) {
        printf("Initializing index %d\n", i);
        buffer[i].buffer_idx = i;
    }
}

void initialize_context(void *shm_ptr, int buffer_size, int input_file_size, int shm_id) {
    struct shm_context context = {
        .size_of_buffer = buffer_size,
        .size_of_input_file = input_file_size,
        .buffer_counter = 0,
        .heartbeat = 0,
        .file_idx = 0,
        .shm_id = shm_id,
    };
    memcpy(shm_ptr, &context, sizeof(struct shm_context));
}

void initialize_semaphores(void *shm_ptr, int buffer_size) {
    struct mesh_semaphores semaphores = {
        .emitter_sem = {},
        .receptor_sem = {},
        .buffer_idx_sem = {},
        .read_buffer_idx_sem = {}
    };

    sem_init(&(semaphores.emitter_sem), 1, buffer_size);
    sem_init(&(semaphores.receptor_sem), 1, 0);
    sem_init(&(semaphores.buffer_idx_sem), 1, 1);
    sem_init(&(semaphores.file_idx_sem), 1, 1);
    sem_init(&(semaphores.output_file_idx_sem), 1, 1);
    sem_init(&(semaphores.read_buffer_idx_sem), 1, 1);

    memcpy(
        shm_ptr + sizeof(struct shm_context),
        &semaphores,
        sizeof(struct mesh_semaphores));
}

void initialize_heartbeat(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    context->heartbeat = 1;
}

void initialize_auditory(void *shm_ptr, int buffer_size) {
    struct auditory_info* auditory_info = 
        (struct auditory_info *)
        (shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores) );

    sem_init(&(auditory_info->alive_emitters_semaphore), 1, 1);
    sem_init(&(auditory_info->total_emitters_semaphore), 1, 1);
    sem_init(&(auditory_info->alive_receptors_semaphore), 1, 1);
    sem_init(&(auditory_info->total_receptors_semaphore), 1, 1);
    sem_init(&(auditory_info->transferred_characters_semaphore), 1, 1);
    sem_init(&(auditory_info->characters_in_buffer_semaphore), 1, 1);

    auditory_info->shm_mem_used = 
        sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct auditory_info) + sizeof(struct shm_caracter) * buffer_size;
}

void *mesh_initialize(int buffer_size) {
    printf("Starting mesh initialization\n");
    int shm_size =
        sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
        sizeof(struct auditory_info) + sizeof(struct shm_caracter) * buffer_size;
    void *shm_ptr = initialize_shared_memory(shm_size);
    int shm_id = get_shm_context(shm_ptr)->shm_id;
    int input_file_size = TEST_FILE_LENGTH; //TODO: get input file size

    initialize_context(shm_ptr, buffer_size, input_file_size, shm_id);
    printf("Context initialized\n");

    initialize_auditory(shm_ptr, buffer_size);
    printf("Auditory initialized\n");

    initialize_heartbeat(shm_ptr);
    printf("Heartbeat initialized\n");

    initialize_semaphores(shm_ptr, buffer_size);
    printf("Semaphores initialized\n");

    initialize_buffer(shm_ptr, buffer_size);
    printf("Mesh initialized correctly!\n");

    return shm_ptr;
}

struct shm_caracter *get_buffer(void *shm_ptr){
    struct shm_caracter *buffer = 
        (struct shm_caracter *) (
            shm_ptr + sizeof(struct shm_context) + sizeof(struct mesh_semaphores) +
            sizeof(struct auditory_info) );
    return buffer;
}

struct mesh_semaphores *mesh_get_all_semaphores(void *shm_ptr) {
    struct mesh_semaphores *semaphores =
        (struct mesh_semaphores *) (shm_ptr + sizeof(struct shm_context) );
    return semaphores;
}

sem_t *mesh_get_emitter_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->emitter_sem);
}

sem_t *mesh_get_receptor_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->receptor_sem);
}

sem_t *mesh_get_buffer_idx_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->buffer_idx_sem);
}

sem_t *mesh_get_file_idx_semaphore(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->file_idx_sem);
}

sem_t *mesh_get_read_buffer_idx_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->read_buffer_idx_sem);
}

sem_t *mesh_get_output_file_idx_semaphore(void *shm_ptr) {
    struct mesh_semaphores *semaphores = mesh_get_all_semaphores(shm_ptr);
    return &(semaphores->output_file_idx_sem);
}

int get_buffer_idx(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *buffer_idx_sem = mesh_get_buffer_idx_semaphore(shm_ptr);
    sem_wait(buffer_idx_sem);

    int buffer_idx = context->buffer_counter;
    context->buffer_counter = (context->buffer_counter + 1) % context->size_of_buffer;

    sem_post(buffer_idx_sem);
    return buffer_idx;
}

int mesh_get_file_idx(void *shm_ptr) {
    if (!get_heartbeat(shm_ptr)) {
        return -1;
    }
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *file_idx_sem = mesh_get_file_idx_semaphore(shm_ptr);

    sem_wait(file_idx_sem);
    int file_idx = context->file_idx;
    if (file_idx >= (context->size_of_input_file-1)) {
        printf("Reached end of file\n");
        file_idx = -1;
    } else {
        context->file_idx++;
    }
    sem_post(file_idx_sem);

    return file_idx;
}

int mesh_get_output_file_idx(void *shm_ptr){
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *file_idx_sem = mesh_get_output_file_idx_semaphore(shm_ptr);

    sem_wait(file_idx_sem);
    int file_idx = context->output_file_idx;
    if (file_idx >= context->size_of_input_file) {
        file_idx = -1;
    } else {
        context->output_file_idx++;
    }
    sem_post(file_idx_sem);

    return file_idx;
}

void *increment_characters_in_buffer(void *shm_ptr) {
    struct auditory_info *auditory = get_auditory_info(shm_ptr);
    sem_t characters_in_buffer_semaphore = auditory->characters_in_buffer_semaphore;
    sem_wait(&characters_in_buffer_semaphore);
    auditory->characters_in_buffer++;
    sem_post(&characters_in_buffer_semaphore);
}

struct shm_caracter mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter) {
    struct shm_context *context = get_shm_context(shm_ptr);
    if (!get_heartbeat(shm_ptr)){
        return (struct shm_caracter){};
    }
    struct shm_caracter *buffer = get_buffer(shm_ptr);
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);

    sem_wait(emitter_sem);

    int buffer_idx = get_buffer_idx(shm_ptr);
    caracter.buffer_idx = buffer_idx;
    buffer[buffer_idx] = caracter;
    create_thread(&increment_characters_in_buffer, shm_ptr);

    sem_post(receptor_sem);

    return caracter;
}

int get_read_buffer_idx(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    sem_t *read_buffer_idx_sem = mesh_get_read_buffer_idx_semaphore(shm_ptr);
    sem_wait(read_buffer_idx_sem);

    int buffer_idx = context->read_buffer_counter;
    context->read_buffer_counter = (context->read_buffer_counter + 1) % context->size_of_buffer;

    sem_post(read_buffer_idx_sem);
    return buffer_idx;
}

void *decrement_characters_in_buffer(void *shm_ptr) {
    struct auditory_info *auditory = get_auditory_info(shm_ptr);
    sem_t characters_in_buffer_semaphore = auditory->characters_in_buffer_semaphore;
    sem_wait(&characters_in_buffer_semaphore);
    auditory->characters_in_buffer--;
    sem_post(&characters_in_buffer_semaphore);
}

void *increment_transferred_characters(void *shm_ptr) {
    struct auditory_info *auditory = get_auditory_info(shm_ptr);
    sem_t transferred_characters_semaphore = auditory->transferred_characters_semaphore;
    sem_wait(&transferred_characters_semaphore);
    auditory->transferred_characters++;
    sem_post(&transferred_characters_semaphore);
}

struct shm_caracter mesh_get_caracter(void *shm_ptr) {
    struct shm_caracter *buffer = get_buffer(shm_ptr);
    sem_t *emitter_sem = mesh_get_emitter_semaphore(shm_ptr);
    sem_t *receptor_sem = mesh_get_receptor_semaphore(shm_ptr);

    sem_wait(receptor_sem);

    int buffer_idx = get_read_buffer_idx(shm_ptr);
    struct shm_caracter caracter = buffer[buffer_idx];
    create_thread(&decrement_characters_in_buffer, shm_ptr);
    create_thread(&increment_transferred_characters, shm_ptr);

    sem_post(emitter_sem);

    return caracter;
}

void *decrement_alive_emitters(void *shm_ptr) {
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);
    sem_t *alive_emitters_sem = &(auditory_info->alive_emitters_semaphore);
    sem_wait(alive_emitters_sem);
    auditory_info->alive_emitters--;
    sem_post(alive_emitters_sem);
}

void *decrement_alive_receptors(void *shm_ptr) {
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);
    sem_t *alive_receptors_sem = &(auditory_info->alive_receptors_semaphore);
    sem_wait(alive_receptors_sem);
    auditory_info->alive_receptors--;
    sem_post(alive_receptors_sem);
}

void mesh_finalize_emitter(void *shm_ptr) {
    create_thread(&decrement_alive_emitters, shm_ptr);
    wait_all_threads();
}

void mesh_finalize_receptor(void *shm_ptr) {
    create_thread(&decrement_alive_receptors, shm_ptr);
    wait_all_threads();
}

void destroy_semaphore(sem_t *semaphore, int process_quantity) {
    for (int i = 0; i < process_quantity; i++) {
        sem_post(semaphore);
    }
    sem_destroy(semaphore);
}

void destroy_all_semaphores(void *shm_ptr) {
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);
    int total_processes = auditory_info->total_emitters + auditory_info->total_receptors;

    sem_t **semaphores = malloc(sizeof(sem_t *) * 12);
    semaphores[0] = mesh_get_emitter_semaphore(shm_ptr);
    semaphores[1] = mesh_get_receptor_semaphore(shm_ptr);
    semaphores[2] = mesh_get_buffer_idx_semaphore(shm_ptr);
    semaphores[3] = mesh_get_file_idx_semaphore(shm_ptr);
    semaphores[4] = mesh_get_output_file_idx_semaphore(shm_ptr);
    semaphores[5] = mesh_get_read_buffer_idx_semaphore(shm_ptr);
    semaphores[6] = &auditory_info->alive_emitters_semaphore;
    semaphores[7] = &auditory_info->total_emitters_semaphore;
    semaphores[8] = &auditory_info->alive_receptors_semaphore;
    semaphores[9] = &auditory_info->total_receptors_semaphore;
    semaphores[10] = &auditory_info->transferred_characters_semaphore;
    semaphores[11] = &auditory_info->characters_in_buffer_semaphore;

    for (int i = 0; i < total_processes; i++) {
        destroy_semaphore(semaphores[i], total_processes);
    }

}

struct auditory_info mesh_finalize(void *shm_ptr) {
    struct shm_context *context = get_shm_context(shm_ptr);
    struct auditory_info *auditory_info = get_auditory_info(shm_ptr);

    context->heartbeat = 0;
    destroy_all_semaphores(shm_ptr);
    while(auditory_info->alive_emitters > 0 || auditory_info->alive_receptors > 0) {
        printf("Someone is still alive...\n");
        printf("Alive emitters: %d\n", auditory_info->alive_emitters);
        printf("Alive receptors: %d\n", auditory_info->alive_receptors);
        sleep(1);
    }

    int shm_id = context->shm_id;
    close_shared_memory(shm_id);
}
