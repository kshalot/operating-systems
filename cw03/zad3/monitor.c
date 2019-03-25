#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/resource.h>
#include <libgen.h>

int mode;

int get_modification_date(char *filename, time_t *container) {
    struct stat sb;
    if(lstat(filename, &sb) == 0) {
        *container = sb.st_mtime;
        return 0;
    }
    return -1;
}

int get_file_content(char *filename, char** content) {
    FILE *fp = fopen(filename, "r");
    if(fp == NULL) {
        return -1;
    }
    fseek(fp, 0L, SEEK_END);
    int content_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    *content = malloc(content_size * sizeof(char));
    fread(*content, sizeof(char), content_size, fp);
    return fclose(fp);
}

int get_copy_name(char *filename, time_t modification_date, char **copy_name) {
    char *parsed_date = malloc(21 * sizeof(char));
    strftime(parsed_date, 21, "_%Y-%m-%d_%H-%M-%S", localtime(&modification_date));
    *copy_name = malloc(strlen(basename(filename)) + 8 /*archive/*/ + 21 /*date and null*/);
    sprintf(*copy_name, "archive/%s%s", basename(filename), parsed_date);
    return 0;
}

int save_copy(char *copy_name, char *content) {
    FILE *fp = fopen(copy_name, "w");
    if(fp == NULL)
        return -1;

    fwrite(content, sizeof(char), strlen(content), fp);
    return fclose(fp);
}

int monitor(char *filename, int time, int freq, rlim_t cpu_limit, rlim_t memory_limit) {
    int copies_made = 0;
    time_t modification_date;
    char *copy_name;
    char *file_content;

    struct rlimit cpu_rlim;
    struct rlimit memory_rlim;

    cpu_rlim.rlim_cur = cpu_limit;
    cpu_rlim.rlim_max = cpu_limit;

    memory_rlim.rlim_cur = memory_limit;
    memory_rlim.rlim_max = memory_limit;

    if(setrlimit(RLIMIT_CPU, &cpu_rlim) != 0)
        return 2;
    if(setrlimit(RLIMIT_AS, &memory_rlim) != 0)
        return 3;

    if(get_modification_date(filename, &modification_date) != 0)
        return 0;

    if(mode == 0) {
        if(get_file_content(filename, &file_content) != 0)
            return 0;
    }
    else {
        pid_t pid = fork();
        if(pid == 0) {
            printf("Copying file: %s\n", filename);
            if(get_copy_name(filename, modification_date, &copy_name) != 0)
                return 0;
            execlp("/bin/cp", "cp", filename, copy_name, NULL);
        }
        copies_made++;
    }
    time_t last_modification_date;
    int delta_time = 0;
    while(time--) {
        sleep(1);
        delta_time++;
        if(delta_time == freq) {
            delta_time = 0;
            if(get_modification_date(filename, &last_modification_date) != 0)
                return copies_made;
            if(last_modification_date != modification_date) {
                printf("Copying file %s\n", filename);
                if(mode == 0) {
                    if(get_copy_name(filename, modification_date, &copy_name) != 0)
                        return copies_made;
                    if(save_copy(copy_name, file_content) != 0)
                        return copies_made;
                }
                else {
                    pid_t pid = fork();
                    if(pid == 0) {
                        if(get_copy_name(filename, last_modification_date, &copy_name) != 0)
                            return copies_made;
                        execlp("/bin/cp", "cp", filename, copy_name, NULL);
                    }
                }
                modification_date = last_modification_date;
                copies_made++;
            }
        }
    }
    return copies_made;
}

int main(int argc, char **argv) {

    if(argc < 6) {
        fprintf(stderr, "Too few arguments!\nUsage: monitor <file> <time> <mode>");
        return 1;
    }
    char *list = argv[1];
    FILE *fp = fopen(list, "r");
    if(fp == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    int time = atoi(argv[2]);
    mode = atoi(argv[3]);
    rlim_t cpu_limit = atoi(argv[4]);
    rlim_t memory_limit = atoi(argv[5]);

    fseek(fp, 0L, SEEK_END);
    int list_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    char *paths = malloc(list_size * sizeof(char));
    int path_counter = 0;
    fread(paths, sizeof(char), list_size, fp);

    char *current_path = strtok(paths, "\n");
    while(current_path != NULL) {
        printf("%s\n", current_path);
        path_counter++;
        //placeholder freq
        int freq = 2;
        pid_t pid = fork();
        if(pid == 0)
            return monitor(current_path, time, freq, cpu_limit, memory_limit);
        current_path = strtok(NULL, "\n");
    }
    while(path_counter--) {
        int status;
        struct rusage before;
        struct rusage after;
        if(getrusage(RUSAGE_CHILDREN, &before)) {
            return 1;
        }
        pid_t pid = wait(&status);
        if(getrusage(RUSAGE_CHILDREN, &after)) {
            return 1;
        }
        if(pid > 0)
            printf("pid %d created %d copies\n", pid, WEXITSTATUS(status));
        else {
            fprintf(stderr, "Error changing process status\n");
            return 1;
        }
        printf("user time: %08ld\n", after.ru_utime.tv_usec - before.ru_utime.tv_usec);
        printf("system time: %08ld\n", after.ru_stime.tv_usec - before.ru_stime.tv_usec);
    }
    return 0;
}