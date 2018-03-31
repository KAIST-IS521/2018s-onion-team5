#include <sys/wait.h>
#include <unistd.h>
#include <string>
#include <cstdio>

#include "rest_server.h"
#include "broadcast_server.h"
#include "health_server.h"

#define FORK_AND_RUN(function) {\
  pid = fork();\
  if (pid == 0)\
    function();\
  else if (pid == -1) {\
    perror("fork");\
    return 1;\
  }\
}

int main(int argc, char *argv[]) {
  pid_t pid;
  int status;

  bool rest      = false;
  bool broadcast = false;
  bool health    = false;

  if (argc < 2) {
    rest = true;
    broadcast = true;
    health = true;
  } else {
    std::string target;

    target = "REST";
    rest = target.compare(argv[1]) == 0;

    target = "BROADCAST";
    broadcast = target.compare(argv[1]) == 0;

    target = "HEALTH";
    health = target.compare(argv[1]) == 0;
  }

  printf("%d %d %d\n", rest, broadcast, health);

  if (rest)
  FORK_AND_RUN(rest_server);

  if (broadcast)
  FORK_AND_RUN(boradcast_listener);

  if (health)
  FORK_AND_RUN(health_server);

  wait(&status);
  return 0;
}
