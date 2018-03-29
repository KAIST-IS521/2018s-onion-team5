#pragma once

#include <string>

class Message {
  std::string from;
  std::string to;

  char type;
  // message type
  //   0 binary
  //   1 text
  //   2 file

  std::string content;
  // message content
  //   0 -- FILELOC
  //   1 -- TEXT
  //   2 -- FILENAME

  //GPG g;

public:
  //Message(GPG gpg);
  Message();

  void setFrom(std::string from);
  void setTo(std::string to);

  int getType();
  void setBinary(std::string locate);
  void setContent(std::string content);
  void setFile(std::string locate);

  void clear();

  std::string serialize();

  bool deserialize(std::string input);
};

/*
  MESSAGE_HEADER_PREFIX
  FROM SIZE | FROM
  TO SIZE   | TO
  TYPE
  MESSAGE_HEADER_POSTFIX

// gpg encrypted data
  BINARY

// text
  TEXT

// file
  FILE NAME LEN | FILE NAME
  FILE CONTENT
*/
