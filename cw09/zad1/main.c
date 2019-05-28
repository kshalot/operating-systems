#include <stdio.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "utility.h"

int number_of_carts;
int cart_cap;

pthread_cond_t* cart_cond;
pthread_mutex_t* cart_mutex;
pthread_mutex_t empty_cart_mutex;
pthread_mutex_t full_cart_mutex;

pthread_mutex_t boarding_mutex;
pthread_mutex_t passenger_mutex;

pthread_cond_t empty_cond;
pthread_cond_t full_cond;

cart_t* carts;
int current_cart_id;


void *passenger_thread(void *data) {
  passenger_t *passenger = (passenger_t*) data;
  while (1) {
    pthread_mutex_lock(&passenger_mutex);

    passenger->cart = current_cart_id;
    carts[current_cart_id].passengers[carts[current_cart_id].current_cap++] = *passenger;
    struct timeval curr = get_time();
    printf("Passenger %d has entered the cart, currently in cart: %d, time: %ld.%06ld\n",
           passenger->id,
           carts[current_cart_id].current_cap,
           curr.tv_sec,
           curr.tv_usec);

    if(carts[current_cart_id].current_cap == cart_cap) {
      curr = get_time();
      printf("Passenger %d started the cart, time: %ld.%06ld\n",
             carts[current_cart_id].passengers[rand() % carts->current_cap].id,
             curr.tv_sec,
             curr.tv_usec);
      pthread_cond_signal(&full_cond);
      pthread_mutex_unlock(&full_cart_mutex);
    } else {
      pthread_mutex_unlock(&passenger_mutex);
    }

    pthread_mutex_lock(&cart_mutex[passenger->cart]);
    curr = get_time();
    carts[current_cart_id].current_cap--;
    
    printf("Passenger %d left the cart, people left in the cart: %d, time: %ld.%06ld \n",
           passenger->id,
           carts[current_cart_id].current_cap,
           curr.tv_sec,
           curr.tv_usec);

    if(carts[current_cart_id].current_cap == 0){
      pthread_cond_signal(&empty_cond);
      pthread_mutex_unlock(&empty_cart_mutex);
    }
    pthread_mutex_unlock(&cart_mutex[passenger->cart]);
  }
}


void *cart_thread(void *data) {
  cart_t *cart = (cart_t*) data;
  if (cart->id == 0)
    pthread_mutex_lock(&passenger_mutex);

  for (int i = 0; i < cart->fares; i++) {
    pthread_mutex_lock(&boarding_mutex);
    if (cart->id != current_cart_id) {
      pthread_cond_wait(&cart_cond[cart->id], &boarding_mutex);
    }
    struct timeval curr = get_time();
    printf("Cart %d has arrived at %ld.%06ld\n",
           cart->id,
           curr.tv_sec,
           curr.tv_usec);

    if (i != 0) {
      pthread_mutex_unlock(&cart_mutex[cart->id]);
      pthread_cond_wait(&empty_cond, &empty_cart_mutex);
    }

    pthread_mutex_lock(&cart_mutex[cart->id]);
    pthread_mutex_unlock(&passenger_mutex);
    pthread_cond_wait(&full_cond, &full_cart_mutex);
    printf("%d is closing it's door\n", current_cart_id);

    curr = get_time();
    current_cart_id = (current_cart_id + 1) % number_of_carts;

    pthread_cond_signal(&cart_cond[current_cart_id]);
    pthread_mutex_unlock(&boarding_mutex);
  }

  pthread_mutex_lock(&boarding_mutex);

  if(cart->id != current_cart_id) {
    pthread_cond_wait(&cart_cond[cart->id], &boarding_mutex);
  }

  struct timeval curr = get_time();
  printf("Cart %d has arrived at %ld.%06ld\n",
         cart->id, 
         curr.tv_sec,
         curr.tv_usec);

  current_cart_id = cart->id;

  pthread_mutex_unlock(&cart_mutex[cart->id]);
  pthread_cond_wait(&empty_cond,&empty_cart_mutex);

  current_cart_id = (current_cart_id + 1) % number_of_carts;

  curr = get_time();
  printf("Cart %d has finished, time: %ld.%06ld\n",
         cart->id,
         curr.tv_sec,
         curr.tv_usec);

  pthread_cond_signal(&cart_cond[current_cart_id]);
  pthread_mutex_unlock(&boarding_mutex);
  pthread_exit(NULL);
}


void init(int number_of_passengers) {
  carts = malloc(sizeof(cart_t) * number_of_carts + sizeof(int) * number_of_passengers);
  cart_mutex = malloc(sizeof(pthread_mutex_t) * number_of_carts);
  cart_cond = malloc(sizeof(pthread_cond_t) * number_of_carts);

  pthread_mutex_init(&boarding_mutex, NULL);
  pthread_mutex_init(&empty_cart_mutex, NULL);
  pthread_mutex_init(&passenger_mutex, NULL);
  pthread_mutex_init(&full_cart_mutex, NULL);
  pthread_cond_init(&empty_cond, NULL);
  pthread_cond_init(&full_cond, NULL);
}


int main(int argc, char **argv) {
  if (argc < 5)
    handle_error("Too few arguments", -1);

  srand(time(NULL));
  int number_of_passengers = atoi(argv[1]);
  number_of_carts = atoi(argv[2]);
  cart_cap = atoi(argv[3]);
  int number_of_fares = atoi(argv[4]);

  init(number_of_passengers);

  current_cart_id = 0;
  pthread_t passenger_threads[number_of_passengers];
  pthread_t cart_threads[number_of_carts];
  passenger_t passengers[number_of_passengers];

  for (int i = 0; i < number_of_passengers; i++) {
    passengers[i].id = i;
  }

  for (int i = 0; i < number_of_carts; i++) {
    carts[i].id = i;
    carts[i].current_cap = 0;
    carts[i].fares = number_of_fares;
    carts[i].passengers = malloc(sizeof(passenger_t) * cart_cap);
    pthread_mutex_init(&cart_mutex[i], NULL);
    pthread_cond_init(&cart_cond[i], NULL);
  }

  for (int i = 0; i < number_of_carts; i++)
    pthread_create(&cart_threads[i], NULL, cart_thread, &carts[i]);

  for (int i = 0; i < number_of_passengers; i++)
    pthread_create(&passenger_threads[i], NULL, passenger_thread, &passengers[i]);

  for (int i = 0; i < number_of_carts; i++) {
    pthread_join(cart_threads[i], NULL);
    pthread_mutex_destroy(&cart_mutex[i]);
    free(carts[i].passengers);
  }

  free(carts);
  free(cart_mutex);
  free(cart_cond);

  return 0;
}
