#include <string>
#include <map>
#include <psocksxx/tcpnsockstream.h>
#include <iostream>

#include "../common/config.h"
#include "../common/node.pb.h"
#include "../common/dumphex.h"
#include "../common/sha1.hpp"
#include "listen.h"


void listener(std::string github_id, std::map<std::string, std::string> &list) {
  int sock;
	struct sockaddr_in servAddr;
  int servPort = 9099;

	// socket
	sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(servPort);

	// BINDING
	bind(sock, (struct sockaddr *) &servAddr, sizeof(servAddr));

	// LISTEN
	listen(sock, 100);

  std::cout << "LISTEN 9099" << std::endl;
  GOOGLE_PROTOBUF_VERIFY_VERSION;
	for (;;) {
    int clnt;
		struct sockaddr_in clntAddr;
		int len = sizeof(clntAddr);
		char client_ip[32];
		int clinet_port;

    clnt = accept(sock, (struct sockaddr * ) &clntAddr, (socklen_t *)&len);
    std::cout << "ACCEPT" << std::endl;

    //std::string buf;
    //buf.resize(4096);
    char buf[4096 + 1];
    memset(buf, 0, 4096 + 1);
    int bufLen = 0;
		//int size = (* css).tellg();

    puts("asdf");
    if ((bufLen = recv(sock, buf, 4096, 0)) < 0) {
      // error handling
    }
    puts("asdf");

    DumpHex(buf, bufLen);

    std::string msg;
    msg.assign(buf, bufLen);
    DumpHex(msg);
    puts("asdf");


    std::cout << msg.size() << std::endl;
    std::cout << "GET" << std::endl;

    // node list
    if (msg.substr(0, 2).compare(LIST_PREFIX) == 0) {
      int len = msg.size();
      if (msg.substr(len - 2, 2).compare(LIST_PREFIX) == 0) {
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

        int bufLen = msg.size();

        //(*css) << msg;
        if (send(sock, &msg[0], bufLen, 0) != bufLen)
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
  google::protobuf::ShutdownProtobufLibrary();
}
