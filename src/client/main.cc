#include <iostream>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <thread>

#include <unistd.h>

#include <time.h>

#include "../common/util.h"
#include "../common/dumphex.h"
#include "../common/tcp_server.h"
#include "../common/config.h"
#include "fetch_key.h"
#include "listen.h"
#include "udp_client.h"
#include "gpg_wrapper.h"
#include "message_wrapper.h"
#include "yang_crypto.h"
#include "messanger.h"


void adver_loop(std::string name) {
  while (true) {
    advertise(name);
    ::sleep(5);
  }
}

bool send_message(std::string to, std::string content, Messanger &msgr) {
  std::map<std::string, std::string> node_list = msgr.get_node_list();
  GPG &g = msgr.getGPG();
  std::vector<std::string> route = generate_path(node_list, msgr.get_name(), to);
  for (auto it: route)
    DumpHex(it);

  std::string filename;

  std::string str_from = route[0];
  std::string str_to = route[route.size() - 1];
  std::string str_next = route[1];

  {
    Message m;
    m.setFrom(str_from);
    m.setTo(str_to);
    m.setContent(content);
    //m.setFile("../01.Memory_And_Kernel_Intro.pdf");
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
      g.encrypt_file(filename, temp_to, temp_file);

      temp_m.setFrom(temp_from);
      temp_m.setTo(temp_to);
      temp_m.setBinary(temp_file);
      filename = temp_m.serialize();
      temp_m.clear();
    }
  }

  TCP_Client clnt(node_list[str_next], NODE_PORT);
  clnt.connect();
  clnt.send_file(filename);
  clnt.close();

  return true;
}

void input_loop(Messanger &msgr) {
  std::map<std::string, std::string> &node_list = msgr.get_node_list();
  while (true) {
    std::string user;
    std::cout << " > ";
    std::getline(std::cin, user);
    if (node_list.find(user) == node_list.end()) {
      std::cout << "No such user" << std::endl;
      continue;
    }

    std::string message;
    std::cout << " " << user << " > ";
    std::getline(std::cin, message);

    if (send_message(user, message, msgr)) {
      std::cout << "[!] Success to send" << std::endl;
    } else {
      std::cout << "[!] Fail to send" << std::endl;
    }
  }
}

int main(int argc, char *argv[]) {
  Messanger msgr;

  if (msgr.interact_cred() == false) {
    return 1;
  }
  std::cout << "Loggined" << std::endl;


  std::thread thread_liste(liste_loop, std::ref(msgr));
  std::thread thread_adver(adver_loop, msgr.get_name());
  std::thread thread_input(input_loop, std::ref(msgr));

  thread_liste.join();
  thread_adver.join();
  thread_input.join();

  return 0;
}
