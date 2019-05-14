#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int shm_id;
sem_t *sem_id;
off_t len;

int main(int argc, char **argv) { 
  if(argc < 2) {
    fprintf(stderr, "Not enough arguments");
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
  if(cargo_number > 0) {
    while(cargo_number > 0 && !belt->trucker_done) {
      load_cargo(belt, loaded, N);
      cargo_number--;
    }
  }
  else {
    while(!belt->trucker_done) {
      load_cargo(belt, loaded, N);
    }
  }   
}

int is_overflown(belt_t *belt, int weight) {
  if(belt->trucker_done)
    exit(-1);

  take_semaphore();
  int check_load = belt->current_load + 1 >= belt->max_load; 
  int check_weight = belt->current_weight + weight >= belt->max_weight;

  int overflow = (check_load || check_weight);
  if (overflow)
    release_semaphore();
  return overflow;
}   

void load_cargo(belt_t *belt, cargo_t **loaded, int weight) {
  if(is_overflown(belt, weight)) {
    printf("Belt is full - waiting for it to be unloaded\n");
    while(is_overflown(belt, weight));
  }
  
  loaded[belt->current_load]->pid = getpid();
  loaded[belt->current_load]->time = microseconds();
  loaded[belt->current_load]->weight = weight;
  
  time_t rawtime;
  struct tm *timeinfo;
  time(&rawtime);
  timeinfo = localtime(&rawtime);
  printf("Cargo loaded: %d weight units, loader: %d, on: %s", weight, getpid(), asctime (timeinfo));
  belt->current_load++;
  release_semaphore();
}

void init_belt(belt_t **belt, cargo_t ***loaded) {
  if((shm_id = shm_open(SHM, O_RDWR, 0666)) == -1)
    error_exit();

  char *shared_memory;
  
  if(*(shared_memory = mmap(NULL, sizeof(cargo_t), PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0)) == -1)
    error_exit();

  (*belt) = (belt_t*) shared_memory;
  len = sizeof(belt_t) + (*belt)->max_load * sizeof(cargo_t);

  if(*(shared_memory = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0)) == -1)
    error_exit();
  (*loaded) = malloc(sizeof(cargo_t) * (*belt)->max_load);

  int i;
  for(i = 0; i < (*belt)->max_load; i++)
    (*loaded)[i] = (cargo_t*) (shared_memory + sizeof(belt_t) + i * sizeof(cargo_t));
}

void init_semaphore() {
  if((sem_id = sem_open(SEM, 0666)) == SEM_FAILED) {
    fprintf(stderr, "No trucker working. Start \"trucker\" first\n");
    exit(1);
  }
}
