#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>

// socket wrapper
#include <psocksxx/tcpnsockstream.h>

#include <string>

#include <unistd.h>

#include "node_manage.h"
#include "http_client.h"
#include "config.h"
#include "dumphex.h"
#include "rest_client.h"

void send_list(std::string& host) {
  // fetch node list from server
  std::string content;

  content = Client_REST::get_nodes();
  if (content.empty()) {
    // rest server is dead
    std::cout << "SERVER IS DEAD" << std::endl;
    return;
  }

  psocksxx::tcpnsockstream ss;
  ss.connect(host.c_str(), NODE_PORT);
  ss << content;

}

void boradcast_listener() {
  NodeManage m;

  int s, flag;
  struct sockaddr_in bcastAddr;

  s = socket (PF_INET, SOCK_DGRAM, IPPROTO_UDP);
  if (s == NULL) {
    perror("socker");
    exit(1);
  }

  flag = 1;
  if (setsockopt (s, SOL_SOCKET, SO_BROADCAST, &flag, sizeof flag) == -1) {
    perror("setsockopt");
    exit(1);
  }

  memset(&bcastAddr, 0, sizeof(bcastAddr));
  bcastAddr.sin_family = AF_INET;
  bcastAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  bcastAddr.sin_port = htons (9099);

  if (bind(s, (struct sockaddr *) &bcastAddr, sizeof(bcastAddr)) == -1) {
    perror("bind");
    exit(1);
  }

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

    github_id = buf;
    ip_addr = inet_ntoa(clntAddr.sin_addr);

    //github_id = github_id.substr(0, github_id.length() - 1);

    //
    github_id.erase(std::find_if(github_id.rbegin(), github_id.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), github_id.end());

    Client_REST::add_node(github_id, ip_addr);
    send_list(ip_addr);

    std::string bu = m.get_binary();
    DumpHex(bu.c_str(), bu.size());
  }
}
