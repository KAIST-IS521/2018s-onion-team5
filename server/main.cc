#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include "node.pb.h"
#include <google/protobuf/util/time_util.h>
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <psocksxx/tcpnsockstream.h>

void call_from_thread() {
  std::cout << "Hello, World" << std::endl;
}

class NodeManage {
    Onion5::NodeList node_list;
  public:
    NodeManage();
    ~NodeManage();
    void add_user(std::string github_id, std::string host);
    void get_binary();
    void saveToFile(std::string filename);
};

NodeManage::NodeManage() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
}

void NodeManage::saveToFile(std::string filename) {
  std::fstream output(filename, std::ios::out | std::ios::trunc | std::ios::binary);
  if (!this->node_list.SerializeToOstream(&output)) {
    std::cerr << "Failed to write." << std::endl;
  }
}

NodeManage::~NodeManage() {
  google::protobuf::ShutdownProtobufLibrary();
}

void NodeManage::add_user(std::string github_id, std::string host) {
  Onion5::Node* node = this->node_list.add_nodes();
  (*node->mutable_github_id()) = github_id;
  (*node->mutable_ip_addr()) = host;
  (*node->mutable_last_check()) = google::protobuf::util::TimeUtil::TimeTToTimestamp(time(NULL));
}

void NodeManage::get_binary() {

}

void send_list(std::string& host) {
  psocksxx::tcpnsockstream ss;

  ss.connect(host.c_str(), 6001);
  ss << "hello" << std::endl;
}

void boradcast_listener(NodeManage m) {
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

    puts(github_id.c_str());
    puts(ip_addr.c_str());

    m.add_user(github_id, ip_addr);
    send_list(ip_addr);
  }
}

int main() {
  NodeManage m;

  pid_t pid;
  int status;

  pid = fork();
  if (pid == 0)
    boradcast_listener(m);
  else if (pid == -1) {
    perror("fork");
    return 1;
  }
  //waitpid(pid, &status, 0);
  getchar();
  m.saveToFile("test.bin");
exit(1);
/*
  std::thread t1(call_from_thread);
  t1.join();

  Onion5::Node* node = node_list.add_nodes();
  (*node->mutable_github_id()) = "AhnMo";
  (*node->mutable_ip_addr()) = "172.17.0.2";
  (*node->mutable_last_check()) = google::protobuf::util::TimeUtil::TimeTToTimestamp(time(NULL));

  node = node_list.add_nodes();
  (*node->mutable_github_id()) = "Leeswimmig";
  (*node->mutable_ip_addr()) = "172.17.0.3";
  (*node->mutable_last_check()) = google::protobuf::util::TimeUtil::TimeTToTimestamp(time(NULL));

  node = node_list.add_nodes();
  (*node->mutable_github_id()) = "S-KYUCHAN";
  (*node->mutable_ip_addr()) = "172.17.0.4";
  (*node->mutable_last_check()) = google::protobuf::util::TimeUtil::TimeTToTimestamp(time(NULL));

  node = node_list.add_nodes();
  (*node->mutable_github_id()) = "HOJUNYANG";
  (*node->mutable_ip_addr()) = "172.17.0.5";
  (*node->mutable_last_check()) = google::protobuf::util::TimeUtil::TimeTToTimestamp(time(NULL));

  //(node_list).DeleteSubrange(1, 1);
  (node_list).mutable_nodes()->DeleteSubrange(1, 1);


  std::fstream output("test.bin", std::ios::out | std::ios::trunc | std::ios::binary);
    if (!node_list.SerializeToOstream(&output)) {
      std::cerr << "Failed to write address book." << std::endl;
      return -1;
    }

  google::protobuf::ShutdownProtobufLibrary();
*/
  return 0;
}
