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

void teardown();
void init_belt(belt_t**, cargo_t***, int, int);
void init_semaphore();
void sigint_handler(int);
void load_cargo(belt_t*, cargo_t**);

int shm_id;
int sem_id;
truck_t *truck;

int main(int argc, char **argv) {
  signal(SIGINT, sigint_handler);
  if(argc < 4) {
    fprintf(stderr, "Not enough arguments");
    return 1;
  }
  atexit(teardown);

  truck = malloc(sizeof(truck_t));
  truck->max_weight = atoi(argv[1]);

  int belt_max_load = atoi(argv[2]);
  int belt_max_weight = atoi(argv[3]);
  belt_t *belt;
  cargo_t **loaded;
  
  init_belt(&belt, &loaded, belt_max_load, belt_max_weight);
  init_semaphore();

  while(1) {
    load_cargo(belt, loaded);
  }
  return 0;
}

void teardown() {
  shmctl(shm_id, IPC_RMID, NULL);
  semctl(sem_id, 1, IPC_RMID, NULL);
}

void init_belt(belt_t **belt, cargo_t ***cargo, int max_load, int max_weight) {
  int key = ftok(getenv("HOME"), 0);
  if((shm_id = shmget(key, sizeof(belt_t) + max_load*sizeof(cargo_t), IPC_CREAT | IPC_EXCL | 0666)) == -1) 
    error_exit();
  
  char *shared_memory;
  if(*(shared_memory = shmat(shm_id, NULL, 0)) == -1) 
    error_exit();

  (*belt) = (belt_t*) shared_memory;
  (*cargo) = malloc(sizeof(cargo_t*)*max_load);
  (*belt)->current_load = 0;
  (*belt)->current_weight = 0;
  (*belt)->max_load = max_load;
  (*belt)->max_weight = max_weight;
  int i;
  for(i = 0; i < max_load; i++) {
    (*cargo)[i] = (cargo_t*) (shared_memory + sizeof(belt_t) + (i * sizeof(cargo_t)));
    (*cargo)[i]->pid = 0;
    (*cargo)[i]->time = 0;
    (*cargo)[i]->weight = 0;
  }
}

void init_semaphore() {
  union semun {
    int val;
    struct semid_ds *buf;
    ushort array [1];
  } sem_attr;

  int key = ftok(getenv("HOME"), 1);
  if((sem_id = semget(key, 1, IPC_CREAT | IPC_EXCL | 0666)) == -1)
    error_exit();

  sem_attr.val = 1;
  if(semctl (sem_id, 0, SETVAL, sem_attr) == -1) {
    error_exit();
  }
}

void sigint_handler(int signum) {
  printf("Got SIGINT. Ending trucker\n");
  exit(0);
}

void load_cargo(belt_t *belt, cargo_t **cargo) {
  if(belt->current_load > 0) {
    take_semaphore();
    cargo_t *loaded = cargo[0];

    if(loaded->weight > truck->max_weight) {
      printf("Cargo is too heavy to load\n");
    }
    else {
      if(truck->current_weight + loaded->weight > truck->max_weight) {
        printf("Truck cannot carry any more cargo - fully loaded\n");
        truck->current_weight = 0;
        printf("Fresh truck inbound\n");
      }

      truck->current_weight += loaded->weight;
      belt->current_weight -= loaded->weight;
      printf("Cargo loaded\n");
      printf("loader id: %d, loading time: %lu, units:%d\n", loaded->pid, microseconds() - loaded->time, loaded->weight);
      printf("truck carry status: %d/%d weight units taken\n", truck->current_weight, truck->max_weight);
    }

    belt->current_load--;
    int i;
    for(i = 0; i < belt->current_load - 1; i++) {
      cargo[i] = cargo[i+1];
    }
    release_semaphore();
  }
}

