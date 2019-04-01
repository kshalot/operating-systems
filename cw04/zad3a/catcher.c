#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int signal_count = 0;

void catch_kill(int sig, siginfo_t *info, void *context) {
    if(sig == SIGUSR1)
        signal_count++;
    else if(sig == SIGUSR2) {
        int i;
        for(i = 0; i < signal_count; i++)
            kill(info->si_pid, SIGUSR1);
        kill(info->si_pid, SIGUSR2);
        exit(0);
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Invalid arguments. Usage 'catcher <mode>'\n");
        return -1;
    }
    char *mode = argv[1];

    if(strcmp(mode, "KILL") == 0) {
        sigset_t signals;
        sigfillset(&signals);
        sigdelset(&signals, SIGUSR1);
        sigdelset(&signals, SIGUSR2);
        sigprocmask(SIG_BLOCK, &signals, NULL);

        struct sigaction action;
        action.sa_sigaction = catch_kill;
        sigemptyset(&action.sa_mask);
        action.sa_flags = SA_SIGINFO;

        if(sigaction(SIGUSR1, &action, NULL) == -1 || sigaction(SIGUSR2, &action, NULL) == -1) {
            fprintf(stderr, "An error occurred\n");
            return -1;
        }
    }
    else if(strcmp(mode, "SIGQUEUE") == 0) {

    }
    else if(strcmp(mode, "SIGRT") == 0) {

    }
    else {
        fprintf(stderr, "Invalid mode! Choose KILL/SIGQUEUE/SIGRT\n");
        return -1;
    }

    printf("PID: %d\n", getpid());

    while(1) {
        pause();
    }

    return 0;
}