#include "tcp_server.h"
#include "tcp_client.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

TCP_Server::TCP_Server(std::string host, int port) {
  this->host = host;
  this->port = port;

  this->sock = ::socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

  memset(&this->server_sockaddr, 0, sizeof(struct sockaddr_in));
  this->server_sockaddr.sin_family = AF_INET;
	this->server_sockaddr.sin_addr.s_addr = ::inet_addr(this->host.c_str());
	this->server_sockaddr.sin_port = ::htons(this->port);
}

TCP_Server::~TCP_Server() {
  this->close();
}

bool TCP_Server::bind() {
  if (::bind(
    this->sock,
    (struct sockaddr *) &this->server_sockaddr,
    sizeof(struct sockaddr_in)) == -1) {
    perror("bind");
    return false;
  }
  return true;
}

bool TCP_Server::listen(int backlog) {
  if (::listen(sock, backlog) == -1) {
    perror("bind");
    return false;
  }
  return true;
}

bool TCP_Server::listen() {
  return this->listen(10);
}

TCP_Client * TCP_Server::accept() {
  int client_fd;
  struct sockaddr_in client_sockaddr;
  int len = sizeof(struct sockaddr_in);
  client_fd = ::accept(this->sock, (struct sockaddr *)&client_sockaddr, (socklen_t *)&len);
  return new TCP_Client(client_fd, client_sockaddr);
}

void TCP_Server::close() {
  this->host.clear();
  this->port = 0;
  ::close(this->sock);
}
