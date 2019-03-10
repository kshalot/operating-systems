#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/times.h>

#ifndef DLL
#include "lib.h"
#endif


double time_elapsed(clock_t begin, clock_t end) {
    return (double) (end - begin) / sysconf(_SC_CLK_TCK);
}

void print_time(clock_t real_start, clock_t real_end, struct tms tms_start, struct tms tms_end) {
    printf("Real:   %.2lf s   ", time_elapsed(real_start, real_end));
    printf("User:   %.2lf s   ", time_elapsed(tms_start.tms_utime, tms_end.tms_utime));
    printf("System: %.2lf s\n\n", time_elapsed(tms_start.tms_stime, tms_end.tms_stime));
}

void add_delete_loop(Array* array, int number_of_blocks) {
    if(array == NULL) {
        fprintf(stderr, "Uninitialised block array\n");
        exit(1);
    }
    char* block = "Block text";
    for(int i = 0; i < number_of_blocks; i++) {
        int index = add_block(array, block);
        remove_block(array, index);
    }
}

int main(int argc, char **argv) {

    if(argc == 1) {
        fprintf(stderr, "Too few arguments!\n");
        exit(1);
    }

    struct tms* tms_start = malloc(sizeof(struct tms*));
    struct tms* tms_end = malloc(sizeof(struct tms*));

    clock_t real_start = times(tms_start);
    clock_t real_end;

    Array* array = NULL;

    for(int i = 1; i < argc; i++) {
        if(strcmp(argv[i], "create_table") == 0) {
            if(++i == argc) {
                fprintf(stderr, "Specify table size!\n");
                exit(1);
            }
            printf("Creating array of size %s\n", argv[i]);
            real_start = times(tms_start);
            array = create(atoi(argv[i]));
            real_end = times(tms_end);
        }
        else if(strcmp(argv[i], "search_directory") == 0) {
            if((argc - i - 1) < 3) {
                fprintf(stderr, "Insufficent number of arguments.\nUsage: search_directory (dir) (target) (temp)\n");
                exit(1);
            } 
            printf("Setting dir, target, temp and saving find to temp\n");
            real_start = times(tms_start);
            set_dir(array, argv[++i]);
            set_target(array, argv[++i]);
            set_temp(array, argv[++i]);            
            find(array);
            real_end = times(tms_end);
        }
        else if(strcmp(argv[i], "remove_block") == 0) {
            if((argc - i - 1) < 1) {
                fprintf(stderr, "Insufficent number of arguments.\nUsage: remove_block (index)\n");
                exit(1);
            }
            printf("Removing block\n");
            real_start = times(tms_start);
            remove_block(array, atoi(argv[++i]));
            real_end = times(tms_end);
        }
        else if(strcmp(argv[i], "dump") == 0) {
            printf("Copying temporary file into array\n");
            real_start = times(tms_start);
            read_temp(array);
            real_end = times(tms_end);
        }
        else if(strcmp(argv[i], "add_delete_loop") == 0) {
            if((argc - i - 1) < 1) {
                fprintf(stderr, "Insufficent number of arguments.\nUsage: add_delete_loop (number_of_blocks)\n");
                exit(1);
            }
            real_start = times(tms_start);
            add_delete_loop(array, atoi(argv[++i]));
            printf("Adding and deleting %s blocks\n", argv[i]);
            real_end = times(tms_end);
        }
        else {
            fprintf(stderr, "Unsupported operation: %s\n", argv[i]);
            exit(1);
        }
        print_time(real_start, real_end, *tms_start, *tms_end);
    }
    printf("Freeing array memory\n\n");
    real_start = times(tms_start);
    delete_array(array);
    real_end = times(tms_end);
    print_time(real_start, real_end, *tms_start, *tms_end);
    
    return 0;
}