#pragma once

#include <string>
//#include <sys/socket.h>
#include <netinet/in.h>

#include "tcp_client.h"

class TCP_Server {
  std::string host;
  int port;

  int sock;
  struct sockaddr_in server_sockaddr;

public:
  TCP_Server(std::string host, int port);
  ~TCP_Server();
  bool bind();
  bool listen();
  bool listen(int backlog);
  TCP_Client * accept();
  void close();


  bool timeout(time_t tv_sec, tv_usec tv_usec);
  bool timeout(struct timeval timeout);
};
