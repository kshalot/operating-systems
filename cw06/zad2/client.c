#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include "chat.h"

void teardown();
void sigint_handler(int);
void parse_input(char*, size_t);
void send_echo(char*, size_t);
void send_list(void);
void send_to_all(char*, size_t);
void send_to_one(char*);
void send_friends(char*, size_t);
void send_add_friends(char*, size_t);
void send_del_friends(char*, size_t);
void send_to_friends(char*, size_t);

mqd_t mqd;
mqd_t client_mqd;
int client_id;
pid_t receiving_child_pid;

int main(int argc, char **argv) {

  atexit(teardown);
  char *message_text = malloc(MAX_MSG_LENGTH * sizeof(char));
  size_t max_len = MAX_MSG_LENGTH;
  struct mq_attr attr;
  attr.mq_maxmsg = MAX_MSG_NUMBER;
  attr.mq_msgsize = MSG_SIZE;
  char name[32];
  sprintf(name, "/%d", getpid());
  if((client_mqd = mq_open(name, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) == -1) {
    fprintf(stderr, "Opening client queue: %s\n", strerror(errno));
    return -1;
  }
  if((mqd = mq_open(SERVER, O_WRONLY)) == -1) {
    fprintf(stderr, "Opening server queue: %s\n", strerror(errno));
    return -1;
  }
  message_t recv;
  send_message(mqd, INIT, getpid(), 0, NULL);
  recv = receive_message(client_mqd);
  client_id = recv.message_length;
  printf("Estabilished connection as client no. %d\n", client_id);

  if(fork() == 0) {
    while(1) {
      recv = receive_message(client_mqd);
      if(recv.mtype == ECHO)
        printf("%.*s", (int)recv.message_length, recv.message_text);
      else if(recv.mtype == STOP)
        exit(0);
    }
  }
  else {
    signal(SIGINT, sigint_handler);
    if(argc > 1) {
      FILE *fd;
      if((fd = fopen(argv[1], "r")) != NULL) {
        char path[256];
        fread(path, sizeof(char), 256, fd);
        char *saveptr;
        char *comm = strtok_r(path, "\n", &saveptr);
        while(comm != NULL) {
          char buffer[strlen(comm) + 1];
          int len = sprintf(buffer, "%s\n", comm);
          parse_input(buffer, len);
          comm = strtok_r(NULL, "\n", &saveptr);
        }
      }
    }
    while(1) {
      size_t len = getline(&message_text, &max_len, stdin);
      parse_input(message_text, len);
    }
  }
}

void teardown() {
  printf("Client ending connection\n");
  mq_close(client_mqd);
  char name[32];
  sprintf(name, "/%d", getpid());
  mq_unlink(name);
  send_message(mqd, STOP, client_id, 0, NULL);
  mq_close(mqd);
  kill(receiving_child_pid, SIGKILL);
}

void sigint_handler(int signum) {
  exit(0);
}

void parse_input(char *in, size_t len) {
  if(strncmp(in, "echo", 4) == 0)
    send_echo(in, len - 5);
  else if(strcmp(in, "list\n") == 0)
    send_list();
  else if(strncmp(in, "2all", 4) == 0)
    send_to_all(in, len - 5); 
  else if(strncmp(in, "2one", 4) == 0)
    send_to_one(in);
  else if(strncmp(in, "friends", 7) == 0)
    send_friends(in, len - 8);
  else if(strncmp(in, "add", 3) == 0)
    send_add_friends(in, len - 4);
  else if(strncmp(in, "del", 3) == 0)
    send_del_friends(in, len - 4);
  else if(strncmp(in, "2friends", 8) == 0)
    send_to_friends(in, len - 9);
  else if(strcmp(in, "stop\n") == 0)
    exit(0);
  else
    printf("Invalid command\n");
}

void send_echo(char *msg, size_t len) {
  send_message(mqd, ECHO, client_id, len, msg + 5);
}

void send_list() {
  send_message(mqd, LIST, client_id, 0, NULL);
}

void send_to_all(char *msg, size_t len) {
  send_message(mqd, TO_ALL, client_id, len, msg + 5);
}

void send_to_one(char *msg) {
  strtok(msg, " ");
  char *saveptr;
  char *cmd = strtok_r(msg + 5, " ", &saveptr);
  if(cmd == NULL) {      
    printf("Invalid number of args\n");
    return;
  }
  int recipient = atoi(cmd);
  cmd = strtok_r(NULL, "", &saveptr);
  size_t len = 0;
  if(cmd != NULL)
    len = strlen(cmd);
  send_message(mqd, TO_ONE, recipient, len, msg + 7);
}

void send_friends(char *msg, size_t len) {
  send_message(mqd, FRIENDS, client_id, len, msg + 8); 
}

void send_add_friends(char *msg, size_t len) {
  send_message(mqd, ADD_FRIENDS, client_id, len, msg + 4);
}

void send_del_friends(char *msg, size_t len) {
  send_message(mqd, DEL_FRIENDS, client_id, len, msg + 4);
}

void send_to_friends(char *msg, size_t len) {
  send_message(mqd, TO_FRIENDS, client_id, len, msg + 9);
}
