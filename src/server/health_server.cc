#include "health_server.h"

#include <time.h>
//#include <psocksxx/tcpnsockstream.h>
#include <string>
#include <cstdio>
#include <unistd.h>

#include "../common/tcp_client.h"
#include "../common/config.h"
#include "../common/node.pb.h"
#include "../common/sha1.hpp"
#include "../common/dumphex.h"
#include "../common/util.h"
#include "rest_client.h"

void health_server() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
  while (true) {
    Onion5::NodeList node_list;
    node_list.ParseFromString(Client_REST::get_nodes());
    for (int i = 0; i < node_list.nodes_size(); i++) {
      const Onion5::Node& node = node_list.nodes(i);

      std::string key = random_string(32);


      std::string github_id = node.github_id();
      std::string ip_addr = node.ip_addr();

      std::cout << "[HEALTH] Let's check '" << github_id << "' (" << ip_addr << ")" << std::endl;

      bool flag = false;

      TCP_Client sock(ip_addr.c_str(), NODE_PORT);

      sock.s_timeout(3, 0);
      if (sock.connect()) {
          flag = true;
      }

      if (flag) {
        flag = false;

        std::string data;
        data += PING_PREFIX;
        data += key;
        data += PING_POSTFIX;
        sock.send(data);

        std::string msg;

        SHA1 checksum1;
        checksum1.update(github_id); // CLIENT
        checksum1.update(key); // SERVER VALUE
        msg = checksum1.final();

        SHA1 checksum2;
        checksum2.update(key);
        checksum2.update(msg);
        msg = checksum2.final();
        std::cout << "Except : " << msg << std::endl;
        //DumpHex(msg);

        std::string buffer;
        if (sock.recv(buffer) > 0) { }
        //DumpHex(buffer);


        if (buffer.substr(0, 2).compare(PONG_PREFIX) == 0) {
          int len = buffer.size();
          //std::cout << len <<std::endl;
          if (buffer.substr(len - 2, 2).compare(PONG_POSTFIX) == 0) {
            buffer = buffer.substr(2, len - 4);
            //DumpHex(buffer);
            std::cout << "Recieve: " << msg << std::endl;
            if (msg.compare(buffer) == 0) {
                flag = true;
            }
          }
        }
      } // end if flag
      //std::cout << "RESULT:" <<  (int) flag << std::endl;
      if (flag == false) {
        std::cout << "[HEALTH] Check failed for '"<< github_id << "', EXTERMINATE!" << std::endl;
        Client_REST::delete_node(node.github_id());
      }
      sock.close();
    }
    sleep(3 * 60);
  }
  google::protobuf::ShutdownProtobufLibrary();
}
