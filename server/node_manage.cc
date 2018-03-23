
#include "node.pb.h"
#include <google/protobuf/util/time_util.h>
#include "node_manage.h"
#include <fstream>
#include <sstream>

NodeManage::NodeManage() {
  GOOGLE_PROTOBUF_VERIFY_VERSION;
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

void NodeManage::delete_user(int idx) {
  this->node_list.mutable_nodes()->DeleteSubrange(1, 1);
}

std::string NodeManage::get_binary() {
  std::ostringstream stream;
  this->node_list.SerializeToOstream(&stream);
  return stream.str();
}
