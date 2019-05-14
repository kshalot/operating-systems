#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDE 


extern int sem_id;
extern int shm_id;

typedef struct {
  int pid;
  unsigned long time;
  int weight;
} cargo_t;

typedef struct {
  int current_weight;
  int current_load;
  int max_load;
  int max_weight;
} belt_t;

typedef struct {
  int max_weight;
  int current_weight;
} truck_t;

void error_exit();
void take_semaphore();
void release_semaphore();
unsigned long microseconds();

#endif
