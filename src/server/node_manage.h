#pragma once

#include <string>
#include "../common/node.pb.h"

class NodeManage {
    Onion5::NodeList node_list;
  public:
    NodeManage();
    ~NodeManage();
    void add_user(std::string github_id, std::string host);
    int find_idx(std::string github_id);
    void delete_user(int idx);
    void delete_user(std::string github_id);
    std::string get_binary();
};
