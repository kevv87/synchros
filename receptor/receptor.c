#include "../common/structures.c"
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
    caracter.value=caracter.value^70;    
    
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
    if (write(fd, str, len) != len) {
        perror("Error writing to file");
        close(fd);
        return 1;
    }

    return 0;
}


void chooseColor(){
    srand(time(NULL));
    int receptorNum= rand() % 6;

    switch(receptorNum){
        case 0:// Set console text color to red
            printf("\033[31m");
            break;

        case 1:// Set console text color to magenta
            printf("\033[35m");

        case 2:// Set console text color to blue
            printf("\033[34m");
            break;

        case 3:// Set console text color to yellow
            printf("\033[33m");

        case 4:// Set console text color to green
            printf("\033[32m");
            break;

        case 5:// Set console text color to cyan
            printf("\033[36m");
            break;
    }
}

void printCharacter(struct shm_caracter character, char asciiValue){
    char datetime_str[50];
    strftime(datetime_str, sizeof(datetime_str), "%c", &character.datetime);

    printf("Caracter leido \n");

    // Print table
    printf("| %-20s | %-15c \n", "Valor del caracter", asciiValue);
    printf("| %-20s | %-15d \n", "Indice de lectura", character.buffer_idx);//Posicion del buffer o del file?
    printf("| %-20s | %-15s \n", "Fecha y hora", datetime_str);
}


int printText(){
    int fd = open("data.txt", O_RDONLY);
     if (fd == -1) {
        perror("Error opening file");
        return 1;
    }

    char buffer[100];
    ssize_t n = read(fd, buffer, 100);

    if (n == -1) {
        printf("Failed to read from file\n");
        return 1;
    }

    printf("File contents:\n");
    printf("%.*s", (int)n, buffer);
    printf("\n");
    
    // close the file descriptor when we are finished with it
    close(fd);

    return 0;
}


void receptor(int mode,  int key){

    if(mode == 0){ 
        //Add logic to make it automatic
    }
    else{
        while(1){//Adjust the condition. Needs to be cycled until there is a signal.
            while(1){
                printf("Press Enter to execute the function.\n");
                int key= getchar();
                if(key==10){
                    break;
                } 
            }
            char asciiValue;
            int getValue;

            struct shm_caracter c1= Create_caracter(2);
            //Make XOR to decrypt
            getValue= c1.value ^ key;
            //convert value to ascii
            //////printf("Pre--- value %d\n", getValue);
            asciiValue = (char) getValue;
            /////printf("ASCII value %c\n", asciiValue);
            int fd = getFile();
            off_t offset = c1.file_idx;
            writeFile(fd, offset , asciiValue);
            printCharacter(c1, asciiValue);
            printText();
        }
    }   

}

int main(){
    chooseColor();
    receptor(1, 70);

    return 0;
}

