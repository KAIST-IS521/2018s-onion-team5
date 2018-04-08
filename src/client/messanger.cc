#include "messanger.h"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <unistd.h>
#include "message_wrapper.h"
#include "gpg_wrapper.h"
#include "../common/tcp_client.h"
#include "../common/config.h"
#include "../common/util.h"
#include "ui.h"
Messanger::Messanger() {
  this->authed = false;
  this->ui = NULL;
}
Messanger::~Messanger() { }

bool Messanger::verify_passphrase(std::string name, std::string passphrase) {
  bool ret = this->gpg.verify_passphrase(name, passphrase);
  this->toogle_authed();
  if (!ret) {
    return false;
  }
  this->set_name(name);
  this->set_passphrase(passphrase);
  return true;
}

void Messanger::set_ui(UI *u) {
  this->ui = u;
}

bool Messanger::is_authed() {
  return authed;
}

bool Messanger::is_authorized() {
  //puts(this->name.c_str());
  //puts(this->passphrase.c_str());
  return !this->name.empty() && !this->passphrase.empty();
}

void Messanger::toogle_authed() {
  this->authed = ~this->authed;
}

void Messanger::set_name(std::string name) {
  this->name = name;
}

void Messanger::set_passphrase(std::string passphrase) {
  this->passphrase = passphrase;
}

std::string Messanger::get_name() {
  return this->name;
}

GPG& Messanger::getGPG() {
  return this->gpg;
}

std::map<std::string, std::string>& Messanger::get_node_list() {
  return this->node_list;
}

void Messanger::push_message(std::string from, std::string content) {
  if (this->ui == NULL) {
    return;
  }
  this->ui->push_message(from, content);
}

bool Messanger::send_message(std::string to, std::string content) {
  if (this->node_list.find(to) == this->node_list.end()) {
    //std::cout << "No such user" << std::endl;
    return false;
  }

  std::vector<std::string> route = generate_path(this->node_list, this->name, to);

  std::string filename;

  std::string str_from = route[0];
  std::string str_to = route[route.size() - 1];
  std::string str_next = route[1];

  {
    Message m;
    m.setFrom(str_from);
    m.setTo(str_to);
    m.setContent(content);
    filename = m.serialize();
  }

  {
    std::string temp_from;
    std::string temp_to;
    std::string temp_file;
    Message temp_m;

    temp_from = str_to;

    int size = route.size();

    for (int i = 0; i < size; ++i) {
      temp_to = temp_from;
      temp_from = route.back();
      route.pop_back();

      //temp_file = filename;
      this->gpg.encrypt_file(filename, temp_to, temp_file);

      temp_m.setFrom(temp_from);
      temp_m.setTo(temp_to);
      temp_m.setBinary(temp_file);
      filename = temp_m.serialize();
      temp_m.clear();
    }
  }

  TCP_Client clnt(this->node_list[str_next], NODE_PORT);
  clnt.connect();
  clnt.send_file(filename);
  clnt.close();

  return true;
}
