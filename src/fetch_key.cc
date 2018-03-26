#include "../common/config.h"
#include "fetch_key.h"

int fetch_key_from_github(std::string user_id, std::string& str) {
  std::string url = "https://raw.githubusercontent.com/" + base_repo + "/2018-Spring/master/IndividualKeys/" + user_id + ".pub";
  return HTTP::_get(url, str);
}
