// g++ -o test_generate_path test_generate_path.cc common/util.cc -std=c++11
#include <map>
#include <vector>
#include <string>
#include <time.h>
#include <algorithm>
#include <iostream>
#include "common/util.h"

int main(int argc, char *argv[]) {
  //std::map<std::string, std::string> user_list;
  std::map<std::string, std::string> nodelist = std::map<std::string, std::string> {
    std::pair<std::string, std::string> {"TestUser1", "172.17.0.2"},
    std::pair<std::string, std::string> {"TestUser2", "172.17.0.3"},
    std::pair<std::string, std::string> {"TestUser3", "172.17.0.4"},
    std::pair<std::string, std::string> {"TestUser4", "172.17.0.5"},
    std::pair<std::string, std::string> {"TestUser5", "172.17.0.6"},
    std::pair<std::string, std::string> {"TestUser6", "172.17.0.7"},
    std::pair<std::string, std::string> {"TestUser7", "172.17.0.8"},
    std::pair<std::string, std::string> {"TestUser8", "172.17.0.9"}
  };

/*
  std::cout << "========================================" << std::endl;
  for (auto it: nodelist) {
    std::cout << it.first << " " << it.second << std::endl;
  }
*/
  std::string from = "TestUser2";
  std::string to = "TestUser5";

  std::vector<std::string> path = generate_path(nodelist, from, to);

  std::cout << "========================================" << std::endl;
  for (auto it: path) {
    std::cout << it << std::endl;
  }

/*
  std::cout << "========================================" << std::endl;
  for (auto it: nodelist) {
    std::cout << it.first << " " << it.second << std::endl;
  }
*/

  return 0;
}
