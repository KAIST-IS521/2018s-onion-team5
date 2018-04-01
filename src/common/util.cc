
#include "util.h"
#include <string>
#include <fstream>
#include <cstdio>
#include <algorithm>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

//////////////////////////////////////////////////////

int get_file_size(IN FILE *fp) {
  int size;

  if (fseek(fp, 0, SEEK_END)) {
    perror("fseek");
    return -1;
  }

  size = ftell(fp);
  if (size == -1) {
    perror("ftell");
    return -1;
  }

  if (fseek(fp, 0, SEEK_SET)) {
    perror("fseek");
    return -1;
  }

  return size;
}

char *rand_string(IN char *str, IN size_t size)
{
  const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ.0123456789";
  if (size) {
    --size;
    for (size_t n = 0; n < size; n++) {
      int key = rand() % (int) (sizeof charset - 1);
      str[n] = charset[key];
    }
    str[size] = '\0';
  }
  return str;
}

//////////////////////////////////////////////////////

int get_file_size(std::string locate) {
  struct stat buf;
  int ret;

  ret = stat(locate.c_str(), &buf);
  if (ret != 0) {
    return -1;
  }
  return buf.st_size;
}

std::string get_temppath() {
  std::string path("/tmp/");
  path += random_string(16);
  return path;
}

std::string save_tempfile(std::string content) {
  std::string path = get_temppath();

  if (write_file(path, content)) {
    return path;
  } else {
    return "";
  }
}

// http://www.cplusplus.com/reference/ostream/ostream/write/
bool write_file(std::string locate, std::string data) {
  bool ret;

  std::ofstream os;
  os.open(locate, std::ofstream::binary);
  os.write(data.c_str(), data.size());

  ret = true;
END_WRITE_FILE:
  if (os) os.close();

  return ret;
}

// http://www.cplusplus.com/reference/istream/istream/read/
bool read_file(std::string locate, std::string& data) {
  bool ret = false;
  int length = -1;
  char *buffer = NULL;

  std::ifstream is;
  is.open(locate, std::ifstream::binary);
  if (!is) {
    // fail to open
    goto END_READ_FILE;
  }

#if 0
  length = get_file_size(locate.c_str());
#else
  is.seekg (0, is.end);
  length = is.tellg();
  is.seekg (0, is.beg);
#endif

  buffer = new char[length];

  is.read(buffer,length);

  if (is) {
    //std::cout << "all characters read successfully.";
    data = buffer;
  } else {
    //std::cout << "error: only " << is.gcount() << " could be read";
    goto END_READ_FILE;
  }
  ret = true;
END_READ_FILE:
  if (is) is.close();
  if (buffer) delete buffer;

  return ret;
}

bool delete_file(std::string locate) {
  if (remove(locate.c_str()) == 0) {
    return true;
  } else {
    perror("remove");
    return false;
  }
}

bool rand_initialized = false;
void rand_seed() {
  int fd = open("/dev/urandom", O_RDONLY);
  unsigned int seed;
  if (read(fd, &seed, sizeof(unsigned int)) != sizeof(unsigned int)) {
    // error read
  }
  close(fd);
  srand(seed);
  rand_initialized = true;
}
std::string random_string(size_t length) {
  if (!rand_initialized) {
    rand_seed();
  }
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

std::vector<std::string> generate_path(
  std::map<std::string, std::string> nodelist,
  std::string from,
  std::string to,
  int count
) {
  if (!rand_initialized) {
    rand_seed();
  }

  std::vector<std::string> keys;
  std::vector<std::string> result;

  for (auto const& s: nodelist)
    keys.push_back(s.first);

  keys.erase(std::remove(keys.begin(), keys.end(), from), keys.end());
  keys.erase(std::remove(keys.begin(), keys.end(),   to), keys.end());

  auto rng = std::default_random_engine {};
  rng.seed(rand());
  std::shuffle(std::begin(keys), std::end(keys), rng);

  result.push_back(from);
  for (int i = 0; i < count; ++i)
    result.push_back(keys[i]);
  result.push_back(to);

  return result;
}

std::vector<std::string> generate_path(
  std::map<std::string, std::string> nodelist,
  std::string from,
  std::string to
) {
  return generate_path(nodelist, from, to, 3);
}
