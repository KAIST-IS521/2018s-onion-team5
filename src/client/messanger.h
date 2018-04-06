#pragma once

#include <string>
#include <map>
#include <vector>
#include "gpg_wrapper.h"

class Messanger {
  GPG gpg;
  std::string name;
  std::string passphrase;
  std::map<std::string, std::string> node_list;
public:
  Messanger();
  ~Messanger();
  bool interact_cred();
  std::string get_name();
  GPG& getGPG();
  std::map<std::string, std::string>& get_node_list();
  void set_name(std::string name);
  void set_passphrase(std::string passphrase);
};
