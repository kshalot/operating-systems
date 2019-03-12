#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

Array* create(int blocks) {
    if (blocks < 0)
        return NULL;
    Array* result = calloc(1, sizeof(Array));
    result -> block_array = calloc(blocks, sizeof(char*));
    result -> array_size = blocks;

    return result;
}

int add_block(Array* array, char* block) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    int index = 0;
    while(array -> block_array[index] != NULL) {
        index += 1;
    }
    array -> block_array[index] = calloc(array -> block_size, sizeof(char));
    strcpy(array -> block_array[index], block);
    return index;
}

int remove_block(Array* array, int index) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    if(index >= array -> array_size || index < 0) {
        fprintf(stderr, "Index out of bounds\n");
        exit(1);
    }
    if(array -> block_array[index] == NULL) {
        return -1;
    }

    free(array -> block_array[index]);
    array -> block_array[index] = NULL;
    return 0;
}

int set_dir(Array* array, char* dirname) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    array -> current_dir = dirname;
    return 0;
}

int set_target(Array* array, char* filename) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    array -> target = filename;
    return 0;
}

int set_temp(Array* array, char* filename) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised blocka array\n");
        exit(1);
    }
    array -> temp_file = filename;
    return 0;
}

int find(Array* array) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    int bufferLength = 32 + strlen(array -> current_dir) + strlen(array -> target);
    char command[bufferLength];
    sprintf(command, "find %s -type f -name %s > %s", array -> current_dir,
     array -> target, array -> temp_file);
    return system(command);
}

int read_temp(Array* array) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    FILE* fp = fopen(array -> temp_file, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error while reading \"%s\" file", array->temp_file);
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    unsigned long size = ftell(fp);
    printf("Size of block: %lu\n", size);
    fseek(fp, 0L, SEEK_SET);
    char* block = calloc(size, sizeof(char));
    array -> block_size = size;
    if(block) {
        fread(block, 1, size, fp);
    }
    else {
        fprintf(stderr, "Error allocating memory for block\n");
        exit(1);
    }
    fclose(fp);
    add_block(array, block);

    return 0;
}

int delete_array(Array* array) {
    if(array == NULL) {
        return 1;
    }
    int i;
    for(i = 0; i < array -> array_size; i++) {
        remove_block(array, i);
    }
    free(array -> block_array);
    array -> block_array = NULL;
    return 0;
}
