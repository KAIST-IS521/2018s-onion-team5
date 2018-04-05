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
#include "messanger.h"
#include "listen.h"
#include "message_wrapper.h"

#include <stdio.h>


void liste_loop(Messanger &msgr) {
  //std::thread thread_liste(liste_loop, std::ref(msgr));
  std::string github_id = msgr.get_name();
  std::map<std::string, std::string> &list = msgr.get_node_list();
  GPG &g = msgr.getGPG();

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
      if (!m.deserialize(msg)) {
        // illegal format
        m.clear();
        continue;
      }

      std::cout << "from: " << m.getFrom() <<  std::endl;
      std::cout << "to: " << m.getTo() <<  std::endl;
      std::cout << "type: " << m.getType() <<  std::endl;
      std::cout << "content: " << m.getContent() <<  std::endl;

      if (github_id.compare(m.getTo()) != 0) {
        // this is not my packet
        std::cout << "[!] this is not my packet" << std::endl;
        m.clear();
        continue;
      }

      if (m.getType() != 0) {
        // in this pharse it cannot be content or file
        std::cout << "[!] in this pharse it cannot be content or file" << std::endl;
        m.clear();
        continue;
      }

      std::string filename = m.getContent();
      m.clear();

      if (!g.decrypt_file(filename, filename)) {
        // in this pharse it cannot be content or file
        std::cout << "[!] decrypt error" << std::endl;
        m.clear();
        continue;
      }

      m.deserialize(filename);
      std::cout << "from2: " << m.getFrom() <<  std::endl;
      std::cout << "to2: " << m.getTo() <<  std::endl;
      std::cout << "type2: " << m.getType() <<  std::endl;
      std::cout << "content2: " << m.getContent() <<  std::endl;

      if (github_id.compare(m.getFrom()) == 0) {
        filename = m.serialize();

        std::cout << "[!] Relay to " << m.getTo() << std::endl;
        TCP_Client clnt(list[m.getTo()], NODE_PORT);
        clnt.connect();
        clnt.send_file(filename);
        clnt.close();

        std::cout << "[!] Delete " << filename << std::endl;
        delete_file(filename);
        continue;
      }

      if (github_id.compare(m.getTo()) == 0) {
        std::cout << "Oh! It's mine" << std::endl;
        std::cout << "===============================" << std::endl;
        std::cout << "from: " << m.getFrom() <<  std::endl;
        std::cout << "to: " << m.getTo() <<  std::endl;
        std::cout << "type: " << m.getType() <<  std::endl;
        std::cout << "content: " << m.getContent() <<  std::endl;
        std::cout << "===============================" << std::endl;
        m.clear();
        continue;
      }

      std::cout << "WTF" << std::endl;

    }
	}
  server.close();
  google::protobuf::ShutdownProtobufLibrary();
}
