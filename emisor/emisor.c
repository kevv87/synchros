#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "../common/structures.c"
#include "../mesh/include/mesh_emitter.h"

struct shm_caracter create_caracter_emisor(int valor, int index) {
    struct shm_caracter caracter;
    time_t current_time = time(NULL);

    caracter.value = valor;
    caracter.file_idx = index;
    caracter.datetime = *localtime(&current_time);
    //sleep(1);
    return caracter;
}

void printCharacter_emisor(struct shm_caracter character, char asciiValue, char prevVal) {
    char datetime_str[50];
    strftime(datetime_str, sizeof(datetime_str), "%c", &character.datetime);

    printf("Caracter codificado y escrito \n");
    // Print table
    printf("| %-20s | %-15c \n", "Valor del caracter", prevVal);
    printf("| %-20s | %-15c \n", "Valor encriptado", asciiValue);
    printf("| %-20s | %-15d \n", "Indice de escritura", character.buffer_idx);
    printf("| %-20s | %-15s \n", "Fecha y hora", datetime_str);
}

char obtener_caracter(int index) {
    FILE *file;
    char c;

    file = fopen("input.txt", "r");
    if (file == NULL) {
        printf("No se pudo abrir el archivo.\n");
        return 0;
    }

    fseek(file, index, SEEK_SET);
    c = fgetc(file);
    fclose(file);
    return c;
}

void emisor(int mode, char key) {
    // Register emitter and get shared memory pointer
    void* shm_ptr = mesh_register_emitter();
    if (shm_ptr == NULL) {
        perror("Error registering emitter");
        return;
    }

    int index_idx = mesh_get_file_idx(shm_ptr);
    int heartbeat = get_heartbeat(shm_ptr);
    while(heartbeat && index_idx != -1) {        
        char c = obtener_caracter(index_idx);
        char encrypted_c = c ^ key;
        struct shm_caracter caracter = create_caracter_emisor(encrypted_c, index_idx);
        printf("Calling add caracter\n");
        caracter = mesh_add_caracter(shm_ptr, caracter);
        printCharacter_emisor(caracter, encrypted_c, c);
        printf("Called get file idx\n");
        index_idx = mesh_get_file_idx(shm_ptr);
        heartbeat = get_heartbeat(shm_ptr);
        printf("%s\n", heartbeat ? "ON" : "OFF");
    }
    // Finalize emitter
    mesh_finalize_emitter(shm_ptr);
}

int main() {
    emisor(1, 70);
    return 0;
}
