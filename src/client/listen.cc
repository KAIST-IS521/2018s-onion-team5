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
#include "../common/util.h"
#include "messenger.h"
#include "listen.h"
#include "message_wrapper.h"

#include <stdio.h>


void liste_loop(Messenger &msgr) {
  //std::thread thread_liste(liste_loop, std::ref(msgr));
  std::string github_id = msgr.get_name();
  std::map<std::string, std::string> &list = msgr.get_node_list();

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
    //puts("===============================================================");
    //DumpHex(msg);
    //puts("===============================================================");

    // node list

    /*
    if (msg.substr(0, 2).compare("\x2F\x74") == 0) {
      DumpHex(msg);
      std::string cmdasdfasd = "xxd "+ msg;
      system(cmdasdfasd.c_str());
    }
    */
    if (msg.substr(0, 2).compare(LIST_PREFIX) == 0) {
      std::cout << "LIST" << std::endl;
      int len = msg.size();
      if (msg.substr(len - 2, 2).compare(LIST_POSTFIX) == 0) {
        Onion5::NodeList node_list;
        msg = msg.substr(2, len - 4);
        node_list.ParseFromString(msg);
        list.clear();
        for (int i = 0; i < node_list.nodes_size(); ++i) {
          const Onion5::Node& node = node_list.nodes(i);
          list.insert (std::pair<std::string, std::string>(node.github_id(), node.ip_addr()));
        }
      }
    }
    // health check
    else if (msg.substr(0, 2).compare(PING_PREFIX) == 0) {
std::cout << "health" << std::endl;
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
    } else if (is_existed_file(msg)) {
      Message m;
      if (m.deserialize(msg)) {
        GPG &g = msgr.getGPG();
        std::string inin_file;
        std::string outo_file;

        inin_file = m.getContent();
        std::cout << "==============================" << std::endl;
        std::cout << inin_file << std::endl;
        std::cout << m.getType() << std::endl;
        std::cout << "==============================" << std::endl;

        if (g.decrypt_file(inin_file, outo_file)) {
          m.clear();
          m.deserialize(outo_file);

          std::cout << "From: " << m.getFrom() << std::endl;
          std::cout << "to: " << m.getTo() << std::endl;
          std::cout << "Type: " << m.getType() << std::endl;
          std::cout << "content: " << m.getContent() << std::endl;
          std::string to = m.getTo();
          std::string from = m.getFrom();
          std::string content = m.getContent();
          if (github_id.compare(to) == 0 && m.getType() != 0) {
            std::cout << from << ":" << content << std::endl;
          } else {
            if (github_id.compare(from) == 0 && m.getType() == 0) {
              TCP_Client clnt(list[to], NODE_PORT);
              clnt.connect();
              clnt.send_file(content);
              clnt.close();
            } else {
              // illegal packet
            }
          }
        }
      }
    }


	}
  server.close();
  google::protobuf::ShutdownProtobufLibrary();
}
