#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <map>
#include <string>


#include "fetch_key.h"
#include "listen.h"
#include "gpg_wrapper.h"
#include "../common/util.h"
#include "../common/dumphex.h"

int main(int argc, char *argv[]) {
  GPG gpg;

  /*
  std::string name;
  std::cout << "Put your private key id: ";

  std::getline(std::cin, name);

  DumpHex(name);

  //char passphrase[PASS_MAX];
  char *passphrase = NULL;
  passphrase = getpass("Put your passphrase: ");

  if (!gpg.verify_passphrase(name, passphrase)) {
    std::cout << "Fail to verify passphrase" << std::endl;;
    return 1;
  }

  std::cout << "Loggined" << std::endl;
  */
  gpg.set_dummy_cred();

  std::map<std::string, std::string> node_list;
  //listener("AhnMo", node_list);

  std::string ret;
  if (fetch_key_from_github("TestUser2", ret)) {
    std::cout << ret << std::endl;
  } else {
    std::cout << "[!] Error to fetch key" << std::endl;
    return 1;
  }

  std::string filename = save_tempfile(ret);

  gpg.add_public_key(filename);





  return 0;
}
