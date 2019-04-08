#include <fcntl.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#define MAX 512

int	main(int argc, char* argv[]) {
	srand(time(NULL));
	if (argc != 3)
		return -1;

	char date_res[MAX];
	char pipe_string[MAX+20];

	int count = atoi(argv[2]);

	printf("%d\n", (int) getpid());

	int pipe = open(argv[1], O_WRONLY);
	if	(pipe < 0)
    return -1;
  
	for (int i = 0; i < count; i++) {
		FILE *date = popen("date", "r");
		fgets(date_res, MAX, date);

    if (sprintf(pipe_string, "%d @ %s", (int) getpid(), date_res) < 0)
      return -1;
  
    if (write(pipe, pipe_string, strlen(pipe_string)) < 0)
      return -1;
  
    sleep(rand() % 4 + 2);
	}

	close(pipe);
	return 0;
}
