#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <map>
#include <string>
#include <vector>

#include "fetch_key.h"
#include "listen.h"
#include "gpg_wrapper.h"
#include "../common/util.h"
#include "../common/dumphex.h"
#include "message_wrapper.h"
#include "yang_crypto.h"

int main(int argc, char *argv[]) {
  /*

  GPG gpg;
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

  /*
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

  */

  /*
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
      m.setContent("HELL oWorld!");
      //m.setFile("/home/ahnmo/Git/2018s-onion-team5/secret.txt");

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
      filename = filename2;
      if (from2.compare(from) == 0) {
        break;
      }

      m2.setFrom(from2);
      m2.setTo(to2);
      m2.setBinary(filename2);
      filename = m2.serialize();
      m2.clear();
    }
  }
  */

//  std::string xxx = "xxd " + filename;
//  system(xxx.c_str());

/*
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
*/
/*
  {
    GPG gpg;
    gpg.verify_passphrase("TestUser1", "xptmxmdlf");

    std::string output;
    if (!gpg.decrypt_file(filename, output)) {
      std::cerr << "Fail to decrypt file" << std::endl;
    }

    Message m;
    if (!m.deserialize(output)) {
      std::cerr << "Fail to deserialize" << std::endl;
    }

    int type = m.getType();
    filename = m.getContent();
  }

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser3", "xptmxmtka");

    std::string output;
    if (!gpg.decrypt_file(filename, output)) {
      std::cerr << "Fail to decrypt file" << std::endl;
    }

    Message m;
    if (!m.deserialize(output)) {
      std::cerr << "Fail to deserialize" << std::endl;
    }

    int type = m.getType();
    filename = m.getContent();
  }

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser4", "xptmxmtk");

    std::string output;
    if (!gpg.decrypt_file(filename, output)) {
      std::cerr << "Fail to decrypt file" << std::endl;
    }

    Message m;
    if (!m.deserialize(output)) {
      std::cerr << "Fail to deserialize" << std::endl;
    }

    int type = m.getType();
    filename = m.getContent();
  }

  {
    GPG gpg;
    gpg.verify_passphrase("TestUser5", "xptmxmdh");

    std::string output;
    if (!gpg.decrypt_file(filename, output)) {
      std::cerr << "Fail to decrypt file" << std::endl;
    }

    Message m;
    if (!m.deserialize(output)) {
      std::cerr << "Fail to deserialize" << std::endl;
    }

    int type = m.getType();
    filename = m.getContent();

    if (m.getType() == 1) {
      std::cout << m.getContent() << std::endl;
      std::string x;
      x = "xxd " + m.getContent();
      system(x.c_str());
    } else if (m.getType() == 2) {
      std::cout << 2 << ": "<< m.getContent() << std::endl;
    }
  }

  std::string x;
  x = "rm /tmp/????????????????";
  system(x.c_str());
*/

  system("echo asdfasdfasdfasdfasdfasdf > /tmp/input");

  std::vector<std::string> routepath;
  routepath.push_back("TestUser2");
  routepath.push_back("TestUser1");
  routepath.push_back("TestUser3");
  routepath.push_back("TestUser4");
  routepath.push_back("TestUser5");

  std::string filepath;
  filepath = enc("/tmp/input", routepath);

  std::cout << filepath << std::endl;

  std::string cmd = "xxd " + filepath;
  system(cmd.c_str());

  routepath.clear();

  int ret;
  std::string output;
  std::string to;


  std::vector<std::vector<std::string>> gpg_info;
  gpg_info.push_back(std::vector<std::string> {"TestUser1", "xptmxmdlf"});
  gpg_info.push_back(std::vector<std::string> {"TestUser3", "xptmxmtka"});
  gpg_info.push_back(std::vector<std::string> {"TestUser4", "xptmxmtk"});
  gpg_info.push_back(std::vector<std::string> {"TestUser5", "xptmxmdh"});

  for (auto it = gpg_info.begin(); it != gpg_info.end(); ++it){

    ret = dec(filepath, (*it)[0], (*it)[1], output, to);
    if (ret == 1) {
      std::cout << "Oh! It's for me!" << std::endl;
      // put message to ui
      // to do this thing we have to know from
      //  ???, ???: message
      std::cout << "File:" << output << std::endl;
      std::string cmd = "xxd " + output;
      system(cmd.c_str());
    } else if (ret == 0) {
      std::cout << "This is not mine... send to " << to << std::endl;
      // relay to other
      filepath = output;
    } else if (ret == -1) {
      std::cout << "Weired packet... " << std::endl;
      std::cout << "STOP" << std::endl;
      // drop
      exit(1);
    } else {
      std::cout << "WHATTHEFUCK" << std::endl;
      std::cout << "STOP" << std::endl;
      // drop
      exit(1);
    }
  }





  std::cout << "DONE" << std::endl;
  system("rm /tmp/input");

  return 0;
}
