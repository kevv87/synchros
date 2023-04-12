#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "common/structures.c"
#include "mesh_emitter.h"

#define BUFFER_SIZE 100

struct shm_caracter create_caracter_emisor(int index) {
    struct shm_caracter caracter;
    time_t current_time = time(NULL);
    // Seed the random number generator with the current time
    srand(time(NULL));

    caracter.value = rand() % (127 + 1 - 33) + 33;
    caracter.datetime = *localtime(&current_time);
    caracter.buffer_idx = index;
    return caracter;
}

void printCharacter_emisor(struct shm_caracter character, char asciiValue) {
    char datetime_str[50];
    strftime(datetime_str, sizeof(datetime_str), "%c", &character.datetime);

    printf("Caracter codificado y escrito \n");

    // Print table
    printf("| %-20s | %-15c \n", "Valor del caracter", asciiValue);
    printf("| %-20s | %-15d \n", "Indice de escritura", character.buffer_idx);
    printf("| %-20s | %-15s \n", "Fecha y hora", datetime_str);
}

// void print_caracter_info(struct shm_caracter caracter) {
//     // Formatear la hora
//     char time_buffer[20];
//     strftime(time_buffer, 20, "%Y-%m-%d %H:%M:%S", &(caracter.datetime));

//     // Imprimir la información en formato tabular alineado con colores
//     printf("\033[1;32mCaracter: \033[0m%c", caracter.value);
//     printf("\033[1;32m | Buffer index: \033[0m%d", caracter.buffer_idx);
//     printf("\033[1;32m | Timestamp: \033[0m%s\n", time_buffer);
// }

void emisor(int mode, char key) {
    // Register emitter and get shared memory pointer
    void* shm_ptr = mesh_register_emitter();
    if (shm_ptr == NULL) {
        perror("Error registering emitter");
        return;
    }

    // Initialize variables
    int index = 0;
    char buffer[BUFFER_SIZE];
    ssize_t n;

    // Read input file
    int fd = open("input.txt", O_RDONLY);
    if (fd == -1) {
        perror("Error opening file");
        return;
    }

    // Read file and add characters to shared memory buffer
    while ((n = read(fd, buffer, BUFFER_SIZE)) > 0) {
        for (int i = 0; i < n; i++) {
            char c = buffer[i];
            char encrypted_c = c ^ key;
            struct shm_caracter caracter = create_caracter_emisor(index);
            caracter.value = encrypted_c;
            mesh_add_caracter(shm_ptr, caracter);
            printCharacter_emisor(caracter, encrypted_c);
            index++;
        }
    }

    close(fd);

    // Finalize emitter
    mesh_finalize_emitter(shm_ptr);
}

int main() {
    emisor(1, 70);
    return 0;
}
