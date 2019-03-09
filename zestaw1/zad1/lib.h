#ifndef ZAD1_LIB_H
#define ZAD1_LIB_H

#define STATIC_ARRAY_SIZE 1000
#define STATIC_BLOCK_SIZE 100

typedef struct Array {
    char** blockArr;
    int arraySize;
    int blockSize;
    char* currentDir;
    char* targetFile;
} Array;

Array* create(int numberOfBlock);

int addBlock(Array* array, char* block);

int removeBlock(Array* array, int index);

int setDir(Array* array, char* dirname);

int setTargetFile(Array* array, char* filename);

int find(Array* array);

int read(Array* array);

#endif
