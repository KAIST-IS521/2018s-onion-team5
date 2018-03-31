
#include "../common/node.pb.h"
//#include <google/protobuf/util/time_util.h>
#include "node_manage.h"
#include <fstream>
#include <sstream>
#include <iostream>

NodeManage::NodeManage() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
}

NodeManage::~NodeManage() {
  google::protobuf::ShutdownProtobufLibrary();
}

void NodeManage::add_user(std::string github_id, std::string host) {
  std::cout << "github_id " << github_id << std::endl;
  std::cout << "host " << host << std::endl;
  if (this->find_idx(github_id) == -1) {
    Onion5::Node* node = this->node_list.add_nodes();
    (*node->mutable_github_id()) = github_id;
    (*node->mutable_ip_addr()) = host;
    //(*node->mutable_last_check()) = google::protobuf::util::TimeUtil::TimeTToTimestamp(time(NULL));
    std::cout << "ADDED" << std::endl;
  }
}

int NodeManage::find_idx(std::string github_id) {
  for (int i = 0; i < this->node_list.nodes_size(); ++i) {
    const Onion5::Node& node = this->node_list.nodes(i);
    std::cout << node.github_id() << std::endl;
    if (node.github_id().compare(github_id) == 0) {
      return i;
    }
  }
  return -1; // not found
}

void NodeManage::delete_user(int idx) {
  std::cout << "Deleted idx " << idx << std::endl;
  this->node_list.mutable_nodes()->DeleteSubrange(idx, 1);
}

void NodeManage::delete_user(std::string github_id) {
  std::cout << "delete target github_id " << github_id << std::endl;
  int idx = this->find_idx(github_id);
  if (idx != -1) {
    this->delete_user(idx);
  }
}

std::string NodeManage::get_binary() {
  std::ostringstream stream;
  this->node_list.SerializeToOstream(&stream);
  return stream.str();
}
