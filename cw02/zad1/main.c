#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>

int generate(int quantity, int size, char* file) {

    unsigned char *buffer = malloc(quantity * size * sizeof(char));
    srand(time(NULL));

    int i;
    for(i = 0; i < quantity * size; i++) {
        // buffer[i] = rand() % 256;
        buffer[i] = 'A' + (rand() % 26);
    }
    int fd = open(file, O_CREAT | O_WRONLY, 0777); 
    if(fd == -1) {
        fprintf(stderr, "Error while opening file");
        exit(-1);
    }
    if(write(fd, buffer, quantity * size * sizeof(char)) < 0) {
        fprintf(stderr, "Error while writing to file");
        exit(-1);
    }
    if(close(fd) == -1) {
        fprintf(stderr, "Error while closing file");
        exit(-1);
    }
    free(buffer);
    return 0;
}

int sys_copy(int quantity, int size, char* source, char* destination) {

    unsigned char *buffer = malloc(size * sizeof(char));

    int source_fd = open(source, O_RDONLY);
    int destination_fd = open(destination, O_CREAT | O_WRONLY, 0777);
    if(source_fd == -1|| destination_fd == -1) {
        fprintf(stderr, "Error while opening file");
        exit(-1);
    }

    int i;
    for(i = 0; i < quantity; i++) {
        read(source_fd, buffer, size * sizeof(char));
        write(destination_fd, buffer, size * sizeof(char));
    }
    if(close(source_fd) == -1 || close(destination_fd) == -1) {
        fprintf(stderr, "Error while closing file");
        exit(-1);
    }
    free(buffer);
    return 0;
}

int lib_copy(int quantity, int size, char* source, char* destination) {

    unsigned char *buffer = malloc(size * sizeof(char));

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
    free(buffer);
    return 0;
}

int sys_sort(int quantity, int size, char *filename) {
    char *buffer_a = malloc(size * sizeof(char));
    char *buffer_b = malloc(size * sizeof(char));
    int fd = open(filename, O_RDWR);
    if(fd == -1) {
        fprintf(stderr, "Error while opening file");
        exit(-1);
    }
    int i, j;
    for(i = 0; i < quantity - 1; i++) {
        lseek(fd, i * size, SEEK_SET);
        read(fd, buffer_a, size * sizeof(char));
        int min_idx = i;
        char min_char = buffer_a[0];
        for(j = i + 1; j < quantity; j++) {
            lseek(fd, j * size, SEEK_SET);
            read(fd, buffer_b, size * sizeof(char));
            if(min_char > buffer_b[0]) {
                min_idx = j;
                min_char = buffer_b[0];
            }
        }
        lseek(fd, min_idx * size, SEEK_SET);
        read(fd, buffer_b, size * sizeof(char));

        lseek(fd, min_idx * size, SEEK_SET);
        write(fd, buffer_a, size * sizeof(char));

        lseek(fd, i * size, SEEK_SET);
        write(fd, buffer_b, size * sizeof(char));
    }
    if(close(fd) == -1) {
        fprintf(stderr, "Error closing file");
        exit(-1);
    }
    free(buffer_a);
    free(buffer_b);
    return 0;
}

int lib_sort(int quantity, int size, char *filename) {
    char *buffer_a = malloc(size * sizeof(char));
    char *buffer_b = malloc(size * sizeof(char));
    FILE *file = fopen(filename, "r+");
    if(file == NULL) {
        fprintf(stderr, "Error while opening file");
        exit(-1);
    }
    int i, j;
    for(i = 0; i < quantity - 1; i++) {
        fseek(file, i * size, SEEK_SET);
        fread(buffer_a, sizeof(char), size, file);
        int min_idx = i;
        char min_char = buffer_a[0];
        for(j = i + 1; j < quantity; j++) {
            fseek(file, j * size, SEEK_SET);
            fread(buffer_b, sizeof(char), size, file);
            if(min_char > buffer_b[0]) {
                min_idx = j;
                min_char = buffer_b[0];
            }
        }
        fseek(file, min_idx * size, SEEK_SET);
        fread(buffer_b, sizeof(char), size, file);

        fseek(file, min_idx * size, SEEK_SET);
        fwrite(buffer_a, sizeof(char), size, file);

        fseek(file, i * size, SEEK_SET);
        fwrite(buffer_b, sizeof(char), size, file);

        if(ferror(file) != 0) {
            fprintf(stderr, "Error while writing to file");
            exit(-1);
        }
    }
    if(fclose(file) != 0) {
        fprintf(stderr, "Error closing file");
        exit(-1);
    }
    free(buffer_a);
    free(buffer_b);
    return 0;
}

int main() {
    int quantity = 10;
    int size = 10;
    generate(quantity, size, "system.txt");
    sys_sort(quantity, size, "system.txt");
    generate(quantity, size, "lib.txt");
    lib_sort(quantity, size, "lib.txt");
    sys_copy(quantity, size, "system.txt", "syscopy.txt");
    lib_copy(quantity, size, "lib.txt", "libcopy.txt");

    return 0;
}