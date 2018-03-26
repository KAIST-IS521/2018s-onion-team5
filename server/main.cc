#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <psocksxx/tcpnsockstream.h>
#include <string>

#include "rest_server.h"
#include "broadcast_server.h"
#include "rest_client.h"


#include "../common/sha1.hpp"

#define FORK_AND_RUN(function) {\
  pid = fork();\
  if (pid == 0)\
    function();\
  else if (pid == -1) {\
    perror("fork");\
    return 1;\
  }\
}

std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

int main() {
  pid_t pid;
  int status;

  FORK_AND_RUN(rest_server);
  FORK_AND_RUN(boradcast_listener);

/*
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  while (true) {
    Onion5::NodeList node_list;
    node_list.ParseFromString(Client_REST::get_nodes());
    for (int i = 0; i < node_list.node_size(); i++) {
      const Onion5::Node& node = node_list.node(i);

      std::string key = random_string(32);
      std::string buffer;

      std::string github_id = node.github_id();
      std::string ip_addr = node.ip_addr();

      psocksxx::tcpnsockstream ss;
      ss.connect(ip_addr.c_str(), NODE_PORT);
      ss << "\xFE\xED" << random_str << "\xFA\xCE";

      string msg;

      SHA1 checksum1;
      checksum1.update(github_id); // CLIENT
      checksum1.update(key); // SERVER VALUE
      msg = checksum1.final();

      SHA1 checksum2;
      checksum2.update(key);
      checksum2.update(msg);
      msg = checksum2.final();

      ss >> buffer;

      bool flag = false;
      if (buffer[0] == '\xFE' && buffer[1] == '\xED') {
        int len = msg.size();
        if (msg[len - 2] == '\xF0' && msg[len - 1] == '\x0D') {
          msg = msg.substr(2, len - 4);
          if (msg.compare(buffer) == 0) {
              flag = true;
          }
        }
      }
      if (flag == false) {
        Client_REST::delete_user(node.github_id());
      }
      delete ss;
    }
    sleep(3 * 60);
  }
  google::protobuf::ShutdownProtobufLibrary();
*/
  wait(&status);
  return 0;
}