Layout de la memoria compartida:
1. Contexto
2. Semaforos
3. Buffer


El inicializador hace:
1. Inicializa memoria compartida
2. Escribe en memoria compartida:
    1. El buffer
    2. El contexto
        - Tamaño del archivo

El receptor hace:
1. Se registra y le dan puntero a memoria
2. Lee un caracter del buffer
    1. Hace down a semaforo de receptores
    2. Pide un indice para leer
        1. Down a semaforo de indice de lectura de buffer
        2. Guarda el indice
        3. indice modulo
        4. Hace up a semaforo de indice de lectura de buffer
        5. retorna el indice de lectura de buffer
    3. Lee el caracter
    4. Hace up a semaforo de emisores
3. Escribe el caracter en el archivo de salida
    1. Pide un indice para escribir en el archivo de salida
           1. Hace down a semaforo de archivo de salida
           2. Guarda el indice
           3. indice++
           4. Hace up a semaforo de archivo de salida
           5. Retorna el indice
    2. Escribe el caracter

Para auditoria salvamos:
- Cantidad de caracteres transferidos
    - Cada vez que un receptor lee un caracter hacemos up al contador
- Cantidad de caracteres en memoria compartida
    - Cada vez que un emisor escribe un caracter hacemos up al contador
    - Cada vez que un receptor lee un caracter hacemos down al contador
- Cantidad de emisores vivos y totales
    - Cada vez que registramos un emisor hacemos up al contador de vivos y up al de totales
    - Cada vez que un emisor termina hace down al contador de vivos
- Cantidad de receptores vivos y totales
    - Cada vez que registramos un receptor hacemos up al contador de vivos y up al de totales
    - Cada vez que un receptor termina hace down al contador de vivos
- Cantidad de memoria utilizada
    - La inicializamos en startup

Remember that auditory should have its own threads!
