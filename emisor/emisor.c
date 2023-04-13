#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include "../common/structures.c"
#include "../mesh/include/mesh_emitter.h"

void print_caracter_emisor(struct shm_caracter character, char asciiValue, char prevVal) {
    char datetime_str[50];
    strftime(datetime_str, sizeof(datetime_str), "%c", &character.datetime);

    printf("Caracter codificado y escrito \n");
    // Print table
    printf("| %-20s | %-15c \n", "Valor del caracter", prevVal);
    printf("| %-20s | %-15c \n", "Valor encriptado", asciiValue);
    printf("| %-20s | %-15d \n", "Indice de escritura", character.buffer_idx);
    printf("| %-20s | %-15s \n", "Fecha y hora", datetime_str);
}

void chooseColor(){
    srand(time(NULL));
    int receptorNum= rand() % 6;

    switch(receptorNum){
        case 0:// Set console text color to red
            printf("\033[31m");
            break;

        case 1:// Set console text color to magenta
            printf("\033[35m");

        case 2:// Set console text color to blue
            printf("\033[34m");
            break;

        case 3:// Set console text color to yellow
            printf("\033[33m");

        case 4:// Set console text color to green
            printf("\033[32m");
            break;

        case 5:// Set console text color to cyan
            printf("\033[36m");
            break;
    }
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

struct shm_caracter create_caracter_emisor(int valor, int index) {
    struct shm_caracter caracter;
    time_t current_time = time(NULL);

    caracter.value = valor;
    caracter.file_idx = index;
    caracter.datetime = *localtime(&current_time);
    //sleep(1);
    return caracter;
}

void emisor(int mode, char key) {
    char str[20];
    sprintf(str, "%d", mode);
    printf("El entero es: %d\n", mode);
    printf("La cadena de caracteres es: %s\n", str);
    // Register emitter and get shared memory pointer
    void* shm_ptr = mesh_register_emitter();
    if (shm_ptr == NULL) {
        perror("Error registering emitter");
        return;
    }

    if(mode == 0) {

    int index_idx = mesh_get_file_idx(shm_ptr);
    int heartbeat = get_heartbeat(shm_ptr);
    while(heartbeat && index_idx != -1) {        
        char c = obtener_caracter(index_idx);
        char encrypted_c = c ^ key;
        struct shm_caracter caracter = create_caracter_emisor(encrypted_c, index_idx);
        caracter = mesh_add_caracter(shm_ptr, caracter);
        print_caracter_emisor(caracter, encrypted_c, c);
        index_idx = mesh_get_file_idx(shm_ptr);
        heartbeat = get_heartbeat(shm_ptr);
    }
    // Finalize emitter
    mesh_finalize_emitter(shm_ptr);

    }
    else {
        //Adjust the condition. Needs to be cycled until there is a signal.
        while(1){
            while(1){
                printf("Press Enter to execute the function.\n");
                int key= getchar();
                if(key==10){
                    break;
                } 
            }
            int index_idx = mesh_get_file_idx(shm_ptr);
            int heartbeat = get_heartbeat(shm_ptr);
            while(heartbeat && index_idx != -1) { 
                char c = obtener_caracter(index_idx);
                char encrypted_c = c ^ key;
                struct shm_caracter caracter = create_caracter_emisor(encrypted_c, index_idx);
                caracter = mesh_add_caracter(shm_ptr, caracter);
                print_caracter_emisor(caracter, encrypted_c, c);
                index_idx = mesh_get_file_idx(shm_ptr);
                heartbeat = get_heartbeat(shm_ptr);
            }
            // Finalize emitter
            mesh_finalize_emitter(shm_ptr);
        }
    }

}

int main() {
    chooseColor();
    emisor(1, 70);
    return 0;
}
