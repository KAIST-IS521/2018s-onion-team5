
#include <string>


class GPG {
  std::string passphrase;
  std::string name;

public:
  void set_dummy_cred();
  bool is_authorized();
  bool verify_passphrase(std::string name, std::string passphrase);
  bool add_public_key(std::string locate);
};
