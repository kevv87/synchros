#ifndef MESH_H
#define MESH_H

void *initialize_shared_memory(char* shared_memory_name, size_t size);

int close_shared_memory(char* shared_memory_name);

#endif