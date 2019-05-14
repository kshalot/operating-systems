#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/ipc.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>

#include "utility.h"

void error_exit() {
  fprintf(stderr, "An error occurred: %s\n", strerror(errno));
  exit(errno);
}

void take_semaphore() {
  if(sem_post(sem_id) == -1) {
    printf("Semaphore take error\n");
    error_exit();
  }
}

void release_semaphore() {
  if(sem_wait(sem_id) == -1) {
    printf("Semaphore release error\n");
    error_exit();
  }
}

unsigned long microseconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * (unsigned long)1000000 + tv.tv_usec;
}
