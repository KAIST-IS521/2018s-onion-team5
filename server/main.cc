#include <sys/wait.h>
#include <unistd.h>

#include "node_rest.h"
#include "broadcast.h"

#define FORK_AND_RUN(function) {\
  pid = fork();\
  if (pid == 0)\
    function();\
  else if (pid == -1) {\
    perror("fork");\
    return 1;\
  }\
}

int main() {
  pid_t pid;
  int status;

  FORK_AND_RUN(node_rest_server);
  FORK_AND_RUN(boradcast_listener);

  wait(&status);
  return 0;
}
