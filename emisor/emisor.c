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
    int receptorNum= rand() % 11;

    switch(receptorNum){
        case 0:// Set console text color to red
            printf("\033[31m");
            break;

        case 1:// Set console text color to magenta
            printf("\033[35m");
            break;

        case 2:// Set console text color to blue
            printf("\033[34m");
            break;

        case 3:// Set console text color to yellow
            printf("\033[33m");
            break;

        case 4:// Set console text color to green
            printf("\033[32m");
            break;

        case 5:// Set console text color to cyan
            printf("\033[36m");
            break;

        case 6:// Set console text color to light red
            printf("\033[91m");
            break;

        case 7:// Set console text color to light blue
            printf("\033[94m");
            break;

        case 8:// Set console text color to light yellow
            printf("\033[93m");
            break;

        case 9:// Set console text color to light green
            printf("\033[92m");
            break;

        case 10:// Set console text color to white
            printf("\033[97m");
            break;
    }
}

int obtener_modo() {
    int modo_ejecucion;
    while (1) {
        printf("Ingrese modo de ejecución (0 o 1): ");
        scanf("%d", &modo_ejecucion);
        if (modo_ejecucion == 0 || modo_ejecucion == 1) {
            break;
        } else {
            printf("Valor inválido. Por favor ingrese 0 o 1.\n");
        }
    }
    return modo_ejecucion;
}

char obtener_caracter(int index) {
    FILE *file;
    char c;

    file = fopen("../initializer/input.txt", "r");
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
    sleep(1);
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

    int index_idx = mesh_get_file_idx(shm_ptr);
    int heartbeat = get_heartbeat(shm_ptr);
    while(heartbeat && index_idx != -1) {        
        while(mode == 1){
            printf("Press Enter to continue.\n");
            int key= getchar();
            if(key==10){
                break;
            } 
        }
        char c = obtener_caracter(index_idx);
        char encrypted_c = c ^ key;
        struct shm_caracter caracter = create_caracter_emisor(encrypted_c, index_idx);
        caracter = mesh_add_caracter(shm_ptr, caracter);
        print_caracter_emisor(caracter, encrypted_c, c);
        index_idx = mesh_get_file_idx(shm_ptr);
        heartbeat = get_heartbeat(shm_ptr);
    }

    if(get_heartbeat(shm_ptr)==0) {
        mesh_finalize_emitter(shm_ptr);
    } else {
        mesh_natural_death_emitter(shm_ptr);
    }
}

int main(int argc, char *argv[]) {

    if (argc != 2) {
        printf("Error: falta la key!\n");
        return 1;
    }

    int key = atoi(argv[1]);
    if (key > 255 || key < 0) {
        printf("Error: key debe ser un entero entre 0 y 255!\n");
        return 1;
    }

    int modo_ejecucion;
    printf("+----------------------+\n");
    printf("| Modo de Ejecución    |\n");
    printf("+----------------------+\n");
    printf("| 0      | Automático  |\n");
    printf("| 1      | Manual      |\n");
    printf("+--------+-------------+\n");
    printf("Ingrese modo de ejecución (0 o 1): ");
    modo_ejecucion = obtener_modo();
    
    chooseColor();
    emisor(modo_ejecucion, key);
    return 0;
}
