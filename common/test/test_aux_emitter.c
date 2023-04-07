#include <stdio.h>

#include "mesh_emitter.h"
#include "test_common.h"

int main() {
    printf("### Running auxiliary emitter!\n\n");
    printf("This process will add some data to the shared buffer\n");

    void *shm_ptr = mesh_register_emitter();
    struct shm_caracter caracter;

    for (int i = 0; i < TEST_BUFFER_SIZE; i++) {
        caracter.value = 'a' + i;
        caracter.buffer_idx = i;
        mesh_add_caracter(shm_ptr, caracter);
    }

    printf("\n### Finished auxiliary emitter! Now receptors can read data!\n\n");
}
