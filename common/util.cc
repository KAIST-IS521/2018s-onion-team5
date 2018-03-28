
#include "util.h"
#include <string>
#include <fstream>
#include <cstdio>
#include <algorithm>

std::string save_tempfile(std::string content) {
  std::string path("/tmp/");
  path += random_string(16);

  std::fstream fs;
  fs.open(path, std::fstream::in | std::fstream::out | std::fstream::app);
  fs << content;
  fs.close();

  return path;
}

bool delete_file(std::string locate) {
  if (remove(locate.c_str()) == 0) {
    return true;
  } else {
    perror("remove");
    return false;
  }
}


std::string random_string(size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}
