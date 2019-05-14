#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/sem.h>
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
  struct sembuf sops;
  sops.sem_num = 0;
  sops.sem_op = -1;
  sops.sem_flg = SEM_UNDO;
  if(semop(sem_id, &sops, 1) == -1)
    error_exit();
}

void release_semaphore() {
  struct sembuf sops;
  sops.sem_num = 0;
  sops.sem_op = 1;
  sops.sem_flg = SEM_UNDO;
  if(semop(sem_id, &sops, 1) == -1)
    error_exit();
}

unsigned long microseconds() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * (unsigned long)1000000 + tv.tv_usec;
}
