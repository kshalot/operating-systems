#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv) {
  if(argc < 2) {
    fprintf(stderr, "Not enough arguments. Usage: \"./main <N>\"");
    return 1;
  }
  srand(time(NULL));
  int N = atoi(argv[1]);

  int i;
  for(i = 0; i < N; i++) {
    // Buffers in order to pass
    // char* arguments to exec
    char weight_buffer[10];
    char number_buffer[10];
    int cargo_weight = rand() % 32 + 1;
    int cargo_number = rand() % 64 + 1;

    if(fork() == 0) {
      sprintf(weight_buffer, "%d", cargo_weight);
      sprintf(number_buffer, "%d", cargo_number);
      execlp("./loader", "loader", weight_buffer, number_buffer, NULL);
    }
  }

  for(i = 0; i < N; i++) {
    wait(NULL);

  }
  return 0;
}
