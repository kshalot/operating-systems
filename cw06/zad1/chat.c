#include "chat.h"
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

message_t create_message(long mtype, int sender, int message_length, char* message_text) {
  message_t msg;
  msg.mtype = mtype;
  msg.sender = sender;
  msg.message_length = message_length;
  if(mtype != INIT && message_length > 0)
    memcpy(msg.message_text, message_text, message_length);
  
  return msg;
}

void send_message(int msqid, int mtype, int sender, int message_length, char* message_text) {
  message_t msg = create_message(mtype, sender, message_length, message_text);
  if(msgsnd(msqid, &msg, MSG_SIZE, 0) == -1) {
    fprintf(stderr, "Sending message: %s\n", strerror(errno));
    exit(-1);
  }
}

message_t receive_message(int msqid) {
  message_t msg;
  if(msgrcv(msqid, &msg, MSG_SIZE, -100, 0) == -1) {
    fprintf(stderr, "Receiving message: %s\n", strerror(errno));
    exit(-1);
  }
  return msg;
}
