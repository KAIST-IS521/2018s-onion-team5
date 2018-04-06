#pragma once

#include <ncurses.h>
#include <map>
#include <vector>
#include <string>
#include "messanger.h"

#define KEY_ESCAPE 27
#define KEY_ENTER2 10

class UI {
  Messanger msgr;
  int max_width;
  int max_height;
  std::map<std::string, std::vector<std::string>> chat;
public:
  UI(Messanger &msg);
  ~UI();
  void init_scr();
  bool checkbox_quit();
  bool login_screen();

  void interface_thread();

  //WINDOW **print_userlist();
  int select_user();
};
