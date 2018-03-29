#include "message_wrapper.h"
#include "../common/config.h"
#include "../common/util.h"
#include <time.h>
#include <fstream>
#include <string>
#include <libgen.h>
#include <cstring>

#include <iostream>

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

Message::Message() {
  this->clear();
}

void Message::setFrom(std::string from) {
  this->from = from;
}

std::string Message::getFrom() {
  return this->from;
}

void Message::setTo(std::string to) {
  this->to = to;
}

std::string Message::getTo() {
  return this->to;
}

int Message::getType() {
  return this->type;
}
std::string Message::getContent() {
  return this->content;
}

void Message::setBinary(std::string locate) {
  this->type = 0;
  this->content = locate;
}

void Message::setContent(std::string content) {
  this->type = 1;
  this->content = content;
}

void Message::setFile(std::string locate) {
  this->type = 2;
  this->content = locate;
}

void Message::clear() {
  this->from.clear();
  this->to.clear();
  this->type = -1;
  this->content.clear();
}

// return filename
std::string Message::serialize() {

  std::string locate = get_temppath();
  time_t t = 0;
  size_t s = 0;
  //int i = 0;

  std::ofstream os;
  os.open(locate, std::ofstream::binary);

  os.write(MESSAGE_PREFIX, 2); // 2 byte

  s = BSWAP64(this->from.size());// 8 byte
  os.write((char *) &s, sizeof(size_t));
  os.write(this->from.c_str(), this->from.size());

  s = BSWAP64(this->to.size());// 8 byte
  os.write((char *) &s, sizeof(size_t));
  os.write(this->to.c_str(), this->to.size());

  t = BSWAP64(time(NULL)); // 8 byte
	os.write((char *) &t, sizeof(time_t)); // 8 byte

  //i = BSWAP32(this->type);
  os.write((char *) &this->type, sizeof(char)); // 4byte

  os.write(MESSAGE_POSTFIX, 2); // 8 byte

  if (this->type == 2) {
    std::string filename = basename(&this->content[0]);
    s = BSWAP64(filename.size());// 8 byte
    os.write((char *) &s, sizeof(size_t));
    os.write(filename.c_str(), filename.size());
  }

  if (this->type == 0 || this->type == 2) {
    std::ifstream is;
    is.open(this->content, std::ifstream::binary);
    if (!is) {
      std::cout << "fail to open" << std::endl;
      // fail to open
    }
    while(!is.eof()) {
      char buff[1024 + 1] = {0};
      is.read(buff, 1024);
      size_t read_size = is.gcount();
      os.write(buff, read_size);
    }
    is.close();
  } else if (this->type == 1) {
    os.write(content.c_str(), content.size());
  }

  os.close();

  return locate;
}

bool Message::deserialize(std::string input) {
  bool ret = false;
  time_t t = 0;
  size_t s = 0;

  std::ifstream is;
  is.open(input, std::ifstream::binary);
  if (!is) {
    std::cout << "fail to open y" << std::endl;
  }

  char buff[1024 + 1] = {0};
  size_t read_size;

  //is.read(buff, 2);
  //read_size = is.gcount();
  //buff[2] = 0;

  is.read(buff, 2);
  if (memcmp(buff, MESSAGE_PREFIX, 2) != 0) {
    goto END_DESERIALIZE;
  }

  is.read((char *) &s, sizeof(size_t));
  s = BSWAP64(s);
  this->from.resize(s);

  is.read(buff, sizeof(char) * s);
  this->from.assign(buff, s);

  is.read((char *) &s, sizeof(size_t));
  s = BSWAP64(s);
  this->to.resize(s);

  is.read(buff, sizeof(char) * s);
  this->to.assign(buff, s);

  is.read((char *) &t, sizeof(time_t));
  t = BSWAP64(t); // 8 byte

  is.read((char *) &this->type, sizeof(char));

  is.read(buff, 2);
  if (memcmp(buff, MESSAGE_POSTFIX, 2) != 0) {
    goto END_DESERIALIZE;
  }

  std::cout << this->from << std::endl;
  std::cout << this->to << std::endl;
  std::cout << t << std::endl;
  std::cout << (short)this->type << std::endl;

  if (this->type == 1) {
    while(!is.eof()) {
      char buff[1024 + 1] = {0};
      is.read(buff, 1024);
      size_t read_size = is.gcount();
      buff[read_size] = 0;
      this->content.append(buff);
    }
    //is.read((char *) &s, sizeof(size_t));
    //this->content.resize(BSWAP64(s));
    //is.read(&this->content[0], sizeof(char) * s);
  } else {

    std::string locate;

    if (this->type == 2) {
      is.read((char *) &s, sizeof(size_t));
      //locate.resize();
      s = BSWAP64(s);
      char *buff = new char[s];
      is.read(buff, sizeof(char) * s);
      size_t read_size = is.gcount();
      buff[read_size] = 0;
      locate = buff;
      delete buff;
    } else if (this->type == 0) {
      locate = get_temppath();
    }

    if (locate.empty()) {
      goto END_DESERIALIZE;
    }

    this->content = locate;
    std::cout << this->content << std::endl;

    std::ofstream os;
    os.open(locate, std::ofstream::binary);

    if (!os) {
      std::cout << "fail to open x" << std::endl;
      // fail to open
    }
    while(!is.eof()) {
      char buff[1024 + 1] = {0};
      is.read(buff, 1024);
      size_t read_size = is.gcount();
      os.write(buff, read_size);
    }
    os.close();

    std::string temp;
    temp = "xxd " + this->content;
    system(temp.c_str());
  }
  std::cout << this->content << std::endl;

  ret = true;
END_DESERIALIZE:
  if (is) is.close();

  if (ret == false) {
    this->clear();
  }

  return ret;
}
