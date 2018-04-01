
#include "yang_crypto.h"

#include <string>
#include <vector>

#include "gpg_wrapper.h"
#include "message_wrapper.h"
#include <iostream>
int dec(
  std::string input_file,
  std::string my_github_id,
  std::string passphrase,
  std::string& output_file,
  std::string& github_id
) {
  GPG g;
  g.set_passphrase(passphrase);
  std::string temp_file;
  if (g.decrypt_file(input_file, temp_file)) {
  //if (true){
    //temp_file = input_file;
    Message m;
    m.deserialize(temp_file);
    output_file = m.getContent();
    std::string to = m.getTo();
    if (my_github_id.compare(to) == 0) {
      github_id = "";
      return 1;
    } else {
      github_id = to;
      return 0;
    }
  } else {
    output_file = "";
    github_id = "";
    return -1;
  }
}

std::string enc(
  std::string input_file,
  std::vector<std::string> path
) {
  GPG g;
  g.set_dummy_cred();

  std::string filename;

  std::string from = path[0];
  std::string to = path[path.size() - 1];
  path.pop_back();

  {
    Message m;
    m.setFrom(from);
    m.setTo(to);
    m.setFile(input_file);
    filename = m.serialize();
  }

  {
    Message m;
    std::string from2;
    std::string to2;
    std::string filename2;
    from2 = to;

    int size = path.size();

    for (int i = 0; i < size; ++i) {
      to2 = from2;
      from2 = path.back();
      path.pop_back();


      if (g.encrypt_file(filename, to2, filename2) == false) {
        return "";
      }

      //filename2 = filename;
      filename = filename2;

      if (from2.compare(from) == 0) {
        break;
      }

      m.setFrom(from2);
      m.setTo(to2);
      m.setBinary(filename2);
      filename = m.serialize();
      m.clear();
    }
  }

  return filename;
}
