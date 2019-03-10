#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lib.h"

Array* create(int numberOfBlocks) {
    if (numberOfBlocks < 0)
        return NULL;
    Array* result = calloc(1, sizeof(Array));
    result -> blockArr = calloc(numberOfBlocks, sizeof(char*));
    result -> arraySize = numberOfBlocks;

    return result;
}

int addBlock(Array* array, char* block) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    int index = 0;
    while(array->blockArr[index] != NULL) {
        index += 1;
    }
    array -> blockArr[index] = calloc(array->blockSize+1, sizeof(char));
    strcpy(array -> blockArr[index], block);
    return index;
}

int removeBlock(Array* array, int index) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    if(index >= array -> arraySize || index < 0) {
        fprintf(stderr, "Index out of bounds\n");
        exit(1);
    }
    if(array -> blockArr[index] == NULL) {
        // fprintf(stderr, "Uninitialised block\n");
        return -1;
    }

    free(array -> blockArr[index]);
    array -> blockArr[index] = NULL;
    return 0;
}

int setDir(Array* array, char* dirname) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    array -> currentDir = dirname;
    return 0;
}

int setTargetFile(Array* array, char* filename) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    array -> targetFile = filename;
    return 0;
}

int setTemp(Array* array, char* filename) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised blocka array\n");
        exit(1);
    }
    array -> tempFile = filename;
    return 0;
}

int find(Array* array) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    int bufferLength = 32 + strlen(array->currentDir) + strlen(array->targetFile);
    char command[bufferLength];
    sprintf(command, "find %s -type f -name %s > %s", array->currentDir,
     array->targetFile, array->tempFile);
    return system(command);
}

int read(Array* array) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    FILE* fp = fopen(array -> tempFile, "r");
    if (fp == NULL) {
        fprintf(stderr, "Error while reading \"%s\" file", array->tempFile);
        exit(1);
    }
    fseek(fp, 0L, SEEK_END);
    long size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    char* block = calloc(size, sizeof(char));
    if(block) {
        fread(block, 1, size, fp);
    }
    fclose(fp);
    addBlock(array, block);

    return 0;
}

int deleteArray(Array* array) {
    if(array == NULL) {
        return 1;
    }
    for(int i = 0; i < array->arraySize; i++) {
        removeBlock(array, i);
    }
    free(array -> blockArr);
    array -> blockArr = NULL;
}
