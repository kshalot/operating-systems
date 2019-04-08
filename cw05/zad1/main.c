#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_ARGS 50
#define MAX_COMMANDS 50

typedef struct {
  char *name;
  int argc;
  char **argv;
} command;


int read_file(char *filename, char **buffer) {
  FILE *file = fopen(filename, "r");
  if(file == NULL)
    return -1;

  fseek(file, 0, SEEK_END);
  int size = ftell(file);
  fseek(file, 0, SEEK_SET);

  *buffer = malloc((size + 1) * sizeof(char));

  return fread(*buffer, 1, size, file);
}

command parse_line(char *line) {
  char *saveptr;
  char **args = malloc((MAX_ARGS + 1) * sizeof(char));
  char *arg = strtok_r(line, " ", &saveptr);
  command cmd = {};

  int i;
  for(i = 0; arg != NULL && i < MAX_ARGS; i++, arg = strtok_r(NULL, " ", &saveptr)) {
    if (i == 0)
      cmd.name = arg;
    args[i] = arg;  
  }
  cmd.argv = args;
  
  return cmd;
}

command  *parse_file(char *file_content, int *size) {
  char *saveptr;
  command *cmds = malloc((MAX_COMMANDS + 1) * sizeof(command));
  char *cmd = strtok_r(file_content, "|\n", &saveptr);

  int i;
  for(i = 0; cmd != NULL && i < MAX_COMMANDS; i++, cmd = strtok_r(NULL, "|\n", &saveptr))
    cmds[i] = parse_line(cmd);

  *size = i;
  return cmds;
}

int execute(command *cmds, int size) {

  int pipes[2][2];

  int i;
  for(i = 0; i < size; i++) {
    command cmd = cmds[i];
    printf("mam command: %s\n", cmd.name);

    if(i != 0) {
      close(pipes[i%2][0]);
      close(pipes[i%2][1]);
    }
    pipe(pipes[i%2]);
    pid_t pid = fork();
    if(pid == 0) {
      if(i != size - 1) {
        close(pipes[i%2][0]);
        dup2(pipes[i%2][1], STDOUT_FILENO);
      }
      if(i != 0) {
        close(pipes[(i+1)%2][1]);
        dup2(pipes[(i+1)%2][0], STDIN_FILENO);
      }
      execvp(cmd.name, cmd.argv);
    }
  }
  close(pipes[i%2][0]);
  close(pipes[i%2][1]);

  return 0;

}

int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "Invalid arguments. Usage: ./main <path>\n");
    return -1;
  }

  char *file_content;
  read_file(argv[1], &file_content);
  int size;
  command *cmds = parse_file(file_content, &size);

  execute(cmds, size);

  free(file_content);
  
  return 0;
}
