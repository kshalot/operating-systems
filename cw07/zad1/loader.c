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

int is_overflown(belt_t*, int);
void load_cargo(belt_t*, cargo_t**, int);
void init_belt(belt_t**, cargo_t***);
void init_semaphore();
void print_belt(belt_t*, cargo_t**);

int shm_id;
int sem_id;

int main(int argc, char **argv) { 
  if(argc < 2) {
    fprintf(stderr, "Not enough arguments");
    exit(0);
  }
  int N = atoi(argv[1]);
  int cargo_number = -1;

  // Optional argument
  if(argc > 2)
    cargo_number = atoi(argv[2]);
  
  belt_t *belt;
  cargo_t **loaded;
  init_semaphore();
  init_belt(&belt, &loaded);
  print_belt(belt, loaded);
  if(cargo_number > 0) {
    while(cargo_number > 0) {
      load_cargo(belt, loaded, N);
      cargo_number--;
    }
  }
  else {
    while(1) {
      load_cargo(belt, loaded, N);
    }
  }   
}

int is_overflown(belt_t *belt, int weight) {
  take_semaphore();
  int check_load = belt->current_load + 1 >= belt->max_load; 
  int check_weight = belt->current_weight + weight >= belt->max_weight;

  int overflow = (check_load || check_weight);
  if (overflow)
    release_semaphore();

  return overflow;
}

void print_belt(belt_t *belt, cargo_t **cargo) {
  for(int i = 0; i < belt->max_load; i++) {
    printf("%d, pid: %d, weight: %d, time: %ld\n", i, cargo[i]->pid, cargo[i]->weight, cargo[i]->time);
  }

}

void load_cargo(belt_t *belt, cargo_t **loaded, int weight) {
  if(is_overflown(belt, weight)) {
    printf("Belt is full - waiting for it to be unloaded\n");
    while(is_overflown(belt, weight));
  }
  loaded[belt->current_load]->pid = getpid();
  loaded[belt->current_load]->time = microseconds();
  loaded[belt->current_load++]->weight = weight;
  belt->current_weight += weight;
  time_t rawtime;
  struct tm * timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf("Cargo loaded: %d weight units, loader: %d, on: %s", weight, getpid(), asctime (timeinfo));
  printf("Loaded cargo. Load got from %d to %d\n", belt->current_load - 1, belt->current_load);
  /* printf("cw %d, cl %d, mw %d, ml %d\n", belt->current_weight, belt->current_load, */
  /*        belt->max_weight, belt->max_load); */
  release_semaphore();
}

void init_belt(belt_t **belt, cargo_t ***loaded) {
  int key = ftok(getenv("HOME"), 0);
  if((shm_id = shmget(key, 0, 0666)) == -1) 
    error_exit();
  
  char *shared_memory;
  if(*(shared_memory = shmat(shm_id, NULL, 0)) == -1) 
    error_exit();
  
  (*belt) = (belt_t*) shared_memory;
  printf("cw %d, cl %d, mw %d, ml %d\n", (*belt)->current_weight, (*belt)->current_load,
         (*belt)->max_weight, (*belt)->max_load);
  (*loaded) = malloc(sizeof(cargo_t)*(*belt)->max_load);

  int i;
  for(i = 0; i < (*belt)->max_load; i++) {
    (*loaded)[i] = (cargo_t*) (shared_memory + sizeof(belt_t) + i * sizeof(cargo_t));
  }
}

void init_semaphore() {
  int key = ftok(getenv("HOME"), 1);
  if((sem_id = semget(key, 1, 0666)) == -1) {
    fprintf(stderr, "No trucker working. Start \"trucker\" first\n");
    exit(1);
  }
}
