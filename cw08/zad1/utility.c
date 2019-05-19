#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include "utility.h"

void error_exit(char *error_message, int status) {
  fprintf(stderr, "%s\nErrno: %s\n", error_message, strerror(errno));
  exit(status);
}


image_t *create_image(int width, int height) {
  image_t *img = malloc(sizeof(image_t));
  img->width = width;
  img->height = height;
  img->buffer = malloc(img->height * sizeof(int*));

  for(int i = 0; i < img->height; i++)
    img->buffer[i] = malloc(img->width * sizeof(int));

  return img;
}


image_t *open_image(char *path) {
  FILE *file = fopen(path, "r");
  if(file == NULL)
    error_exit(path, -1);

  int width;
  int height;

  fscanf(file, "P2 %i %i 255", &width, &height);
  image_t *img = create_image(width, height);

  for(int i = 0; i < img->height; i++)
    for(int j = 0; j < img->width; j++)
      fscanf(file, "%i", &img->buffer[i][j]);

  fclose(file);
  return img;
}


void save_image(image_t *img, char *name) {
  FILE *file = fopen(name, "w");
  if(file == NULL)
    error_exit("Error opening image while saving", -1);
  fprintf(file, "P2\n%i %i\n255\n", img->width, img->height);

  for(int i = 0; i < img->height; i++) {
    for(int j = 0; j < img->width; j++) {
      fprintf(file, "%i", img->buffer[i][j]);
      if(j+1 != img->width) fputc(' ', file);
    }
    fputc('\n', file);
  }
  fclose(file);
}


filter_t *open_filter(char *path) {
  FILE *file = fopen(path, "r");
  if(file == NULL)
    error_exit("Error opening filter", -1);

  int size;
  fscanf(file, "%i", &size);
  filter_t *filter = malloc(sizeof(filter_t));
  filter->size = size;
  filter->buffer = malloc(filter->size * sizeof(double*));

  for(int i = 0; i < filter->size; i++) {
    filter->buffer[i] = malloc(filter->size * sizeof(double));
    for(int j = 0; j < filter->size; j++)
      fscanf(file, "%lf", &filter->buffer[i][j]);
  }
  fclose(file);
  
  return filter;
}
