#include "../common/structures.c"
#include "../mesh/include/mesh_finalizer.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <wiringPi.h>
#include <stdio.h>

#define BUTTON_PIN 17


void transferredCharacters(struct auditory_info *info){
  printf("From all process there is a total of %d characters transferred in total\n", info->transferred_characters);
  return;
}

void memCharacters(struct auditory_info *info){
  printf("From all process there are still a total of %d characters in shared memory\n", info->characters_in_buffer);
  return;
}

void emisors(struct auditory_info *info){
  printf("From all process there is a total of %d emisors alive and %d in total\n", info->alive_emitters, info->total_emitters);
  return;
}

void receptors(struct auditory_info *info){
  printf("From all process there is a total of %d receptors alive and %d in total\n", info->alive_receptors, info->total_receptors);
  return;
}

void usedMem(struct auditory_info *info){
  printf("All process used a total of %d in memory\n", info->shm_mem_used);
  return;
}

void printInfo(struct auditory_info *info){
  printf("*******************************************************************************************\n");
  transferredCharacters(info);
  memCharacters(info);
  emisors(info);
  receptors(info);
  usedMem(info);
  printf("*******************************************************************************************\n");
}

int finalizer() {
  struct auditory_info info = mesh_finalize(mesh_get_shm_ptr());
  printInfo(&info);
  return 0;
}

int main()
{
  wiringPiSetupGpio(); // initialize wiringPi
  pinMode(BUTTON_PIN, INPUT); // set button pin as input
  while (1) {
    if (digitalRead(BUTTON_PIN) == HIGH) {
       printf("Button pressed!\n");
       delay(100); 
       finalizer();
       return 0;
     }
  }
  return 0;
}

