#ifndef ZAD1_LIB_H
#define ZAD1_LIB_H

typedef struct Array {
    char** block_array;
    int array_size;
    int block_size;
    char* current_dir;
    char* target;
    char* temp_file;
} Array;

Array* create(int numberOfBlock);

int add_block(Array* array, char* block);

int remove_block(Array* array, int index);

int set_dir(Array* array, char* dirname);

int set_target(Array* array, char* filename);

int set_temp(Array* array, char* filename);

int find(Array* array);

int read_temp(Array* array);

int delete_array(Array* array);

#endif
