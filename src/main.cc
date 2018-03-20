#include <iostream>
#include <string>

#include "fetch_key.hh"

//using namespace ;

int main(int argc, char *argv[]) {

  std::string ret;
  if (fetch_key_from_github("AhnMo", ret) == 0) {
    std::cout << ret << std::endl;
  } else {
    std::cout << "[!] Error to fetch key" << std::endl;
  }


  return 0;
}
