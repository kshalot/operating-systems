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

//TODO Ma zwracać index i dodawać na pierwsze wolne miejsce!
int addBlock(Array* array, char* block) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        return -1;
    }
    int index = 0;
    while(array->blockArr[index] != NULL) {
        index += 1;
    }
    array -> blockArr[index] = calloc(array->blockSize+1, sizeof(char));
    strcpy(array -> blockArr[index], block);
    return 0;
}

int removeBlock(Array* array, int index) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        return -1;
    }
    if(index >= array -> arraySize || index < 0) {
        fprintf(stderr, "Index out of bounds\n");
        return -1;
    }
    if(array -> blockArr[index] == NULL) {
        fprintf(stderr, "Uninitialised block\n");
        return -1;
    }

    free(array -> blockArr[index]);
    array -> blockArr[index] = NULL;
    return 0;
}

int setDir(Array* array, char* dirname) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        return -1;
    }
    array -> currentDir = dirname;
    return 0;
}

int setTargetFile(Array* array, char* filename) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        return -1;
    }
    array -> targetFile = filename;
    return 0;
}

int find(Array* array) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        return -1;
    }
    int bufferLength = 32 + strlen(array->currentDir) + strlen(array->targetFile);
    char command[bufferLength];
    sprintf(command, "find %s -type f -name %s > temp.txt", array->currentDir, array->targetFile);
    return system(command);
}

int read(Array* array) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        return -1;
    }
    FILE* fp = fopen("temp.txt", "r");
    if (fp == NULL) {
        fprintf(stderr, "Error while reading \"temp.txt\" file");
        return -1;
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
