#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include "chat.h"

void handle_message(message_t*);
void handle_init(message_t*);
void handle_echo(message_t*);
void handle_list(void);
void handle_friends(message_t*);
void handle_add_friends(message_t*);
void handle_del_friends(message_t*);
void handle_to_all(message_t*);
void handle_to_friends(message_t*);
void handle_to_one(message_t*);
void handle_stop(message_t*);
void sigint_handler(int);
void teardown(void);

mqd_t mqd;
mqd_t clients[64];
int client_counter = 0;
int friends[64];
int number_of_friends = 0;

int main(void) {

  printf("Server starting...\n");
  signal(SIGINT, sigint_handler);
  struct mq_attr attr;
  attr.mq_maxmsg = MAX_MSG_NUMBER;
  attr.mq_msgsize = MSG_SIZE;
  if((mqd = mq_open(SERVER, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) == -1) {
    fprintf(stderr, "Starting server: %s\n", strerror(errno));
    return -1;
  }
  atexit(teardown);

  message_t msg;
  while(1) {
    msg = receive_message(mqd);
    handle_message(&msg);
  }
  
  return 0;
}

void teardown() {
  int i;
  for(i = 0; i < client_counter; i++) {
    if(clients[i] != -1) {
      printf("Closing connection to client %d\n", i);
      send_message(clients[i], STOP, 0, 0, NULL);
      mq_close(clients[i]);
    }
  }
  printf("Server shutdown\n");    
  mq_close(mqd);
  mq_unlink(SERVER);
}

// To avoid EEXIST error
void sigint_handler(int signum) {
  exit(0);
}

//====================================================================
// Handlers
//====================================================================

void handle_message(message_t *msg) {
  switch(msg->mtype) {
  case INIT:
    handle_init(msg);
    break;
  case ECHO:
    handle_echo(msg);
    break;
  case LIST:
    handle_list();
    break;
  case FRIENDS:
    handle_friends(msg);
    break;
  case ADD_FRIENDS:
    handle_add_friends(msg);
    break;
  case DEL_FRIENDS:
    handle_del_friends(msg);
    break;
  case TO_ALL:
    handle_to_all(msg);
    break;
  case TO_FRIENDS:
    handle_to_friends(msg);
    break;
  case TO_ONE:
    handle_to_one(msg);
    break;
  case STOP:
    handle_stop(msg);
    break;
  }
}

void handle_init(message_t *recv) {
  char name[32];
  sprintf(name, "/%d", recv->sender);
  if((clients[client_counter] = mq_open(name, O_WRONLY)) == -1) {
    fprintf(stderr, "Error connecting client: %s\n", strerror(errno));
    exit(-1);
  }
  send_message(clients[client_counter], INIT, 0, client_counter, NULL);
  printf("Client %d connected\n", client_counter);
  ++client_counter;
}

void handle_echo(message_t *recv) {
  time_t now;
  time(&now);
  char *message_text = malloc(MAX_MSG_LENGTH * sizeof(char));
  size_t len = sprintf(message_text, "%s%.*s", ctime(&now),
                       (int)recv->message_length, recv->message_text);
  send_message(clients[recv->sender], ECHO, 0, len, message_text);
}

void handle_list() {
  int i;
  printf("Connected clients:\n");
  for(i = 0; i < client_counter; i++)
    if(clients[i] != -1)      
      printf("id: %d, msqid: %d\n", i, clients[i]);
}

void handle_friends(message_t *recv) {
  char *buffer = malloc(recv->message_length);
  memcpy(buffer, recv->message_text, recv->message_length);
  char *str = strtok(buffer, " ");
  number_of_friends = 0;
  while(str != NULL) {
    int is_friend = 0;
    int id = atoi(str);
    int i;
    for(i = 0; i < number_of_friends; i++) {
      if(friends[i] == id)
        is_friend = 1;
    }
    if(!is_friend)
      friends[number_of_friends++] = id;
    str = strtok(NULL, " ");
  }
}

void handle_add_friends(message_t *recv) {
  char *buffer = malloc(recv->message_length);
  memcpy(buffer, recv->message_text, recv->message_length);
  char *str = strtok(buffer, " ");
  while(str != NULL) {
    int is_friend = 0;
    int id = atoi(str);
    int i;
    for(i = 0; i < number_of_friends; i++) {
      if(friends[i] == id)
        is_friend = 1;
    }
    if(!is_friend) {
      friends[number_of_friends] = id;
      number_of_friends++;
    }
    str = strtok(NULL, " ");
  }
}

void handle_del_friends(message_t *recv) {
  char *buffer = malloc(recv->message_length);
  memcpy(buffer, recv->message_text, recv->message_length);
  char *str = strtok(buffer, " ");
  while(str != NULL) {
    int id = atoi(str);
    int i;
    for(i = 0; i < number_of_friends; i++) {
      if(friends[i] == id) {
        int j;
        for(j = i; j < number_of_friends - 1; j++)
          friends[j] = friends[j+1];
        number_of_friends--;
      }
    }
    str = strtok(NULL, " ");
  } 
}

void handle_to_all(message_t *recv) {
  time_t now;
  time(&now);
  char *message_text = malloc(MAX_MSG_LENGTH * sizeof(char));
  size_t len = sprintf(message_text, "%s%.*s", ctime(&now),
                       (int)recv->message_length, recv->message_text);
  int i;
  for(i = 0; i < client_counter; i++)
    if(clients[i] != -1)
      send_message(clients[i], ECHO, 0, len, message_text);
}

void handle_to_friends(message_t *recv) {
  time_t now;
  time(&now);
  char *message_text = malloc(MAX_MSG_LENGTH * sizeof(char));
  size_t len = sprintf(message_text, "%s%.*s", ctime(&now),
                       (int)recv->message_length, recv->message_text);
  int i;
  for(i = 0; i < number_of_friends; i++)
    if(clients[friends[i]] != -1)
      send_message(clients[friends[i]], ECHO, 0, len, message_text);
}

void handle_to_one(message_t *recv) {
  if(clients[recv->sender] != 0) {
    time_t now;
    time(&now);
    char *message_text = malloc(MAX_MSG_LENGTH * sizeof(char));
    size_t len = sprintf(message_text, "%s%.*s", ctime(&now),
                         (int)recv->message_length, recv->message_text);
    send_message(clients[recv->sender], ECHO, 0, len, message_text);
  }
}

void handle_stop(message_t *recv) {
  clients[recv->sender] = -1;
  int i;
  int connected_clients = 0;
  for(i = 0; i < client_counter; i++)
    if(clients[i] != -1)
      connected_clients++;
  if(connected_clients == 0) {
    printf("No connected clients\n");
    exit(0);
  }
}
