#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

int signal_count = 0;
int expected_count;

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

void handle_kill(int sig, siginfo_t *info, void *context) {
    if(sig == SIGUSR1) {
        signal_count++;
        if(signal_count < expected_count) {
            if(kill(info->si_pid, SIGUSR1) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
        else {
            if(kill(info->si_pid, SIGUSR2) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
    }
    else if(sig == SIGUSR2) {
        printf("Got back %d out of %d signals\n", signal_count, expected_count);
        exit(0);
    }
}

void handle_queue(int sig, siginfo_t *info, void *context) {
    if(sig == SIGUSR1) {
        signal_count++;
        printf("SIGUSR1 no. %d caught\n", info->si_value.sival_int);
        if(signal_count < expected_count) {
            union sigval val = {signal_count};
            if (sigqueue(info->si_pid, SIGUSR1, val) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
        else {
            if(kill(info->si_pid, SIGUSR2) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
    }
    else if(sig == SIGUSR2) {
        printf("Got back %d out of %d signals\n", signal_count, expected_count);
        exit(0);
    }
}

void handle_sigrt(int sig, siginfo_t *info, void *context) {
    if(sig == SIGRTMIN) {
        signal_count++;
        if(signal_count < expected_count) {
            if(kill(info->si_pid, SIGRTMIN) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
        else {
            if(kill(info->si_pid, SIGRTMAX) != 0) {
                fprintf(stderr, "An error occurred\n");
                exit(-1);
            }
        }
    }
    else if (sig == SIGRTMAX) {
        printf("Got back %d out of %d signals\n", signal_count, expected_count);
        exit(0);
    }
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
        sigsetup(handle_kill);
        if(kill(pid, SIGUSR1) != 0) {
            fprintf(stderr, "Signal SIGUSR1 wasn't sent properly\n");
            exit(-1);
        }
    }
    else if(strcmp(mode, "SIGQUEUE") == 0) {
        sigsetup(handle_queue);
        union sigval val = {0};
        if(sigqueue(pid, SIGUSR1, val) != 0) {
            fprintf(stderr, "Signal SIGUSR2 wasn't sent properly\n");
            exit(-1);
        }
    }
    else if(strcmp(mode, "SIGRT") == 0) {
        rt_sigsetup(handle_sigrt);
        if(kill(pid, SIGRTMIN) != 0) {
            fprintf(stderr, "Signal SIGRTMIN wasn't sent properly\n");
            exit(-1);
        }
    }
    else {
        fprintf(stderr, "Invalid mode! Choose KILL/SIGRT/SIGQUEUE\n");
        return -1;
    }

    while(1) {
        pause();
    }

    return 0;
}