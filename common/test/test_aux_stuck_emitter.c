#include <stdio.h>

#include "mesh_emitter.h"
#include "test_common.h"

int main() {
    printf("### Running auxiliary stuck emitter!\n\n");
    printf("This process will add some data to the shared buffer, but it will eventually get stuck!\n");

    void *shm_ptr = mesh_register_emitter();
    struct shm_caracter caracter;

    int i;
    i = mesh_get_file_idx(shm_ptr);
    while(get_heartbeat(shm_ptr) && i < 20) {
        caracter.value = 'a' + i;
        printf("Could get stuck...\n");
        mesh_add_caracter(shm_ptr, caracter);
        printf("Unstuck!\n");
    }

    mesh_finalize_emitter(shm_ptr);
    printf("\n### Finished auxiliary emitter! Now receptors can read data!\n\n");
}
