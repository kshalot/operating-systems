#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "utility.h"

void handle_error(char *error_msg, int rval) {
  printf("An error occurred: %s\nErrno: %d, %s",
         error_msg,
         errno,
         strerror(errno));
  exit(rval);
}

struct timeval get_time() {
  struct timeval curr;
  gettimeofday(&curr, NULL);
  return curr;
}

