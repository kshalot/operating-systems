#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Invalid arguments. Usage 'catcher <mode>'");
        return -1;
    }
    if(strcmp(mode, "KILL") == 0) {

    }
    else if(strcmp(mode, "SIGQUEUE") == 0) {

    }
    else if(strcmp(mode, "SIGRT") == 0) {

    }
    else {
        fprintf(stderr, "Invalid mode! Choose KILL/SIGQUEUE/SIGRT\n");
        return -1;
    }

    while(1) {
        pause();
    }

    return 0;
}