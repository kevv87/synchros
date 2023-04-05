#include "structures.c"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>



struct shm_caracter Create_caracter(int dir){
    struct shm_caracter caracter;
    time_t current_time =time(NULL);
    // Seed the random number generator with the current time
    srand(time(NULL));

    caracter.value= rand() % (127 + 1 - 33) + 33;
    printf("Original value: %d\n", caracter.value);
    caracter.value=caracter.value^70;
    printf("XOR value: %d\n", caracter.value);
    
    
    caracter.datetime= *localtime(&current_time);
    //caracter.buffer_idx= rand() % 300;
    caracter.buffer_idx= 1;
    caracter.file_idx= dir;
    return caracter;
}

int getFile(){
    int fd = open("data.txt", O_WRONLY, 0644);
     if (fd == -1) {
        perror("Error opening file");
        return 1;
    }
    return fd;
}

int writeFile(int fd, off_t offset, char value){
    //off_t offset = 10; // write to the 10th byte of the file
    if (lseek(fd, offset, SEEK_SET) == -1) {
        perror("Error setting file position");
        close(fd);
        return 1;
    }

    char* str = &value;
    size_t len = sizeof(*str);
    printf("size of str %zu\n", len);
    if (write(fd, str, len) != len) {
        perror("Error writing to file");
        close(fd);
        return 1;
    }

    // close the file descriptor when we are finished with it
    close(fd);
    return 0;
}


void receptor(int mode,  int key){
    char asciiValue;
    int getValue;

    struct shm_caracter c1= Create_caracter(2);
    //Make XOR to decrypt
    getValue= c1.value ^ key;
    //convert value to ascii
    printf("Pre--- value %d\n", getValue);
    asciiValue = (char) getValue;
    printf("ASCII value %c\n", asciiValue);
    int fd = getFile();
    off_t offset = c1.file_idx;
    writeFile(fd, offset , asciiValue);

}

int main(){

    /* struct shm_caracter c1= Create_caracter(1);
    printf("Value: %d\n", c1.value);
    printf("Buffer index: %d\n",c1.buffer_idx);
    printf("File index: %d\n",c1.file_idx);
    
    char datetime_str[50];
    strftime(datetime_str, sizeof(datetime_str), "%c", &c1.datetime);
    printf("Datetime: %s\n", datetime_str); */

    receptor(0, 70);


    return 0;
}