#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int signal_count;

void handle_kill(int signum) {
        signal_count++;
}

int main(int argc, char **argv) {
    
    if(argc != 4) {
        fprintf(stderr, "Invalid arguments. Usage 'sender <pid> <signal_count> <mode>\n");
        return -1;
    }
    int pid = atoi(argv[1]);
    signal_count = atoi(argv[2]);
    char *mode = argv[3];

    if(strcmp(mode, "KILL") == 0) {
        sigset_t signals;
        sigfillset(&signals);
        sigdelset(&signals, SIGUSR1);
        sigdelset(&signals, SIGUSR1);
        sigprocmask(SIG_BLOCK, &signals, NULL);

        struct sigaction action;
        action.sa_sigaction = handle_kill;
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_SIGINFO;

        if(sigaction(SIGUSR1, &action, NULL) == -1 || sigaction(SIGUSR2, &action, NULL) == -1) {
            fprintf(stderr, "An error occurred\n");
            return -1;
        }
        int i;
        for(i = 0; i < signal_count; i++) {
            kill(pid, SIGUSR1);
        }
        kill(pid, SIGUSR2);
    }
    else if(strcmp(mode, "SIGQUEUE")) {

    }
    else if(strcmp(mode, "SIGRT")) {

    }
    else {
        fprintf("Invalid mode! Choose KILL/SIGRT/SIGQUEUE");
        return -1;
    }

    while(1) {
        pause();
    }

    return 0;
}