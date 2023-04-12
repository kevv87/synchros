#include <stdio.h>

#include "test_common.h"
#include "mesh_finalizer.h"

int main() {
    printf("\n### Running auxiliary finalizer process!\n");
    
    void *shm_ptr = mesh_get_shm_ptr();
    mesh_finalize(shm_ptr);

    printf("### Succesfully finalized shared memory, tests wont work now!\n\n");
    return 0;
}
