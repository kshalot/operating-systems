#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>

int status = 0;

void handle_sigtstp(int signum) {
    if(status == 0) {
        printf("Oczekuję na CTRL+Z - kontynuacja albo CTRL+C - zakończenie programu\n");
        status = 1;
    }
    else {
        status = 0;
    }
}

void handle_sigint(int signum) {
    printf("Odebrano sygnal SIGINT\n");
    exit(0);
}


int main(int argc, char **argv) {
        
    time_t current_time;
    signal(SIGTSTP, handle_sigtstp);

    struct sigaction action;
    action.sa_handler = handle_sigint;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    while(1) {
        if(status == 0) {
            time(&current_time);
            printf("%s", asctime(localtime(&current_time)));
        }
    }
}