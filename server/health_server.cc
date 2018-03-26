#include "health_server.h"

#include <time.h>
#include <psocksxx/tcpnsockstream.h>
#include <string>

#include "../common/config.h"
#include "../common/node.pb.h"
#include "../common/sha1.hpp"
#include "rest_client.h"

void health_server() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  while (true) {
    Onion5::NodeList node_list;
    node_list.ParseFromString(Client_REST::get_nodes());
    for (int i = 0; i < node_list.nodes_size(); i++) {
      const Onion5::Node& node = node_list.nodes(i);

      std::string key = random_string(32);
      std::string buffer;

      std::string github_id = node.github_id();
      std::string ip_addr = node.ip_addr();

      psocksxx::tcpnsockstream ss;
      ss.connect(ip_addr.c_str(), NODE_PORT);
      ss << PING_PREFIX << key << PING_POSTFIX;

      std::string msg;

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
      if (buffer.substr(0, 2).compare(PONG_PREFIX) == 0) {
        int len = msg.size();
        if (buffer.substr(len - 2, 2).compare(PONG_POSTFIX) == 0) {
          msg = msg.substr(2, len - 4);
          if (msg.compare(buffer) == 0) {
              flag = true;
          }
        }
      }
      if (flag == false) {
        Client_REST::delete_node(node.github_id());
      }
      //delete ss;
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
