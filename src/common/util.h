#pragma once

#include <string>
#include <vector>
#include <map>


//////////////////////////////////////////////////////

#define IN
#define OUT

int get_file_size(IN FILE *fp);
char *rand_string(IN char *str, IN size_t size);

//////////////////////////////////////////////////////

int get_file_size(std::string locate);
std::string get_temppath();
std::string save_tempfile(std::string content);
bool write_file(std::string locate, std::string data);
bool read_file(std::string locate, std::string& data);
bool delete_file(std::string locate);
void rand_seed();
std::string random_string(size_t length);

std::vector<std::string> generate_path(
  std::map<std::string, std::string> nodelist,
  std::string from,
  std::string to,
  int count
);

std::vector<std::string> generate_path(
  std::map<std::string, std::string> nodelist,
  std::string from,
  std::string to
);


bool is_existed_file (const std::string& name);

#if 1
#define BSWAP32(x) __builtin_bswap32(x)
#define BSWAP64(x) __builtin_bswap64(x)
#else
#define BSWAP32(val) \
 ( (((val) >> 24) & 0x000000FF) | (((val) >>  8) & 0x0000FF00) | \
   (((val) <<  8) & 0x00FF0000) | (((val) << 24) & 0xFF000000) )

#define BSWAP64(val) \
 ( (((val) >> 56) & 0x00000000000000FF) | (((val) >> 40) & 0x000000000000FF00) | \
   (((val) >> 24) & 0x0000000000FF0000) | (((val) >>  8) & 0x00000000FF000000) | \
   (((val) <<  8) & 0x000000FF00000000) | (((val) << 24) & 0x0000FF0000000000) | \
   (((val) << 40) & 0x00FF000000000000) | (((val) << 56) & 0xFF00000000000000) )
#endif
