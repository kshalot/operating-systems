typedef struct image_t {
  int width;
  int height;
  int **buffer;
} image_t;


typedef struct filter_t {
  int size;
  double **buffer;
} filter_t;


void error_exit(char*, int);
image_t *create_image(int, int);
image_t *open_image(char*);
void save_image(image_t*, char*);
filter_t *open_filter(char*);
