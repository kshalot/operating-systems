#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

int main(int argc, char **argv) {

    if(argc < 5) {
        fprintf(stderr, "Too few arguments!\nUsage: monitor <file> <pmin> <pmax> <bytes>");
        return 1;
    }
    srand(time(NULL));

    char *filename = argv[1];
    int pmin = atoi(argv[2]);
    int pmax = atoi(argv[3]);
    int bytes = atoi(argv[4]);

    pid_t pid = fork();
    if(pid == 0) {
        int hold_time = rand() % (pmax - pmin + 1) + pmin;
        char *random = malloc(bytes * sizeof(char));
        char *date = malloc(21 * sizeof(char));

        int duration = 30;
        while(duration > 0) {
            sleep(hold_time);
            FILE *file = fopen(filename, "a");
            if(file == NULL) {
                fprintf(stderr, "Error opening file\n");
                return 1;
            }
            time_t current_time = time(NULL);
            for(int i = 0; i < bytes; i++)
                random[i] = rand() % 57 + 65;
            strftime(date, 21, "%Y-%m-%d_%H-%M-%S", localtime(&current_time));
            fprintf(file, "pid: %d seconds: %d date: %s random string: %s\n", getpid(), hold_time, date, random);
            fclose(file);
            duration -= hold_time;
        }
    }
    return 0;
}