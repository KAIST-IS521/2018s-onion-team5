#pragma once

#include <string>

extern const std::string base_repo;
extern const int BROADCAST_PORT;
extern const int NODE_PORT;
extern const int REST_SERV_PORT;

#define LIST_PREFIX  "\xD1\x5E"
#define LIST_POSTFIX "\xAE\xEE"

#define PING_PREFIX  "\xFE\xED"
#define PING_POSTFIX "\xFA\xCE"

#define PONG_PREFIX  "\xFE\xED"
#define PONG_POSTFIX "\xF0\xDD"
