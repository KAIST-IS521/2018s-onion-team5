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
#include "message_wrapper.h"

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
  /*
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

  if (!delete_file(filename)) {
    std::cout << "Fail to delete" << std::endl;;
  }


  std::string tempfile = save_tempfile("test input");
  std::string outputfile;
  std::cout << "tempfile: " << tempfile << std::endl;

  std::cout << "===========================================" << std::endl;
  std::cout << "gpg.encrypt_file(tempfile, \"TestUser6\", outputfile)" << std::endl;
  if (!gpg.encrypt_file(tempfile, "TestUser6", outputfile)) {
    std::cout << "Encrypt file fail..." << std::endl;
  }
  std::cout << outputfile << std::endl;

  std::cout << "===========================================" << std::endl;
  std::cout << "gpg.encrypt_file(tempfile, \"TestUser3\", outputfile)" << std::endl;
  if (!gpg.encrypt_file(tempfile, "TestUser3", outputfile)) {
    std::cout << "Encrypt file fail..." << std::endl;
  }
  std::cout << outputfile << std::endl;

  std::string output;
  output = "ls -al " + outputfile + "; xxd " + outputfile;

  system(output.c_str());

  if (!delete_file(outputfile)) {
    std::cout << "Fail to delete" << std::endl;;
  }

  output.clear();

  std::cout << "===========================================" << std::endl;
  std::cout << "gpg.encrypt(\"Hello World!\", \"TestUser6\", output)" << std::endl;
  if (!gpg.encrypt("Hello World!", "TestUser6", output)) {
    std::cout << "Encrypt file fail..." << std::endl;
  }
  std::cout << output << std::endl;

  std::cout << "===========================================" << std::endl;
  std::cout << "gpg.encrypt(\"Hello World!\", \"TestUser3\", output)" << std::endl;
  if (!gpg.encrypt("Hello World!", "TestUser3", output)) {
    std::cout << "Encrypt file fail..." << std::endl;
  }
  //std::cout << output << std::endl;
  DumpHex(output);

  std::cout << "===========================================" << std::endl;
  std::cout << "Delete tempfile!" << std::endl;;
  if (!delete_file(tempfile)) {
    std::cout << "Fail to delete" << std::endl;
  }
  */
  {
    Message m;
    m.setFrom("AhnMo");
    m.setTo("S-KYUCHAN");
    m.setContent("this is test message");

    std::string filename = m.serialize();
    std::string temp;
    temp = "xxd " + filename;
    system(temp.c_str());

    {
      Message m2;
      m2.deserialize(filename);
    }

    delete_file(filename);
  }

  {
    Message m;
    m.setFrom("AhnMo");
    m.setTo("S-KYUCHAN");
    m.setBinary("test.txt");

    std::string filename = m.serialize();
    std::string temp;
    temp = "xxd " + filename;
    system(temp.c_str());

    {
      Message m2;
      m2.deserialize(filename);
    }

    delete_file(filename);
  }

  {
    Message m;
    m.setFrom("AhnMo");
    m.setTo("S-KYUCHAN");
    m.setFile("/home/ahnmo/Git/2018s-onion-team5/test.txt");

    std::string filename = m.serialize();
    std::string temp;
    temp = "xxd " + filename;
    system(temp.c_str());

    {
      Message m2;
      m2.deserialize(filename);
    }

    delete_file(filename);
  }


  return 0;
}