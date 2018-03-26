#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <map>
#include <string>


#include "fetch_key.h"
#include "listen.h"

#define TEMPFILE "/tmp/fofofofoffff"

int main(int argc, char *argv[]) {
  std::map<std::string, std::string> node_list;
  listener("AhnMo", node_list);

  /*
  std::string passphrase;

  std::string name("TestTest");

  //char passphrase[PASS_MAX];
  passphrase = getpass("Put your passphrase: ");

  pid_t pid;
  int status = 0;

  pid = fork();
  if (pid < 0) { }
  else if (pid == 0) {
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    if (fd > 2) close(fd);

    system("touch " TEMPFILE);
    execlp(
      "/usr/bin/gpg",
      "gpg",
        "--batch",
        "--no-use-agent",
        "-o" "/dev/null",
        "--local-user", name.c_str(),
        "--passphrase", passphrase.c_str(),
        "-as",
        TEMPFILE,
        0);
    // not reach
    puts("error");
    exit(1);
  }
  waitpid(pid, &status, NULL);
  system("rm " TEMPFILE);

  printf("status %d\n", status);

  return 0;
  std::string ret;
  if (fetch_key_from_github("AhnMo", ret) == 0) {
    std::cout << ret << std::endl;
  } else {
    std::cout << "[!] Error to fetch key" << std::endl;
  }
  */




  return 0;
}
