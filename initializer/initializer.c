#include "mesh_initializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int get_file_length(char *file_name) {
    struct stat st;
    if(stat(file_name, &st) == 0) {
        return st.st_size;
    } else {
        printf("Error al obtener el tamaño del archivo %s\n", file_name);
        return -1;
    }
}

int main(int argc, char *argv[]) {
    //TODO: Falta el identificador de espacio compartido
    printf("Inicializando el espacio compartido!\n");
    if (argc != 2) {
        printf("Este programa usa exactamente 1 argumento!\n");
        printf("Falta el argumento de tamaño del buffer\n");
        return 1;
    }

    int buffer_size = (int) atoi(argv[1]);
    char *file_name = "input.txt";

    int file_length = get_file_length(file_name);
    if (file_length == -1) {
        return 1;
    }

    mesh_initialize(buffer_size, file_length);
}
