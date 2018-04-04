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
#include "messenger.h"


void adver_loop(std::string name) {
  while (true) {
    advertise(name);
    ::sleep(5);
  }
}

bool send_message(std::string to, std::string content, Messenger &msgr) {
  std::map<std::string, std::string> node_list = msgr.get_node_list();
  GPG &g = msgr.getGPG();
  std::vector<std::string> route = generate_path(node_list, msgr.get_name(), to);
  for (auto it: route)
    DumpHex(it);

  std::string filename;

  std::string str_from = route[0];
  std::string str_to = route[route.size() - 1];
  std::string str_next = route[1];
  route.pop_back();

/*
  {
    Message m;
    m.setFrom(str_from);
    m.setTo(str_to);
    m.setContent(content);
    filename = m.serialize();
  }

  {
    Message m;
    std::string from2;
    std::string to2;
    std::string filename2;
    from2 = str_to;

    int size = path.size();

    for (int i = 0; i < size; ++i) {
      to2 = from2;
      from2 = path.back();
      path.pop_back();


      if (g.encrypt_file(filename, to2, filename2) == false) {

        return false;
      }

      m.setFrom(from2);
      m.setTo(to2);
      m.setBinary(filename2);
      filename = m.serialize();
      m.clear();
    }
  }
*/

{
    Message m;
    m.setFrom(str_from);
    m.setTo(str_to);
    m.setContent(content);

    filename = m.serialize();
  }

  std::string from2;
  std::string to2;
  std::string filename2;
  Message m2;

  from2 = to;

  int size = route.size();
  for (int i = 0; i < size; ++i){
    to2 = from2;
    from2 = route.back();
    route.pop_back();

#if 1
    g.encrypt_file(filename, to2, filename2);
#else
    filename2 = filename;
#endif
    filename = filename2;

    m2.setFrom(from2);
    m2.setTo(to2);
    m2.setBinary(filename2);
    filename = m2.serialize();
    m2.clear();
  }

  std::string xxxx;
read_file(filename, xxxx);
  DumpHex(xxxx);

  TCP_Client clnt(node_list[str_next], NODE_PORT);
  clnt.connect();
  clnt.send_file(filename);
  clnt.close();

  return true;
}

void input_loop(Messenger &msgr) {
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
  Messenger msgr;

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
