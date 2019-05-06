#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <mqueue.h>
#include "chat.h"

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
  if(mq_send(msqid, (char*)&msg, MSG_SIZE, msg.mtype) == -1) {
    fprintf(stderr, "Sending message: %s\n", strerror(errno));
    exit(-1);
  }
}

message_t receive_message(int msqid) {
  message_t msg;
  if(mq_receive(msqid, (char*)&msg, MSG_SIZE, NULL) == -1) {
    fprintf(stderr, "Receiving message: %s\n", strerror(errno));
    exit(-1);
  }
  return msg;
}
