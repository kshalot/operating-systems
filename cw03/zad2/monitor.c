#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

const int mode;

int get_modification_date(char *filename, time_t **container) {
    struct stat *sb;
    if(stat(filename, sb) == 0) {
        *container = &sb->st_mtime;
        return 0;
    }
    return -1;
}

int get_file_content(char *filename, char** content) {
    FILE *fp = fopen(filename, "r");
    if(fp == NULL)
        return -1;

    fseek(fp, 0L, SEEK_END);
    int content_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);
    *content = malloc(content_size * sizeof(char));
    fread(*content, sizeof(char), content_size, fp);
    return fclose(fp);
}

int get_copy_name(char *filename, time_t *modification_date, char **copy_name) {
    char *parsed_date = malloc(21 * sizeof(char));
    strftime(parsed_date, 21, "_%Y-%m-%d_%H-%M-%S", localtime(modification_date));
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

int monitor(char *filename, int time, int freq) {
    int copies_made = 0;
    time_t *modification_date;
    char *copy_name;
    char *file_content;

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

    time_t *last_modification_date;
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
                        if(get_copy_name(filename, modification_date, &copy_name) != 0)
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

    if(argc < 4) {
        fprintf(stderr, "Too few arguments!\nUsage: monitor <file> <time> <mode>");
        exit(1);
    }
    FILE *fp = fopen(argv[1], "r");
    if(fp == NULL) {
        fprintf(stderr, "Error opening file\n");
        exit(1);
    }

}