#include "ui.h"

#include "messanger.h"
#include <iostream>
#include <ncurses.h>
#include <map>
#include <thread>
#include <string>

#include "../common/dumphex.h"

UI::UI(Messanger &msgr) {
  this->msgr = msgr;
}

UI::~UI() {
  endwin();
}

void UI::init_scr() {
/*
COLOR_BLACK COLOR_RED COLOR_GREEN COLOR_YELLOW
COLOR_BLUE COLOR_MAGENTA COLOR_CYAN COLOR_WHITE
*/
	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK); // foreground, background
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_WHITE, COLOR_YELLOW);
	init_pair(4, COLOR_BLACK, COLOR_WHITE);
	curs_set(2);
	noecho();
	keypad(stdscr, TRUE);

  getmaxyx(stdscr, this->max_height, this->max_width);

}

bool UI::checkbox_quit() {
  WINDOW *check;
  int key;

  check = newwin(3, 40, this->max_height / 2 - 2, this->max_width / 2 - 20);

  wmove(check, 1, 2);
  wprintw(check, "Exit program (y/N) ? ");
  wbkgd(check, COLOR_PAIR(2));
  box(check, 0, 0);
  refresh();

  key = wgetch(check);
  delwin(check);

  return key == 'y' || key == 'Y';
}

bool UI::login_screen() {
  std::string githubid(81, '\0');
  std::string password(81, '\0');

  WINDOW *passwindow = newwin(6, 80, this->max_height / 2 - 3, this->max_width / 2 - 40);
  refresh();
  box(passwindow, 0, 0);
  wbkgd(passwindow, COLOR_PAIR(3));

  echo();
  mvwprintw(passwindow, 1, 5, "Enter the ID: ");
  wmove(passwindow, 1, 20);
  wgetnstr(passwindow, &githubid[0], 80);

  noecho();
	mvwprintw(passwindow, 3, 5, "Passphrase: ");
	wmove(passwindow, 3, 18);
	wgetnstr(passwindow, &password[0], 80);

  delwin(passwindow);

  touchwin(stdscr);
	refresh();

  githubid = githubid.substr(0, githubid.find('\0'));
  password = password.substr(0, password.find('\0'));
  //password.replace(password.begin(), password.end(), "\0", "");

  GPG &g = this->msgr.getGPG();
  bool ret = g.verify_passphrase(githubid, password);

  if (ret) {
    this->msgr.set_name(githubid);
    this->msgr.set_passphrase(password);
  }

  return ret;
}

int UI::select_user() {
  int i;
  WINDOW **items;

  std::map<std::string, std::string> &userlist = this->msgr.get_node_list();
  std::vector<std::string> userlist_name;

  for (auto it = userlist.begin(); it != userlist.end(); ++it) {
    if (this->msgr.get_name().compare((* it).first) == 0) continue;
    userlist_name.push_back((* it).first);
  }

  int start_col = this->max_width / 2 - 20;
  items = new WINDOW * [userlist_name.size() + 1];

  // outter
  items[0] = newwin(20, 40, 5, start_col);
  wbkgd(items[0], COLOR_PAIR(3));
  box(items[0], ACS_VLINE, ACS_HLINE);

  // inner
  for (i = 0; i < userlist_name.size(); i++) {
  	items[i + 1] = subwin(items[0], 1, 17, 6 + i, start_col + 1);
    wprintw(items[i + 1], "%s", userlist_name[i].c_str());
  }

  // select
  int selected = 0, count = userlist_name.size();
  int key, ret = -2;
  while (1) {
    wbkgd(items[selected + 1], COLOR_PAIR(2));
    wnoutrefresh(items[selected + 1]); // color on
    wrefresh(items[0]);
    doupdate();
    key = getch();
    //wprintw(items[0], "%d", selected);
    if (key == KEY_DOWN) {
      wbkgd(items[selected + 1], COLOR_PAIR(3));
      wnoutrefresh(items[selected + 1]); // color off
      selected = (selected + 1) % count;
      continue;
    }
    if (key == KEY_UP) {
      wbkgd(items[selected + 1], COLOR_PAIR(3));
      wnoutrefresh(items[selected + 1]); // color off
      selected = (selected + count - 1) % count;
      continue;
    }
    if (key == KEY_ESCAPE) {
      if (this->checkbox_quit()) {
        ret = -1;
        break;
      }
      wbkgd(items[0], COLOR_PAIR(3));
      box(items[0], ACS_VLINE, ACS_HLINE);
    }
    if (key == KEY_ENTER2) {
      ret = selected;
      break;
    }
  }
  for (i = 0; i < userlist_name.size(); i++) {
    delwin(items[i]);
  }
  delete items;
  return ret;
}


void UI::interface_thread() {
  this->init_scr();

  if (!this->login_screen()) {
    // login fail
    return;
  }

  while (true) {
    int idx;
    noecho();
    if ((idx = this->select_user()) < 0) {
      break;
    }

  }

}

int main(int argc, char *argv[]) {
  Messanger m;

  std::map<std::string, std::string>&test = m.get_node_list();
  test.insert(std::pair<std::string, std::string>("TestUser1", "172.17.0.3"));
  test.insert(std::pair<std::string, std::string>("TestUser2", "172.17.0.4"));
  test.insert(std::pair<std::string, std::string>("TestUser3", "172.17.0.5"));
  test.insert(std::pair<std::string, std::string>("TestUser4", "172.17.0.6"));
  test.insert(std::pair<std::string, std::string>("TestUser5", "172.17.0.7"));


UI cui(m);

  std::thread interface_th(&UI::interface_thread, cui);
  interface_th.join();

  return 0;
}
