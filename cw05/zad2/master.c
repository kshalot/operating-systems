#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
 
#define MAX 512


int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "Too few arguments!\n");
    return -1;
  }

  char line[MAX];
  if (mkfifo(argv[1], S_IWUSR | S_IRUSR) < 0) {
    fprintf(stderr, "error fifo\n");
    return -1;
  }

	FILE *pipe = fopen(argv[1], "r");
	if (!pipe) {
    fprintf(stderr, "error pipe\n");
    return -1;
  }

	while (fgets(line, MAX, pipe) != NULL)
    fputs(line, stdout);

	fclose(pipe);
	return 0;
}

