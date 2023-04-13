#include "mesh_initializer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

int get_file_length(char *file_name) {
    struct stat st;
    if(stat(file_name, &st) == 0) {
        return st.st_size;
    } else {
        printf("Error al obtener el tamaño del archivo %s\n", file_name);
        return -1;
    }
}

void init_file(int input_size){
    int fd; // file descriptor
    char *filename = "output.txt";
    char buffer[input_size];

     memset(buffer, 0, input_size); // fill buffer with blank spaces

    fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644); // create and open file

    if (fd == -1) { // error handling
        perror("open");
        exit(1);
    }
    // write to the file
    write(fd, buffer, input_size);

    // close the file
    close(fd);
} 

int main(int argc, char *argv[]) {
    //TODO: Falta el identificador de espacio compartido
    printf("Inicializando el espacio compartido!\n");

    if (argc != 2) {
        printf("Error: Debe ingresar el tamaño del buffer como argumento\n");
        return 1;
    }

    int buffer_size = (int) atoi(argv[1]);
    char *file_name = "input.txt";
    int file_length = get_file_length(file_name);
    if (file_length == -1) {
        return 1;
    } 

    printf("antes");
    init_file(file_length);

    mesh_initialize(buffer_size, file_length);
    return 0;
}
