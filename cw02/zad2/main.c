#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

int search_directory(char *dir_path) {
    DIR *directory = opendir(dir_path);
    if(directory == NULL) {
        fprintf(stderr, "Error opening directory\n");
        exit(-1);
    }
    struct stat file_stat;
    struct dirent *entry;

    while((entry = readdir(directory)) != NULL) {
        if(strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
            continue;
        }
        char *absolute_dir_path = realpath(dir_path, NULL);
        char *entry_path = malloc(strlen(absolute_dir_path) + strlen(entry->d_name) + 1 /*nullbyte*/);
        sprintf(entry_path, "%s/%s", absolute_dir_path, entry->d_name);
        printf("%s\n", entry_path);
        if(lstat(entry_path, &file_stat) == 0) {
            if(S_ISDIR(file_stat.st_mode)) {
                search_directory(entry_path);
            }
        }
        free(entry_path);     
    }
}





int main() {
    search_directory("../..");
}