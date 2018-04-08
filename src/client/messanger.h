#pragma once

#include <string>
#include <map>
#include <vector>
#include "gpg_wrapper.h"
#include "ui.h"

class Messanger {
  GPG gpg;
  std::string name;
  std::string passphrase;
  std::map<std::string, std::string> node_list;
  bool authed;
  UI *ui;
public:
  Messanger();
  ~Messanger();
  bool verify_passphrase(std::string name, std::string passphrase);
  bool is_authed();
  bool is_authorized();
  void toogle_authed();
  void set_ui(UI *ui);
  std::string get_name();
  GPG& getGPG();
  std::map<std::string, std::string>& get_node_list();
  void set_name(std::string name);
  void set_passphrase(std::string passphrase);
  bool send_message(std::string to, std::string content);
  void push_message(std::string from, std::string content);
};
