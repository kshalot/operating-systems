#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <wait.h>

pid_t pid = 0;

void handle_sigtstp(int signum) {
    if(waitpid(pid, NULL, WNOHANG) == 0) {
        printf("Oczekuję na CTRL+Z - kontynuacja, albo CTRL+C - zakończenie programu\n");
        kill(pid, SIGKILL);
    }
    else {
        pid = fork();
        if (pid == 0)
            execl("script.sh", "script.sh", NULL);
    }
}

void handle_sigint(int signum) {
    printf("Odebrano sygnal SIGINT\n");
    if(waitpid(pid, NULL, WNOHANG) == 0)
        kill(pid, SIGKILL);
    exit(0);
}


int main(int argc, char **argv) {
        
    signal(SIGTSTP, handle_sigtstp);

    struct sigaction action;
    action.sa_handler = handle_sigint;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGINT, &action, NULL);

    pid = fork();

    if(pid == 0) {
        execl("script.sh", "script.sh", NULL);
        exit(0);
    }
    while(1);
}