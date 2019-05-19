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

void teardown();
void init_belt(belt_t**, cargo_t***, int, int);
void init_semaphore();
void sigint_handler(int);
void load_cargo(belt_t*, cargo_t**);

int shm_id;
sem_t *sem_id;
off_t len;
char *shared_memory;
truck_t *truck;

belt_t *belt;

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
  cargo_t **loaded;
  
  init_belt(&belt, &loaded, belt_max_load, belt_max_weight);
  init_semaphore();

  while(1) {
    load_cargo(belt, loaded);
  }
  return 0;
}

void teardown() {
  sem_close(sem_id);
  sem_unlink(SEM);
  munmap(shared_memory, len);
  shm_unlink(SHM);
}

void init_belt(belt_t **belt, cargo_t ***cargo, int max_load, int max_weight) {
  if((shm_id = shm_open(SHM, O_RDWR | O_CREAT | O_EXCL, 0666)) == -1) 
    error_exit();
  
  len = sizeof(belt_t) + max_load * sizeof(cargo_t);
  if(ftruncate(shm_id, len) == -1)
    error_exit();
  
  if(*(shared_memory = mmap(NULL, len, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0)) == -1) 
    error_exit();

  (*belt) = (belt_t*) shared_memory;
  (*cargo) = malloc(sizeof(cargo_t*)*(max_load + 10));
  (*belt)->current_load = 0;
  (*belt)->current_weight = 0;
  (*belt)->max_load = max_load;
  (*belt)->max_weight = max_weight;
  (*belt)->trucker_done = 0;

  int i;
  for(i = 0; i < max_load; i++) {
    (*cargo)[i] = (cargo_t*) (shared_memory + sizeof(belt_t) + (i * sizeof(cargo_t)));
    (*cargo)[i]->pid = 0;
    (*cargo)[i]->time = 0;
    (*cargo)[i]->weight = 0;
  }
}

void init_semaphore() {
  if((sem_id = sem_open(SEM, O_CREAT | O_EXCL, 0666, 1)) == SEM_FAILED)
     error_exit();
}

void sigint_handler(int signum) {
  printf("Got SIGINT. Ending trucker\n");
  belt->trucker_done = 1;
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

