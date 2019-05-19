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

int execute(command *cmds, int size) {

  int pipes[2][2];

  int i;
  for(i = 0; i < size; i++) {
    command cmd = cmds[i];

    if(i != 0) {
      close(pipes[i%2][0]);
      close(pipes[i%2][1]);
    }
    if(pipe(pipes[i%2]) == -1)
      return -1;
    pid_t pid = fork();
    if(pid == 0) {
      if(i != size - 1) {
        close(pipes[i%2][0]);
        if(dup2(pipes[i%2][1], STDOUT_FILENO) < 0)
          return -1;
      }
      if(i != 0) {
        close(pipes[(i+1)%2][1]);
        if(dup2(pipes[(i+1)%2][0], STDIN_FILENO) < 0)
          return -1;
      }
      execvp(cmd.name, cmd.argv);
    }
  }
  close(pipes[0][0]);
  close(pipes[0][1]);
  close(pipes[1][0]);
  close(pipes[1][1]);

  for(i = 0; i < size; i++) {
    int status;
    wait(&status);
  }

  return 0;
}

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

command parse_command(char *command_string) {
  char *saveptr;
  char **args = malloc((MAX_ARGS + 1) * sizeof(char));
  char *arg = strtok_r(command_string, " ", &saveptr);
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

command *parse_line(char *line, int *size) {
  char *saveptr;
  command *cmds = malloc(MAX_COMMANDS * sizeof(command));
  char *command_string = strtok_r(line, "|", &saveptr);
  
  int i;
  for(i = 0; command_string != NULL && i < MAX_COMMANDS; i++, command_string = strtok_r(NULL, "|", &saveptr))
    cmds[i] = parse_command(command_string);
  
  *size = i;
  return cmds;
}

int parse_file(char *file_content) {
  char *saveptr;
  command *cmds = malloc((MAX_COMMANDS + 1) * sizeof(command));
  char *cmd = strtok_r(file_content, "\n", &saveptr);

  int i;
  for(i = 0; cmd != NULL && i < MAX_COMMANDS; i++, cmd = strtok_r(NULL, "\n", &saveptr)) {
    int size;
    cmds = parse_line(cmd, &size);
    execute(cmds, size);
  }

  return 0;
}


int main(int argc, char **argv) {
  if(argc != 2) {
    fprintf(stderr, "Invalid arguments. Usage: ./main <path>\n");
    return -1;
  }

  char *file_content;
  read_file(argv[1], &file_content);
  parse_file(file_content);

  free(file_content);
  
  return 0;
}
