#pragma once
#include <string>

class Client_REST {
  static const std::string base_url;
public:
  static std::string get_nodes();
  static bool add_node(std::string github_id, std::string ip_address);
  static bool delete_node(std::string github_id);
};
