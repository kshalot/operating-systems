#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <ftw.h>

int search_directory(char *dir_path) {
    DIR *directory = opendir(dir_path);
    if(directory == NULL) {
        fprintf(stderr, "Error opening directory\n");
        return -1;
    }
    struct stat file_stat;
    struct dirent *entry;

    while((entry = readdir(directory)) != NULL) {
        if(strcmp(entry->d_name, "..") == 0 || strcmp(entry->d_name, ".") == 0) {
            continue;
        }
        char *entry_path = malloc(strlen(dir_path) + strlen(entry->d_name) + 1 /*nullbyte*/ + 1 /*slash*/);
        sprintf(entry_path, "%s/%s", dir_path, entry->d_name);
        if(lstat(entry_path, &file_stat) == 0) {
            if(S_ISDIR(file_stat.st_mode)) {
                pid_t pid = fork();
                int status;
                if(pid == 0) {
                    printf("%s\n", entry_path);
                    execlp("/bin/ls", "ls", "-l", entry_path, NULL);
                }
                else {
                    printf("pid: %d\n", pid);
                    wait(&status);
                }
                search_directory(entry_path);
            }
        }
        free(entry_path);
    }
    return 0;
}

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Too few arguments\n");
        exit(-1);
    }
    char *path = argv[1];
    char *absolute_path = realpath(path, NULL);
    search_directory(absolute_path);
}