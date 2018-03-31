#include <iostream>
#include <string>
#include <map>
#include <string>
#include <vector>
#include <thread>

#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>

#include <time.h>

#include "../common/util.h"
#include "../common/dumphex.h"

#include "fetch_key.h"
#include "listen.h"
#include "udp_client.h"
#include "gpg_wrapper.h"
#include "message_wrapper.h"

void test(std::string &test) {

  while (true) {
    std::cout << "In thread: " << test << std::endl;
    sleep(1);
  }

}

int main(int argc, char *argv[]) {
  GPG gpg;

  std::string name;
  std::string passphrase;

  std::map<std::string, std::string> node_list;

  std::cout << "Put your private key id: ";
  std::getline(std::cin, name);

  //char passphrase[PASS_MAX];
  passphrase = getpass("Put your passphrase: ");

  if (!gpg.verify_passphrase(name, passphrase)) {
    std::cout << "Fail to verify passphrase" << std::endl;;
    return 1;
  }

  std::cout << "Loggined" << std::endl;

  std::string input;
  //std::thread thread_listener(listener, std::ref(node_list));
  std::thread thread_listener(listener, name, std::ref(node_list));

  while (true) {
    advertise(name);
    std::cout << node_list.size() << std::endl;
    sleep(1);
  }

  exit(1);

  std::string filename;

  std::string from;
  from = "TestUser2";

  std::string to;
  to = "TestUser5";

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser2", "xptmxmdl");

    std::vector<std::string> route;

    route.push_back(from); // humm,

    route.push_back("TestUser1");
    route.push_back("TestUser3");
    route.push_back("TestUser4");

    {
      Message m;
      m.setFrom(from);
      m.setTo(to);
      //m.setContent("HELL oWorld!");
      m.setFile("/home/ahnmo/Git/2018s-onion-team5/secret.txt");

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
      gpg.encrypt_file(filename, to2, filename2);
  #else
      filename2 = filename;
  #endif
      m2.setFrom(from2);
      m2.setTo(to2);
      m2.setBinary(filename2);
      filename = m2.serialize();
      m2.clear();
    }
  }

  if (false) {
    GPG gpg;
    gpg.verify_passphrase("TestUser1", "xptmxmdlf");

    Message m;
    if (!m.deserialize(filename)) {
      std::cout << "Fail to deserialize" << std::endl;
      goto END_RELAY;
    }

    //(from.compare(m.getFrom()) == 0);
    if (to.compare(m.getTo()) == 0 && m.getType() != 0) {
      std::cout << "Oh, it's for me??" << std::endl;
    }

    END_RELAY:
    ;
  }

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser1", "xptmxmdlf");

    Message m;
    if (!m.deserialize(filename)) { }

    std::string output;
    gpg.decrypt_file(m.getContent(), output);
    // 내용 체크 스킵
    filename = output;
  }

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser3", "xptmxmtka");

    Message m;
    if (!m.deserialize(filename)) { }

    std::string output;
    gpg.decrypt_file(m.getContent(), output);
    // 내용 체크 스킵
    filename = output;
  }

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser4", "xptmxmtk");

    Message m;
    if (!m.deserialize(filename)) { }

    std::string output;
    gpg.decrypt_file(m.getContent(), output);
    // 내용 체크 스킵
    filename = output;
  }

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser5", "xptmxmdh");

    Message m;
    if (!m.deserialize(filename)) { }

    std::string output;
    gpg.decrypt_file(m.getContent(), output);
    m.clear();

    if (!m.deserialize(output)) { }
    if (m.getType() == 1) {
      std::cout << m.getContent() << std::endl;
    } else if (m.getType() == 2) {
      std::cout << 2 << ": "<< m.getContent() << std::endl;
    }
  }

  std::string x;
  x = "rm /tmp/????????????????";
  system(x.c_str());



  return 0;
}
