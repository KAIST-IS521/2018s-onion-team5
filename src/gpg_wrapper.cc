#include "gpg_wrapper.h"
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <cstdio>
#include "../common/util.h"

#define CLOSE_STDIO {\
  int fd = open("/dev/null", O_RDWR);\
  dup2(fd, STDIN_FILENO);\
  dup2(fd, STDOUT_FILENO);\
  dup2(fd, STDERR_FILENO);\
  if (fd > 2) close(fd);\
}

#define FORK_AND_WAIT(TASK) {\
  pid_t pid = fork();\
  if (pid < 0) { perror("fork"); exit(1); }\
  else if (pid == 0) {\
    { TASK }; \
    exit(1);\
  }\
  waitpid(pid, &status, NULL);\
}

#define GPG_RET_CHECK(CALLBACK) {\
  if (status == 0) {\
    { CALLBACK; }\
    return true;\
  } else if (status == 512) {\
    return false;\
  } else {\
    puts("UNKNOWN STATUS CODE, EXIT");\
    exit(1);\
  }\
}

void GPG::set_dummy_cred() {
  this->name = "1234";
  this->passphrase = "1234";
}

bool GPG::is_authorized() {
  return !(this->name.empty() || this->passphrase.empty());
}

bool GPG::verify_passphrase(std::string name, std::string passphrase) {
  int status = 0;

  std::string tempfile = save_tempfile("");
  std::string cmd;

  FORK_AND_WAIT({
    CLOSE_STDIO;
    cmd = "touch " + tempfile;
    system(cmd.c_str());
    execlp(
      "/usr/bin/gpg",
      "gpg",
        "--batch",
        "--no-use-agent",
        "-o" "/dev/null",
        "--local-user", name.c_str(),
        "--passphrase", passphrase.c_str(),
        "-as",
        tempfile.c_str(),
        0);
    perror("execlp");
  });
  cmd = "rm " + tempfile;
  system(cmd.c_str());

  printf("status %d\n", status);

  GPG_RET_CHECK({
    this->name = name;
    this->passphrase = passphrase;
  });
}

bool GPG::add_public_key(std::string locate) {
  if (!this->is_authorized()) {
    return false;
  }

  int status = 0;
  FORK_AND_WAIT({
    CLOSE_STDIO;
    execlp(
      "/usr/bin/gpg",
      "gpg",
        "--batch",
        "--no-use-agent",
        "--import", locate.c_str(),
        0);
    perror("execlp");
  });
  printf("status %d\n", status);

  GPG_RET_CHECK({});
}
