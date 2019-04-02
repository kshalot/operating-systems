#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int signal_count = 0;

void sigsetup(void (*f) (int, siginfo_t*, void*)) {
    sigset_t signals;
    sigfillset(&signals);
    sigdelset(&signals, SIGUSR1);
    sigdelset(&signals, SIGUSR2);
    sigprocmask(SIG_BLOCK, &signals, NULL);

    struct sigaction action;
    action.sa_sigaction = f;
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;

    if(sigaction(SIGUSR1, &action, NULL) == -1 || sigaction(SIGUSR2, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred\n");
        exit(-1);
    }
}

void rt_sigsetup(void (*f) (int, siginfo_t*, void*)) {
    sigset_t signals;
    sigfillset(&signals);
    sigdelset(&signals, SIGRTMIN);
    sigdelset(&signals, SIGRTMAX);

    sigprocmask(SIG_BLOCK, &signals, NULL);

    struct sigaction action;
    action.sa_sigaction = f;
    action.sa_flags = SA_SIGINFO;

    sigemptyset(&action.sa_mask);
    sigaddset(&action.sa_mask, SIGRTMIN);
    sigaddset(&action.sa_mask, SIGRTMAX);

    if(sigaction(SIGRTMIN, &action, NULL) == -1 || sigaction(SIGRTMAX, &action, NULL) == -1) {
        fprintf(stderr, "An error occurred\n");
        exit(-1);
    }    
}

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

void catch_queue(int sig, siginfo_t *info, void *context) {
    if(sig == SIGUSR1)
        signal_count++;
    else if(sig == SIGUSR2) {
        int i;
        for(i = 0; i < signal_count; i++) {
            union sigval val = {i};
            if(sigqueue(info->si_pid, SIGUSR1, val) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
        kill(info->si_pid, SIGUSR2);
        exit(0);
    }
}

void catch_rt(int sig, siginfo_t *info, void *context) {
    if(sig == SIGRTMIN)
        signal_count++;
    else {
        int i;
        for(i = 0; i < signal_count; i++) {
            if(kill(info->si_pid, SIGRTMIN) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
        if(kill(info->si_pid, SIGRTMAX) != 0) {
            fprintf(stderr, "An error occurred\n");
            exit(-1);
        }
        exit(0);
    }
}

int main(int argc, char **argv) {
    if(argc != 2) {
        fprintf(stderr, "Invalid arguments. Usage 'catcher <mode>'\n");
        return -1;
    }
    char *mode = argv[1];

    if(strcmp(mode, "KILL") == 0)
        sigsetup(catch_kill);
    else if(strcmp(mode, "SIGQUEUE") == 0)
        sigsetup(catch_queue);
    else if(strcmp(mode, "SIGRT") == 0)
        rt_sigsetup(catch_rt);
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