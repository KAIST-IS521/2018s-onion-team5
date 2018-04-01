#include "common/tcp_server.h"
#include <iostream>

int main(int argc, char *argv[]) {
  TCP_Server server("0.0.0.0", 9099);

  if (!server.bind()) {
    return -1;
  }
  std::cout << "Ok to bind" << std::endl;

  if (!server.listen()) {
    return -1;
  }
  std::cout << "Ok to listen" << std::endl;

  while (true) {
    TCP_Client * client = server.accept();

    std::string buff;
    int size = client->recv(buff);
    std::cout << "client sent: " << buff << std::endl;

    client->send(buff);
    std::cout << "server send: " << buff << std::endl;

    delete client;
  }

  return 0;
}
