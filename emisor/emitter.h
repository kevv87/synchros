#ifndef EMITTER
#define EMITTER

/*
Este programa utiliza las siguientes bibliotecas:

- stdio.h: Proporciona funciones para entrada y salida estándar.
- stdlib.h: Proporciona funciones para la gestión de memoria dinámica.
- string.h: Proporciona funciones para la manipulación de cadenas.
- unistd.h: Proporciona funciones para el acceso a recursos del sistema operativo.
- sys/shm.h: Proporciona funciones para la gestión de memoria compartida.
- sys/ipc.h: Proporciona funciones para la gestión de claves de IPC.
- semaphore.h: Proporciona funciones para el manejo de semáforos.
- fcntl.h: Proporciona funciones para el manejo de archivos.
- errno.h: Define una variable externa errno que se utiliza para almacenar información sobre errores.
- time.h: Proporciona funciones para trabajar con fechas y horas.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <semaphore.h>
#include <fcntl.h>
#include <errno.h>
#include <time.h>

/*
Este programa utiliza las siguientes definiciones:

- MAX_BUFFER_SIZE: Define el tamaño máximo del búfer utilizado para leer y escribir en la memoria compartida.
- SHM_KEY: Define la clave utilizada para la memoria compartida.
- SEM_KEY: Define la clave utilizada para el semáforo.
- SHM_SIZE: Define el tamaño de la memoria compartida en bytes.
- SEMAPHORE_NAME: Define el nombre del semáforo utilizado en el programa.
*/

#define MAX_BUFFER_SIZE 1024
#define SHM_KEY 0x1234
#define SEM_KEY 0x5678
#define SHM_SIZE 1024
#define SEMAPHORE_NAME "/mesh_semaphore"

/*
 * Struct que representa un caracter en la memoria compartida.
 * Contiene el valor del caracter, su índice de posición en la memoria
 * compartida, y la hora en la que fue introducido.
 */

struct shm_caracter {
    char value;
    int index;
    time_t timestamp;
};

void *mesh_register_emitter();
void mesh_finalize_emitter(void *shm_ptr);
int mesh_get_file_idx(void *shm_ptr);
int mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter, sem_t *sem);
void mesh_send_file(char *file_path, char key);
void mesh_receive_file(char *output_file_path, char key);

#endif /* EMITTER */
