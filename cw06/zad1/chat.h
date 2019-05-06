#ifndef CHAT_H_INCLUDED
#define CHAT_H_INCLUDED

#include <unistd.h>

#define PROJ_ID 1
#define MSG_SIZE sizeof(message_t) - sizeof(long)
#define MAX_MSG_LENGTH 256

#define INIT 1
#define ECHO 2
#define LIST 3
#define FRIENDS 4
#define ADD_FRIENDS 5
#define DEL_FRIENDS 6
#define TO_ALL 7
#define TO_FRIENDS 8
#define TO_ONE 9
#define STOP 10

typedef struct message_t {
  long mtype;
  int sender;
  size_t message_length;
  char message_text[MAX_MSG_LENGTH];
} message_t;

message_t create_message(long, int, int, char*);
void send_message(int, int, int, int, char*);
message_t receive_message(int);

#endif
