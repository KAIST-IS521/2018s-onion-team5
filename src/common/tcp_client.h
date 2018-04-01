#pragma once

#include <string>
#include <netinet/in.h>
#include <sys/time.h>


class TCP_Client {
  bool client;

  std::string host;
  int port;

  int client_sock;
  struct sockaddr_in client_sockaddr;

  int write(void *ptr, int len);
  int read(void *ptr, int len);

public:
  TCP_Client(std::string host, int port);
  TCP_Client(int sock, struct sockaddr_in sockaddr);
  ~TCP_Client();

  bool connect();
  int send(std::string data);
  int recv(std::string &data);
  void close();

};
