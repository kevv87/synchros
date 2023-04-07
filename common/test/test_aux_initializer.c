#include <stdio.h>

#include "test_common.h"
#include "mesh_initializer.h"

int main() {
    printf("\n### Running auxiliary initializer process!\n");
    
    mesh_initialize(TEST_BUFFER_SIZE);

    printf("### Succesfully initialized shared memory, you can run tests now!\n\n");
    return 0;
}
