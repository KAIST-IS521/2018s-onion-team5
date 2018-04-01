#pragma once

class TCP_Client {
  bool client;

  int client_sock;
  struct sockaddr_in client_sockaddr;
public:
  Client_sock();
  Client_sock(int sock, struct sockaddr_in sockaddr);
  ~Client_sock();

  bool connect(std::string host, int port);

  int send() {

  }

  int recv();


  void close();
}
