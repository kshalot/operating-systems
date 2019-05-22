#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include "utility.h"

int ceil_divide(int, int);
int calculate_pixel(int, int);
void fill_column(int);
static void *thread_block();
static void *thread_interleaved(void*);
long subtract_times(struct timeval, struct timeval);

int number_of_threads;
image_t *input;
image_t *output;
filter_t *filter;


int main(int argc, char **argv) {
  if(argc < 6)
    error_exit("Not enough arguments", -1);
  number_of_threads = atoi(argv[1]);
  char *partition_scheme = argv[2];
  char *input_path = argv[3];
  char *filter_path = argv[4];
  char *output_path = argv[5];

  input = open_image(input_path);
  output = open_image(output_path);
  for(int i = 0; i < output->height; i++)
    for(int j = 0; j < output->width; j++)
      output->buffer[i][j] = 0;
  filter = open_filter(filter_path);
  
  pthread_t *threads = malloc(number_of_threads * sizeof(pthread_t));
  for(int i = 0; i < number_of_threads; i++) {
    int *value = malloc(sizeof(int));
    *value = i;
    if(strcmp("block", partition_scheme) == 0)
      pthread_create(threads + i, NULL, &thread_block, value);
    else
      pthread_create(threads + i, NULL, &thread_interleaved, value);
  }

  struct timeval start;
  gettimeofday(&start, NULL);
  long threading_time = 0;
  for(int i = 0; i < number_of_threads; i++) {
    long *operation_time;
    pthread_join(threads[i], (void*) &operation_time);
    printf("Thread %d: %ld microseconds\n", i+1, *operation_time);
    threading_time += *operation_time;
  }
  struct timeval end;
  gettimeofday(&end, NULL);
  printf("Total time passed: %ld microseconds\n", subtract_times(start, end));
  printf("Threading time: %ld microseconds\n", threading_time);

  save_image(output, output_path);
  
  return 0;
}


int ceil_divide(int x, int y) {
  return (x + y - 1) / y; // 1 + ((x - 1) / y)
}


int filter_equation(int a, int b, int threshold) {
  if(a < b)
    return b;
  else
    if(a > threshold)
      return threshold;
    else
      return a;
}


int calculate_pixel(int x, int y) {
  int c = filter->size;
  double pixel = 0;
  for(int i = 0; i < c; i++)
    for(int j = 0; j < c; j++) {
      int a = filter_equation(y - ceil_divide(c, 2) + i - 1,
                              0,
                              input->height - 1);
      int b = filter_equation(x - ceil_divide(c, 2) + j - 1,
                              0,
                              input->width - 1);
      pixel += input->buffer[a][b] * filter->buffer[j][i];
    }

  return round(pixel);
}


void fill_column(int x) {
  for (int i = 0; i < input->height; i++)
    output->buffer[i][x] = calculate_pixel(x, i);
}


static void *thread_block(void *argv) {
  struct timeval start;
  gettimeofday(&start, NULL);

  int k = (*(int*) argv);
  int a = ceil_divide(k * input->width, number_of_threads);
  int b = ceil_divide((k+1) * input->width, number_of_threads);

  for(int i = a; i < b; i++)
    fill_column(i);

  struct timeval end;
  gettimeofday(&end, NULL);

  long *passed_time = malloc(sizeof(long));
  *passed_time = subtract_times(start, end);

  pthread_exit(passed_time);
}


static void *thread_interleaved(void *argv) {
  struct timeval start;
  gettimeofday(&start, NULL);

  int k = (*(int*) argv);
  for(int x = k; x < input->width; x += number_of_threads)
    fill_column(x);

  struct timeval end;
  gettimeofday(&end, NULL);

  long *passed_time = malloc(sizeof(long));
  *passed_time = subtract_times(start, end);

  pthread_exit(passed_time);
}


long subtract_times(struct timeval t1, struct timeval t2) {
  return (t2.tv_sec - t1.tv_sec) * 1000000 + t2.tv_usec - t1.tv_usec;
}
