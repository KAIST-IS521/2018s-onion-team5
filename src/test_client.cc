#include "common/tcp_client.h"
#include <iostream>

int main(int argc, char *argv[]) {
  TCP_Client conn("127.0.0.1", 9099);

  if (!conn.connect()) {
    return -1;
  }
  std::cout << "Ok to connect" << std::endl;

  std::string buff;
  buff = "123123";
  conn.send(buff);
  std::cout << "client send: " << buff << std::endl;

  buff.clear();

  conn.recv(buff);
  std::cout << "server sent: " << buff << std::endl;

  conn.close();

  return 0;
}
