#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>

int time_comparison(struct tm *x, struct tm *y) {
    // x < y => -1
    // x == y => 0
    // x > y => 1
    if(x->tm_year == y->tm_year && x->tm_mon == y->tm_mon && x->tm_mday == y->tm_mday)
        return 0;
    if(x->tm_year > y->tm_year)
        return 1;
    if(x->tm_year == y->tm_year && x->tm_mon > y->tm_mon)
        return 1;
    if(x->tm_year == y->tm_year && x->tm_mon == y->tm_mon && x->tm_mday > y->tm_mday)
        return 1;
    return -1;
}

int print_file(struct stat file_stat, char *path) {
    printf("%s\n", path);
}

int search_directory(char *dir_path, struct tm *reference_date, char mode) {
    DIR *directory = opendir(dir_path);
    if(directory == NULL) {
        fprintf(stderr, "Error opening directory\n");
        return -1;
    }
    struct stat file_stat;
    struct dirent *entry;
    char *absolute_dir_path = realpath(dir_path, NULL);

    while((entry = readdir(directory)) != NULL) {
        if(strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
            continue;
        }
        char *entry_path = malloc(strlen(absolute_dir_path) + strlen(entry->d_name) + 1 /*nullbyte*/ + 1 /*slash*/);
        sprintf(entry_path, "%s/%s", absolute_dir_path, entry->d_name);
        if(lstat(entry_path, &file_stat) == 0) {
            if(S_ISDIR(file_stat.st_mode)) {
                search_directory(entry_path, reference_date, mode);
            }
            else {
                if((mode == '<' && time_comparison(localtime(&(file_stat.st_mtime)), reference_date) == -1) ||
                   (mode == '=' && time_comparison(localtime(&(file_stat.st_mtime)), reference_date) == 0) ||
                   (mode == '>' && time_comparison(localtime(&(file_stat.st_mtime)), reference_date) == 1)) {
                    print_file(file_stat, entry_path);
                }
            }
        }
        free(entry_path);
    }
    return 0;
}

int main(int argc, char **argv) {
    if(argc < 4) {
        fprintf(stderr, "Too few arguments\n");
        exit(-1);
    }
    char *path = argv[1];
    char *unparsed_date = argv[2];
    char mode = argv[3][0];

    printf("Searching in %s for date %s in mode %c\n", path, unparsed_date, mode);

    struct tm *parsed_date = malloc(sizeof(struct tm));
    strptime(unparsed_date, "%Y-%m-%d", parsed_date);

    search_directory(path, parsed_date, mode);
}