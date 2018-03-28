#pragma once

#include <string>

class GPG {
  std::string passphrase;
  std::string name;

public:
  void set_dummy_cred();
  bool is_authorized();
  bool verify_passphrase(std::string name, std::string passphrase);
  bool add_public_key(std::string locate);

  bool encrypt_file(std::string input_locate, std::string recipient, std::string& output_locate);
  bool encrypt(std::string input, std::string recipient, std::string& output);
  bool decrypt_file(std::string input_locate, std::string recipient, std::string& output_locate);
  bool decrypt(std::string input, std::string recipient, std::string& output);
};
