#include <string>
#include <map>
#include <psocksxx/tcpnsockstream.h>
#include <iostream>

#include "../common/config.h"
#include "../common/node.pb.h"
#include "../common/dumphex.h"
#include "../common/sha1.hpp"
#include "../common/tcp_server.h"
#include "../common/tcp_client.h"
#include "listen.h"

#include <stdio.h>


void listener(std::string github_id, std::map<std::string, std::string> &list) {
  TCP_Server server("0.0.0.0", 9099);

  if (!server.bind()) {
    return;
  }
  std::cout << "Ok to bind" << std::endl;

  if (!server.listen()) {
    return;
  }
  std::cout << "Ok to listen" << std::endl;

  GOOGLE_PROTOBUF_VERIFY_VERSION;
	for (;;) {
    TCP_Client * client = server.accept();

    std::string msg;
    int size = client->recv(msg);
    puts("===============================================================");
    DumpHex(msg);
    puts("===============================================================");

    // node list
    if (msg.substr(0, 2).compare(LIST_PREFIX) == 0) {
      int len = msg.size();
      if (msg.substr(len - 2, 2).compare(LIST_POSTFIX) == 0) {
        Onion5::NodeList node_list;
        msg = msg.substr(2, len - 4);
        node_list.ParseFromString(msg);
        list.clear();
        for (int i = 0; i < node_list.nodes_size(); ++i) {
          const Onion5::Node& node = node_list.nodes(i);
          std::cout << node.github_id() << std::endl;
          std::cout << node.ip_addr() << std::endl;
          list.insert (std::pair<std::string, std::string>(node.github_id(), node.ip_addr()));
        }
      }
    }
    // health check
    else if (msg.substr(0, 2).compare(PING_PREFIX) == 0) {

      int len = msg.size();
      if (msg.substr(len - 2, 2).compare(PING_POSTFIX) == 0) {
        std::string key = msg.substr(2, len - 4);
        std::string msg;

        SHA1 checksum1;
        checksum1.update(github_id);
        checksum1.update(key);
        msg = checksum1.final();

        SHA1 checksum2;
        checksum2.update(key);
        checksum2.update(msg);
        msg = checksum2.final();

        msg = PONG_PREFIX + msg + PONG_POSTFIX;
        DumpHex(msg);

        if (client->send(msg) != msg.size())
        {
          // error handling
        }
      }
    }
    // encrypted packet
    else if (false) {
      // encrypted packet

    }

		//delete css;

	}
  server.close();
  google::protobuf::ShutdownProtobufLibrary();
}
