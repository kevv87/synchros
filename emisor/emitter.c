#include "emitter.h"

/*
 * mesh_register_emitter
 *
 * Esta función crea el semáforo y la memoria compartida necesarios para que los emisores puedan escribir en la red mesh.
 *
 * Retorna un puntero a la memoria compartida.
 *
 * Parámetros:
 *   - Ninguno
 *
 * Errores:
 *   - Si se produce algún error al crear el semáforo, se imprime un mensaje de error y se sale del programa.
 *   - Si se produce algún error al crear la memoria compartida, se imprime un mensaje de error y se sale del programa.
 *   - Si se produce algún error al unir la memoria compartida a un puntero, se imprime un mensaje de error y se sale del programa.
 */
void *mesh_register_emitter() {
    // Creamos el semáforo para controlar el acceso a la memoria compartida
    sem_t *sem = sem_open("/mesh_semaphore", O_CREAT | O_EXCL, 0644, 1);
    if (sem == SEM_FAILED) {
        perror("Error creando semáforo");
        exit(EXIT_FAILURE);
    }

    // Creamos la memoria compartida para almacenar los datos
    int shm_id = shmget(SHM_KEY, sizeof(struct shm_caracter) * MAX_BUFFER_SIZE, IPC_CREAT | IPC_EXCL | 0666);
    if (shm_id < 0) {
        perror("Error creando memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Unimos la memoria compartida a un puntero
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("Error uniendo memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Inicializamos los valores de la memoria compartida a cero
    memset(shm_ptr, 0, sizeof(struct shm_caracter) * MAX_BUFFER_SIZE);

    // Devolvemos el puntero a la memoria compartida
    return shm_ptr;
}

/*
 * Finaliza el emisor y libera la memoria compartida
 *
 * Parámetros:
 * - void *shm_ptr: puntero a la memoria compartida
 *
 * Retorno: ninguno
 */
void mesh_finalize_emitter(void *shm_ptr) {
    // Liberamos la memoria compartida
    if (shmdt(shm_ptr) == -1) {
        perror("Error liberando memoria compartida");
        exit(EXIT_FAILURE);
    }
}

/**
 * Añade un caracter a la memoria compartida. Si no hay espacio disponible,
 * espera a que se libere algún espacio.
 * 
 * @param shm_ptr Puntero a la memoria compartida.
 * @param caracter Caracter a añadir a la memoria compartida.
 * @param sem Puntero al semáforo que controla el acceso a la memoria compartida.
 * @return El índice de la posición en la memoria compartida en la que se añadió el caracter.
 */

int mesh_add_caracter(void *shm_ptr, struct shm_caracter caracter, sem_t *sem) {
    struct shm_caracter *shm_buf = (struct shm_caracter*) shm_ptr;
    int idx = mesh_get_file_idx(shm_ptr);
    if (idx >= 0) {
        // Si hay espacio disponible, añadimos el caracter a la memoria compartida
        shm_buf[idx] = caracter;
        // Mostramos la información del caracter introducido
        printf("%-10d%-10d%-10ld%-10c\n", caracter.index, idx, caracter.timestamp, caracter.value);
        return idx;
    } else {
        // Si no hay espacio disponible, esperamos a que se libere algún espacio
        while (sem_wait(sem) == -1) {
            if (errno != EINTR) {
                perror("Error esperando semáforo");
                exit(EXIT_FAILURE);
            }
        }
        // Una vez que se libera el semáforo, intentamos añadir el caracter de nuevo
        return mesh_add_caracter(shm_ptr, caracter, sem);
    }
}

/**
 * mesh_get_file_idx - Devuelve el índice de la próxima posición libre en la memoria compartida.
 * 
 * @shm_ptr: Puntero a la memoria compartida.
 * 
 * Devuelve el índice de la próxima posición libre en la memoria compartida. Si no hay espacios vacíos,
 * devuelve -1.
 */

int mesh_get_file_idx(void *shm_ptr) {
    struct shm_caracter *shm_buf = (struct shm_caracter*) shm_ptr;
    int i;
    for (i = 0; i < MAX_BUFFER_SIZE; i++) {
        if (shm_buf[i].timestamp == 0) {
            // Si encontramos un espacio vacío, devolvemos su índice
            return i;
        }
    }
    // Si no hay espacios vacíos, devolvemos -1
    return -1;
}

/**
 * mesh_send_file - Envía un archivo a la red mesh
 * @file_path: ruta del archivo a enviar
 * @key: clave de 8 bits para codificar los caracteres del archivo
 *
 * Esta función se encarga de enviar un archivo a la red mesh codificando cada uno
 * de sus caracteres con una clave de 8 bits utilizando la operación XOR y
 * almacenándolos en una memoria compartida. Además, registra el emisor en la red
 * mesh utilizando la función mesh_register_emitter, y libera la memoria
 * compartida utilizada y el semáforo de control al finalizar.
 */


void mesh_send_file(char *file_path, char key) {
    // Registramos el emisor en la red mesh
    void *shm_ptr = mesh_register_emitter();

    // Abrimos el archivo y leemos los datos
    FILE *file = fopen(file_path, "r");
    if (file == NULL) {
        perror("Error abriendo archivo");
        exit(EXIT_FAILURE);
    }
    char buffer[MAX_BUFFER_SIZE];
    int read_count = fread(buffer, 1, MAX_BUFFER_SIZE, file);

    // Cerramos el archivo
    fclose(file);

    // Iniciamos un contador de índice
    int idx = 0;

    // Creamos el semáforo para controlar el acceso a la memoria compartida
    sem_t *sem = sem_open("/mesh_semaphore", 0);
    if (sem == SEM_FAILED) {
        perror("Error abriendo semáforo");
        exit(EXIT_FAILURE);
    }

    // Codificamos los caracteres del buffer con la clave de 8 bits utilizando la operación XOR
    int i;
    for (i = 0; i < read_count; i++) {
        buffer[i] = buffer[i] ^ key;
    }

    // Llenamos los espacios disponibles de forma circular con los caracteres codificados del buffer
    int buffer_idx = 0;
    while (buffer_idx < read_count) {
        // Obtenemos un caracter codificado del buffer
        char caracter = buffer[buffer_idx];

        // Creamos una estructura para almacenar el caracter y su información asociada
        struct shm_caracter shm_char = { .value = caracter, .index = idx, .timestamp = time(NULL) };

        // Añadimos el caracter a la memoria compartida
        mesh_add_caracter(shm_ptr, shm_char, sem);

        // Incrementamos el contador de índice
        idx = (idx + 1) % MAX_BUFFER_SIZE;

        // Incrementamos el índice del buffer
        buffer_idx++;
    }

    // Liberamos la memoria compartida y el semáforo
    mesh_finalize_emitter(shm_ptr);
    sem_close(sem);
}

/**
 * mesh_receive_file - Función para recibir un archivo a través de la red mesh.
 *
 * La función crea un semáforo para controlar el acceso a la memoria compartida.
 * Obtiene el identificador de la memoria compartida utilizando la clave 'key'.
 * Mapea la memoria compartida a una dirección virtual.
 * Abre el archivo de salida indicado en 'output_file_path'.
 * Lee los datos de la memoria compartida y los escribe en el archivo de salida.
 * Cierra el archivo de salida y libera la memoria compartida.
 * Cierra el semáforo.
 *
 * @output_file_path: Ruta y nombre del archivo de salida.
 * @key: Clave de 8 bits utilizada para codificar el archivo antes de enviarlo a través de la red mesh.
 *
 * Returns: Esta función no retorna ningún valor.
 *
 * Errors:
 *   - Si no se puede abrir el semáforo, la función muestra un mensaje de error y termina la ejecución.
 *   - Si no se puede obtener el identificador de la memoria compartida, la función muestra un mensaje de error y termina la ejecución.
 *   - Si no se puede mapear la memoria compartida, la función muestra un mensaje de error y termina la ejecución.
 *   - Si no se puede abrir el archivo de salida, la función muestra un mensaje de error y termina la ejecución.
 *   - Si no se pueden escribir todos los datos en el archivo de salida, la función muestra un mensaje de error y termina la ejecución.
 *   - Si no se puede liberar la memoria compartida, la función muestra un mensaje de error y

*/

void mesh_receive_file(char *output_file_path, char key) {
    // Creamos el semáforo para controlar el acceso a la memoria compartida
    sem_t *sem = sem_open("/mesh_semaphore", 0);
    if (sem == SEM_FAILED) {
        perror("Error al abrir el semáforo");
        exit(EXIT_FAILURE);
    }
    // Obtenemos el identificador de la memoria compartida
    int shm_id = shmget(key, SHM_SIZE, 0);
    if (shm_id < 0) {
        perror("Error al obtener el identificador de la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Mapeamos la memoria compartida a nuestra dirección virtual
    void *shm_ptr = shmat(shm_id, NULL, 0);
    if (shm_ptr == (void*)-1) {
        perror("Error al mapear la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Abrimos el archivo de salida
    FILE *output_file = fopen(output_file_path, "wb");
    if (!output_file) {
        perror("Error al abrir el archivo de salida");
        exit(EXIT_FAILURE);
    }

    // Leemos los datos de la memoria compartida y los escribimos en el archivo de salida
    size_t bytes_written = fwrite(shm_ptr, sizeof(char), SHM_SIZE, output_file);
    if (bytes_written != SHM_SIZE) {
        perror("Error al escribir en el archivo de salida");
        exit(EXIT_FAILURE);
    }

    // Cerramos el archivo de salida y liberamos la memoria compartida
    fclose(output_file);
    if (shmdt(shm_ptr) == -1) {
        perror("Error al liberar la memoria compartida");
        exit(EXIT_FAILURE);
    }

    // Liberamos el semáforo
    if (sem_close(sem) == -1) {
        perror("Error al cerrar el semáforo");
        exit(EXIT_FAILURE);
    }
}