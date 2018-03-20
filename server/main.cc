#include <iostream>
#include <fstream>
#include <thread>
#include <ctime>
#include "node.pb.h"
#include <google/protobuf/util/time_util.h>


void call_from_thread() {
  std::cout << "Hello, World" << std::endl;
}

int main() {

  GOOGLE_PROTOBUF_VERIFY_VERSION;

  Onion5::NodeList node_list;

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
  return 0;
}
