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

struct tm *reference_date;
char mode;

int compare_times(struct tm *x, struct tm *y) {
    if(x->tm_year == y->tm_year && x->tm_mon == y->tm_mon && x->tm_mday == y->tm_mday) return 0;
    if(x->tm_year > y->tm_year) return 1;
    if(x->tm_year == y->tm_year && x->tm_mon > y->tm_mon) return 1;
    if(x->tm_year == y->tm_year && x->tm_mon == y->tm_mon && x->tm_mday > y->tm_mday) return 1;
    return -1;
}

int print_file(struct stat file_stat, const char *path) {
    if((mode == '<' && compare_times(reference_date, localtime(&(file_stat.st_mtime))) == -1) ||
       (mode == '=' && compare_times(reference_date, localtime(&(file_stat.st_mtime))) == 0) ||
       (mode == '>' && compare_times(reference_date, localtime(&(file_stat.st_mtime))) == 1)) {
        printf("path: %s\n", path);
        printf("type: ");
        switch(file_stat.st_mode & S_IFMT) {
            case S_IFREG:
                printf("file");
                break;
            case S_IFDIR:
                printf("dir");
                break;
            case S_IFBLK:
                printf("block");
                break;
            case S_IFIFO:
                printf("fifo");
                break;
            case S_IFLNK:
                printf("symlink");
                break;
            case S_IFCHR:
                printf("char");
                break;
            case S_IFSOCK:
                printf("socket");
                break;
            default:
                printf("Unkown");
        }
        printf("\nsize: %ld\n", file_stat.st_size);
        printf("Access time: %s", ctime(&file_stat.st_atime));
        printf("Modification time: %s\n", ctime(&file_stat.st_mtime));
    }
    return 0;
}

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
                    printf("%d\n", getppid());
                    execlp("/bin/ls", "ls", "-l", entry_path, NULL);
                }
                else {
                    wait(&status);
                }
                search_directory(entry_path);
            }
            else {
                // print_file(file_stat, entry_path);
            }
        }
        free(entry_path);
    }
    return 0;
}

int nftw_fn(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    if(!S_ISDIR(sb->st_mode))
        print_file(*sb, fpath);
    return 0;
}

int main(int argc, char **argv) {
    if(argc < 5) {
        fprintf(stderr, "Too few arguments\n");
        exit(-1);
    }
    char *path = argv[1];
    char *absolute_path = realpath(path, NULL);
    
    char *unparsed_date = argv[2];
    mode = argv[3][0];
    char *walk_type = argv[4];

    reference_date = malloc(sizeof(struct tm));
    strptime(unparsed_date, "%Y-%m-%d", reference_date);

    if(strcmp(walk_type, "stat") == 0) {
        search_directory(absolute_path);
    }
    else if(strcmp(walk_type, "nftw") == 0) {
        nftw(absolute_path, &nftw_fn, 100, FTW_PHYS);
    }
    else {
        fprintf(stderr, "Invalid walk type! Use \"nftw\" or \"stat\"");
    }
}