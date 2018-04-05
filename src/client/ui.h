#pragma once

#include <ncurses.h>
#include <map>
#include "messanger.h"

class UI {
  Messanger msgr;
  int max_width;
  int max_height;
public:
  UI(Messanger &msg);
  ~UI();
  void init_scr();
  bool checkbox_quit();
  bool login_screen();

  void interface_thread();
};
