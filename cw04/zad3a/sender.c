#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int signal_count = 0;
int expected_count;

void handle_kill(int sig, siginfo_t *info, void *context) {
    if(sig == SIGUSR1)
        signal_count++;
    else if(sig == SIGUSR2) {
        printf("Sent %d out of %d signals\n", signal_count, expected_count);
        exit(0);
    }
}

void handle_queue(int sig, siginfo_t *info, void *context) {
    if(sig == SIGUSR1)
        signal_count++;
    else if(sig == SIGUSR2) {

    }
}

void handle_sigrt(int sig, siginfo_t *info, void *context) {
    
}

int main(int argc, char **argv) {
    
    if(argc != 4) {
        fprintf(stderr, "Invalid arguments. Usage 'sender <pid> <signal_count> <mode>\n");
        return -1;
    }
    int pid = atoi(argv[1]);
    expected_count = atoi(argv[2]);
    char *mode = argv[3];

    if(strcmp(mode, "KILL") == 0) {
        sigset_t signals;
        sigfillset(&signals);
        sigdelset(&signals, SIGUSR1);
        sigdelset(&signals, SIGUSR2);
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
        for(i = 0; i < expected_count; i++)
            if(kill(pid, SIGUSR1) != 0) {
                fprintf(stderr, "Signal SIGUSR1 wasn't sent properly\n");
                exit(-1);
            }
        if(kill(pid, SIGUSR2) != 0) {
            fprintf(stderr, "Signal SIGURS2 wasn't sent properly\n");
            exit(-1);
        }
    }
    else if(strcmp(mode, "SIGQUEUE")) {

    }
    else if(strcmp(mode, "SIGRT")) {

    }
    else {
        fprintf(stderr, "Invalid mode! Choose KILL/SIGRT/SIGQUEUE");
        return -1;
    }

    while(1) {
        pause();
    }

    return 0;
}