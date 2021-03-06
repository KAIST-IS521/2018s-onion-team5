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
  std::string github_id = msgr.get_name();
  std::map<std::string, std::string> &list = msgr.get_node_list();
  GPG &g = msgr.getGPG();

  TCP_Server server("0.0.0.0", 9099);

  if (!server.bind()) {
    return;
  }

  if (!server.listen()) {
    return;
  }

  GOOGLE_PROTOBUF_VERIFY_VERSION;
  TCP_Client *client = NULL;
	for (;;) {
    if (client) {
      client->close();
      client = NULL;
    }
    client = server.accept();

    std::string msg;
    int size = client->recv(msg);

    if (msg.substr(0, 2).compare(LIST_PREFIX) == 0) {
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

        if (client->send(msg) != msg.size()) {
          continue;
        }
      }
    } else if (is_existed_file(msg)) {
      Message m;
      if (!m.deserialize(msg)) {
        // illegal format
        m.clear();
        continue;
      }

      if (github_id.compare(m.getTo()) != 0) {
        // this is not my packet
        m.clear();
        continue;
      }

      if (m.getType() != 0) {
        // in this pharse it cannot be content or file
        m.clear();
        continue;
      }

      std::string filename = m.getContent();
      m.clear();

      if (!g.decrypt_file(filename, filename)) {
        // in this pharse it cannot be content or file
        m.clear();
        continue;
      }

      m.deserialize(filename);

      if (github_id.compare(m.getFrom()) == 0) {
        filename = m.serialize();

        TCP_Client clnt(list[m.getTo()], NODE_PORT);
        if (clnt.connect()) {
          clnt.send_file(filename);
          clnt.close();
        }

        delete_file(filename);
        continue;
      }

      if (github_id.compare(m.getTo()) == 0) {
        if (m.getType() == 1) {
          msgr.push_message(m.getFrom(), m.getFrom() + ": " + m.getContent());
        } else if (m.getType() == 2) {
          msgr.push_message(m.getFrom(), m.getFrom() + " send '" + m.getContent() + "'");
        }
        m.clear();
        continue;
      }
    }
	}
  server.close();
  google::protobuf::ShutdownProtobufLibrary();
}
