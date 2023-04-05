El inicializador hace:
1. Inicializa memoria compartida
2. Escribe en memoria compartida:
    1. El buffer
    2. El contexto
        - Tamaño del archivo

El emisor ocupa:
1. Obtener un indice del archivo inicial para leer
    sincronizadamente
    1. Funcion de obtener un indice de lectura, necesita:
        - El tamaño del archivo total
        Funcion hace:
        1. Down a semaforo de lectura (compartido)
        2. Guarda el indice
        3. Incrementa el indice (compartido)
        4. Up a semaforo de lectura (compartido)
        5. Devuelve indice
2. Agregar un caracter
    1. Down a semaforo de emisores (compartido)
    2. Obtener un indice para escribir en el buffer
        1. Down a semaforo de buffer_idx
        2. Guarda el indice actual
        3. Actualiza el indice con modulo (compartido)
        4. Up a semaforo de buffer_idx
        5. Devuelve el indice
    3. Escribe el caracter en el buffer compartido en el indice
        asignado.
    4. Up a semaforo de receptores (compartido)

