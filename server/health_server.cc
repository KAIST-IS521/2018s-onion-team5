#include "health_server.h"

#include <time.h>
#include <psocksxx/tcpnsockstream.h>
#include <string>
#include <cstdio>

#include "../common/config.h"
#include "../common/node.pb.h"
#include "../common/sha1.hpp"
#include "../common/dumphex.h"
#include "rest_client.h"

void health_server() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  while (true) {
    Onion5::NodeList node_list;
    node_list.ParseFromString(Client_REST::get_nodes());
    printf("%d\n", node_list.nodes_size());
    for (int i = 0; i < node_list.nodes_size(); i++) {
      const Onion5::Node& node = node_list.nodes(i);

      std::string key = random_string(32);
      std::string buffer;

      std::string github_id = node.github_id();
      std::string ip_addr = node.ip_addr();

      bool flag = false;

      DumpHex(github_id);
      DumpHex(ip_addr);

      psocksxx::tcpnsockstream ss;
      try {
        ss.connect(ip_addr.c_str(), NODE_PORT);
        flag = true;
      } catch ( psocksxx::sockexception &e ) {
        flag = false;
      }

      if (flag) {
        flag = false;
        ss << PING_PREFIX << key << PING_POSTFIX <<std::endl;

        std::string msg;

        SHA1 checksum1;
        checksum1.update(github_id); // CLIENT
        checksum1.update(key); // SERVER VALUE
        msg = checksum1.final();

        SHA1 checksum2;
        checksum2.update(key);
        checksum2.update(msg);
        msg = checksum2.final();

        DumpHex(msg);

        ss >> buffer;
        DumpHex(buffer);


        if (buffer.substr(0, 2).compare(PONG_PREFIX) == 0) {
          int len = buffer.size();
          std::cout << len <<std::endl;
          if (buffer.substr(len - 2, 2).compare(PONG_POSTFIX) == 0) {
            buffer = buffer.substr(2, len - 4);
            DumpHex(buffer);
            if (msg.compare(buffer) == 0) {
                flag = true;
            }
          }
        }
      } // end if flag
      std::cout << "RESULT:" <<  (int) flag << std::endl;
      if (flag == false) {
        Client_REST::delete_node(node.github_id());
      }
    }
    sleep(3 * 60);
  }
  google::protobuf::ShutdownProtobufLibrary();
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
