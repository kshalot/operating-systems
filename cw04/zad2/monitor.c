#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>

typedef struct {
    char *filename;
    pid_t pid;
    int running;
} child_process;

int stop = 0;
int end = 0;

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

int monitor(char *filename, int time, int freq) {
    int copies_made = 0;
    time_t modification_date;
    char *copy_name;
    char *file_content;

    if(get_modification_date(filename, &modification_date) != 0)
        return 0;

    if(get_file_content(filename, &file_content) != 0)
        return 0;

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
                // printf("Copying file %s\n", filename);
                if(get_copy_name(filename, modification_date, &copy_name) != 0)
                    return copies_made;
                if(save_copy(copy_name, file_content) != 0)
                    return copies_made;
                modification_date = last_modification_date;
                copies_made++;
            }
        }
    }
    return copies_made;
}

/*---------------------------------------------------*/
/*----------- SIGNAL PROCESSING FUNCTIONS -----------*/
/*---------------------------------------------------*/

void handle_sigint(int signo) {
    end = 1;
}

void handle_sigusr1(int signo) {
    stop = !stop;
}

int list(child_process *children, int length) {
    if(children == NULL)
        return -1;

    int i;
    for(i = 0; i < length; i++) {
        if(children[i].running == 1){
            printf("Process with id %d is currently monitoring file \'%s\'\n", children[i].pid, children[i].filename);
        }
        else
            printf("Stopped process with id %d is monitoring file '%s'\n", children[i].pid, children[i].filename);
    }
    return 0;
}

int stop_process(child_process *child) {
    if(child == NULL) {
        printf("Child is null\n");
        return -1;
    }

    if(!child->running) {
        printf("%d already stopped\n", child->pid);
        return 1;
    }

    if(kill(child->pid, SIGUSR1) == -1) {
        printf("%d sigusr returned -1\n", child->pid);
        return -1;
    }
    else{
        child->running = 0;
        printf("STOPPED PROCESS %d\n", child->pid);
    }
    return 0;
}

int stop_pid(child_process *children, int length, pid_t pid) {
    if(children == NULL)
        return -1;

    int i;
    child_process *target = NULL;
    for(i = 0; i < length; i++) {
        if(children[i].pid == pid)
            target = &children[i];
    }

    if(target != NULL)
        return stop_process(target);

    return -1;
}

int stop_all(child_process *children, int length) {
    if(children == NULL)
        return -1;

    int i;
    for(i = 0; i < length; i++)
        stop_process(&children[i]);

    return 0;
}

int start_process(child_process *child) {
    if(child == NULL) {
        printf("NULL\n");
        return -1;
    }

    if(child->running) {
        printf("%d already stopped\n", child->pid);
        return 1;
    }

    if(kill(child->pid, SIGUSR1) == -1) {
        return -1;
    }
    else
        child->running = 1;

    return 0;
}

int start_pid(child_process *children, int length, pid_t pid) {
    if(children == NULL)
        return -1;

    int i;
    child_process *target = NULL;
    for(i = 0; i < length; i++) {
        if(children[i].pid == pid)
            target = &children[i];
    }

    if(target != NULL)
        return start_process(target);

    return -1;
}

int start_all(child_process *children, int length) {
    if(children == NULL)
        return -1;

    int i;
    for(i = 0; i < length; i++)
        start_process(&children[i]);

    return 0;
}

int main(int argc, char **argv) {

    if(argc < 3) {
        fprintf(stderr, "Too few arguments!\nUsage: monitor <file> <time>");
        return 1;
    }
    char *listfile = argv[1];
    FILE *fp = fopen(listfile, "r");
    if(fp == NULL) {
        fprintf(stderr, "Error opening file\n");
        return 1;
    }
    int time = atoi(argv[2]);

    fseek(fp, 0L, SEEK_END);
    int list_size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    struct sigaction action;
    action.sa_handler = handle_sigint;
    sigemptyset(&action.sa_mask);
    sigaction(SIGINT, &action, NULL);

    char *paths = malloc(list_size * sizeof(char));
    int path_counter = 0;
    fread(paths, sizeof(char), list_size, fp);

    char *s = paths;
    int lines;
    for (lines=0; s[lines]; s[lines]=='\n' ? lines++ : *s++);
    child_process *children = calloc(lines, sizeof(child_process));

    char *current_path = strtok(paths, " \n");
    while(current_path != NULL) {
        int freq = atoi(strtok(NULL, " \n"));
        pid_t pid = fork();
        if(pid == 0) {
            struct sigaction child_action;
            child_action.sa_handler = handle_sigusr1;
            sigemptyset(&child_action.sa_mask);
            sigaction(SIGUSR1, &child_action, NULL);
            return monitor(current_path, time, freq);
        }
        else {
            child_process *forked = &children[path_counter];
            forked->filename = current_path;
            forked->pid = pid;
        }
        current_path = strtok(NULL, " \n");
        path_counter++;
    }

    // Command + pid
    char cmd[15];
    while(!end) {
        fgets(cmd, 15, stdin);

        if (strcmp(cmd, "LIST\n") == 0) {
            if(list(children, path_counter) == -1)
                printf("An error occurred\n");
        }
        else if (strcmp(cmd, "STOP ALL\n") == 0){
            if(stop_all(children, path_counter) != 0)
                printf("An error occurred\n");
        }        
        else if (strncmp(cmd, "STOP ", 5) == 0){
            int pid = atoi(cmd+5);
            if (pid != 0)
                if(stop_pid(children, path_counter, pid) != 0)
                    printf("An error occurred\n");
        }       
        else if (strcmp(cmd, "START ALL\n") == 0) {
            if(start_all(children, path_counter) != 0)
                printf("An error occurred\n");
        }  
        else if (strncmp(cmd, "START ", 6) == 0) {
            int pid = atoi(cmd + 6);
            if (pid != 0) {
                if(start_pid(children, path_counter, pid) != 0)
                    printf("An error occurred\n");
            }
            else
                fprintf(stderr, "No process with pid: %s\n", cmd + 6);
        }
        else if (strcmp(cmd, "END\n") == 0){
            end = 1;
        }
        else {
            fprintf(stderr, "No such command\n");
        }
    }

    while(path_counter--) {
        int status;
        pid_t pid = wait(&status);
        if(pid > 0)
            printf("pid %d created %d copies\n", pid, WEXITSTATUS(status));
        else {
            fprintf(stderr, "Error changing process status\n");
            return 1;
        }
    }
    return 0;
}