#pragma once

#include <string>
#include <map>
#include <vector>
#include "gpg_wrapper.h"

class Messenger {
  GPG gpg;
  std::string name;
  std::string passphrase;
  std::map<std::string, std::string> node_list;
public:
  Messenger();
  ~Messenger();
  bool interact_cred();
  std::string get_name();
  GPG& getGPG();
  std::map<std::string, std::string>& get_node_list();
};
