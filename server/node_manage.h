#pragma once

#include "node.pb.h"

class NodeManage {
    Onion5::NodeList node_list;
  public:
    NodeManage();
    ~NodeManage();
    void add_user(std::string github_id, std::string host);
    void delete_user(int idx);
    void get_binary();
    void saveToFile(std::string filename);
};
