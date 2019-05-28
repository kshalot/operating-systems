#ifndef UTILITY_H_INCLUDED
#define UTILITY_H_INCLUDED

#include <sys/time.h>

typedef struct {
  int id;
  int cart;
} passenger_t;

typedef struct {
  int id;
  int current_cap;
  int fares;
  passenger_t* passengers;
} cart_t;


void handle_error(char*, int);
struct timeval get_time();

#endif
