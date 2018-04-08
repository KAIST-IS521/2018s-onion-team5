#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
//#include <psocksxx/tcpnsockstream.h>
#include <string>
#include <unistd.h>
#include <iostream>
#include <algorithm>

#include "../common/config.h"
#include "../common/http_client.h"
#include "../common/dumphex.h"
#include "../common/tcp_client.h"

#include "rest_client.h"
#include "node_manage.h"

void send_list(std::string& host) {
  std::string content;

  content = Client_REST::get_nodes();
  if (content.empty()) {
    std::cout << "MAYBE SERVER IS DEAD" << std::endl;
    return;
  }

  TCP_Client conn(host.c_str(), NODE_PORT);

  std::string buff;
  buff += LIST_PREFIX;
  buff += content;
  buff += LIST_POSTFIX;

  conn.connect();
  conn.send(buff);
  conn.close();
}

void boradcast_listener() {
  NodeManage m;
  int s, flag;
  struct sockaddr_in bcastAddr;

  s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == 0) {
    perror("sockek");
    exit(1);
  }
  puts("[BROADCAST] socket OK");

  flag = 1;
  if (setsockopt (s, SOL_SOCKET, SO_BROADCAST, &flag, sizeof flag) == -1) {
    perror("setsockopt");
    exit(1);
  }
  puts("[BROADCAST] setsockopt OK");

  memset(&bcastAddr, 0, sizeof(bcastAddr));
  bcastAddr.sin_family = AF_INET;
  bcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bcastAddr.sin_port = htons (9099);

  if (bind(s, (struct sockaddr *) &bcastAddr, sizeof(bcastAddr)) == -1) {
    perror("bind");
    exit(1);
  }
  puts("[BROADCAST] BIND OK");

  for (;;) {
    struct sockaddr_in clntAddr;
    char buf[BUFSIZ];
    int clntAddrLen = sizeof(clntAddr), bufLen;

    std::string github_id;
    std::string ip_addr;

    if ((bufLen = recvfrom(s, buf, BUFSIZ, 0, (struct sockaddr *)&clntAddr, (socklen_t *)&clntAddrLen)) == -1) {
			perror("recvfrom");
      exit(1);
    }

    buf[bufLen] = 0;
    std::string buff(buf, bufLen);
    if (buff.substr(0, 2).compare(BROADCAST_PREFIX) == 0) {
      //int size = buf[2];
      int size = buff.size();
      if (buff.substr(size - 2, 2).compare(BROADCAST_POSTFIX) == 0) {
        github_id = buff.substr(2, size - 4);
      }
    }

    if (!github_id.empty()) {
      //puts("recvfrom");
      std::cout << "[BROADCAST] " << github_id << " is here!" << std::endl;
      ip_addr = inet_ntoa(clntAddr.sin_addr);

      github_id.erase(std::find_if(github_id.rbegin(), github_id.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), github_id.end());

      Client_REST::add_node(github_id, ip_addr);
      send_list(ip_addr);
    }
  }
}
