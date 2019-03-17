#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int generate(int quantity, int size, char* file) {

    unsigned char *buffer = malloc(quantity * size * sizeof(char));

    int i;
    for(i = 0; i < quantity * size; i++) {
        buffer[i] = rand() % 256;
    }
    int fd = open(file, O_CREAT | O_WRONLY, 0777); 
    if(fd == -1) {
        fprintf(stderr, "Error while opening file");
        exit(-1);
    }
    if(write(fd, buffer, quantity * size) < 0) {
        fprintf(stderr, "Error while writing to file");
        exit(-1);
    }
    if(close(fd) == -1) {
        fprintf(stderr, "Error while closing file");
        exit(-1);
    }
    return 0;
}

int sys_copy(int quantity, int size, char* source, char* destination) {

    unsigned char *buffer = malloc(quantity * size * sizeof(char));

    int source_fd = open(source, O_RDONLY);
    int destination_fd = open(destination, O_CREAT | O_RDONLY, 0777);
    if(source_fd == -1|| destination_fd == -1) {
        fprintf(stderr, "Error while opening file");
        exit(-1);
    }

    int i;
    for(i = 0; i < quantity; i++) {
        read(source_fd, buffer, size);
        write(destination_fd, buffer, size);
    }
    if(close(source_fd) == -1 || close(destination_fd) == -1) {
        fprintf(stderr, "Error while closing file");
        exit(-1);
    }
    return 0;
}

int lib_copy(int quantity, int size, char* source, char* destination) {

    unsigned char *buffer = malloc(quantity * size * sizeof(char));

    FILE *source_f = fopen(source, "r");
    FILE *destination_f = fopen(destination, "w");
    if(source_f == NULL || destination_f == NULL) {
        fprintf(stderr, "Error while opening file");
        exit(-1);
    }

    int i;
    for(i = 0; i < quantity; i++) {
        fread(buffer, sizeof(char), size, source_f);
        fwrite(buffer, sizeof(char), size, destination_f);
    }
    if(fclose(source_f) == -1 || fclose(destination_f) == -1) {
        fprintf(stderr, "Error while closing file");
        exit(-1);
    }
    return 0;
}










int main() {
    generate(10, 10, "lol.txt");
    return 0;
}