#include "messenger.h"
#include <string>
#include <map>
#include <vector>
#include <iostream>
#include <unistd.h>

#include "gpg_wrapper.h"

Messenger::Messenger() { }
Messenger::~Messenger() { }

bool Messenger::interact_cred() {
  std::string name;
  std::string passphrase;

  std::cout << "Put your private key id: ";
  std::getline(std::cin, name);

  //char passphrase[PASS_MAX];
  passphrase = getpass("Put your passphrase: ");
  if (!this->gpg.verify_passphrase(name, passphrase)) {
    std::cout << "Fail to verify passphrase" << std::endl;
    return false;
  }
  this->name = name;
  this->passphrase = passphrase;
  return true;
}

std::string Messenger::get_name() {
  return this->name;
}

GPG& Messenger::getGPG() {
  return this->gpg;
}

std::map<std::string, std::string>& Messenger::get_node_list() {
  return this->node_list;
}
