#include "ui.h"

#include "messanger.h"
#include <iostream>
#include <ncurses.h>
#include <map>
#include <thread>
#include <string>
#include <unistd.h>
#include "../common/dumphex.h"

UI::UI(Messanger *msgr) {
  this->msgr = msgr;
  this->current = NULL;
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

  if (msgr->verify_passphrase(githubid, password)) {
    this->msgr->set_name(githubid);
    this->msgr->set_passphrase(password);
    return true;
  }

  return false;
}

int UI::select_user() {
  int i;
  WINDOW **items;

  int start_col = this->max_width / 2 - 20;
  items = new WINDOW * [this->userlist.size() + 1];

  // outter
  items[0] = newwin(20, 40, 5, start_col);
  wbkgd(items[0], COLOR_PAIR(3));
  box(items[0], ACS_VLINE, ACS_HLINE);

  // inner
  for (i = 0; i < this->userlist.size(); i++) {
  	items[i + 1] = subwin(items[0], 1, 17, 6 + i, start_col + 1);
    wprintw(items[i + 1], "%s", this->userlist[i].c_str());
  }

  // select
  int selected = 0, count = this->userlist.size();
  int key, ret = -2;
  while (1) {
    wbkgd(items[selected + 1], COLOR_PAIR(2));
    wnoutrefresh(items[selected + 1]); // color on
    wrefresh(items[0]);
    doupdate();
    key = getch();
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
  for (i = 0; i < count; i++) {
    delwin(items[i + 1]);
  }
  delwin(items[0]);
  delete items;

  touchwin(stdscr);
	refresh();

  return ret;
}

void UI::chat_room(int idx) {
  noecho();
  WINDOW ** chat_window;

  std::string name = this->userlist[idx];

  int start_col = this->max_width / 2 - 20;

  chat_window = new WINDOW * [4];

  // outter box
  chat_window[0] = newwin(30, 45, 2, start_col);
	wbkgd(chat_window[0], COLOR_PAIR(3));
	box(chat_window[0], ACS_VLINE, ACS_HLINE);

  // against name
  chat_window[1] = subwin(chat_window[0], 1, 40, 3, start_col + 2);
	wbkgd(chat_window[1], COLOR_PAIR(3));
	mvwprintw(chat_window[1], 0, 20 - name.size() / 2, "%s", name.c_str());
  this->opened_current = name;

  // chatting log
  chat_window[2] = subwin(chat_window[0], 20, 40, 4, start_col + 2);
	wbkgd(chat_window[2], COLOR_PAIR(4));
  this->current = chat_window[2];

  // chatting textbox
  chat_window[3] = subwin(chat_window[0], 5, 40, 25, start_col + 2);
	wbkgd(chat_window[3], COLOR_PAIR(4));
	wrefresh(chat_window[0]);

  this->refresh_chatlog(name);

  std::string message;
  int message_idx = 0;
  noecho();
  while (true) {
    wclear(chat_window[3]);
    wprintw(chat_window[3],  ">> ");
    wprintw(chat_window[3], message.c_str());
    wnoutrefresh(chat_window[3]);
    doupdate();

    int key = getch();
    if (key == KEY_ESCAPE) {
      if (message.size() == 0)
        break;
      else {
        message.clear();
        continue;
      }
    } else if (key == KEY_F(2) && message.size() == 0) {
      wclear(chat_window[3]);
      wprintw(chat_window[3], "FILE? ");
      message.assign(81, '\0');
      echo();
      wgetnstr(chat_window[3], &message[0], 80);
      noecho();
      message = message.substr(0, message.find('\0'));
      //this->msgr->send_file(name, message);
      this->push_message(name, this->msgr->get_name() + " send '" + message + "'");
      message.clear();
      continue;
    } else if (key == KEY_BACKSPACE || key == 127) {
      if (message.size() > 0) {
        message = message.substr(0, message.length() - 1);
      }
      continue;
    } else if( key == KEY_ENTER2 ) {
      if (message.size() == 0) continue;
      this->msgr->send_message(name, message);
      this->push_message(name, this->msgr->get_name() + ": " + message);
      message.clear();
      continue;
    }
    if (message_idx < 200) {
      message += ((char) key & 0xFF);
    }
  }
  noecho();

  this->current = NULL;
  this->opened_current.clear();

  for (int i = 3; i >= 0; --i) {
    delwin(chat_window[i]);
  }
  delete chat_window;

  touchwin(stdscr);
	refresh();
}

void UI::refresh_chatlog(std::string name) {
  if (this->current == NULL || this->opened_current.compare(name) != 0) {
    return;
  }

  std::vector<std::string> current = this->get_chat()[name];

  if (current.size() == 0) {
    return;
  }
  //DumpHex(current[0]); getchar();
  int offset = 0;
  for (auto it = current.begin(); it != current.end(); ++it) {
    mvwprintw(this->current, offset, 0, "%s", (*it).c_str());
    offset += (*it).size() / 40 + 1;
  }
  wnoutrefresh(this->current);
  doupdate();
}

void UI::push_message(std::string name, std::string content) {
  std::map<std::string, std::vector<std::string>> &chats = this->get_chat();
  if (chats.find(name) == chats.end()) {
    std::vector<std::string> empty;
    chats.insert(std::pair<std::string, std::vector<std::string>>(name, empty));
  }
  chats[name].push_back(content);
  this->refresh_chatlog(name);
}

void UI::update_userlist() {
  std::map<std::string, std::string> &userlist = this->msgr->get_node_list();
  while (userlist.empty()) {
    sleep(1);
  }
  this->userlist.clear();
  for (auto it = userlist.begin(); it != userlist.end(); ++it) {
    if (this->msgr->get_name().compare((* it).first) == 0) continue;
    this->userlist.push_back((* it).first);
  }
}

std::map<std::string, std::vector<std::string>>& UI::get_chat() {
  return this->chat;
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
    update_userlist();
    if (this->userlist.empty()) {
      continue;
    }
    if ((idx = this->select_user()) < 0) {
      break;
    }
    this->chat_room(idx);
  }

}

/*
int main(int argc, char *argv[]s) {
  Messanger m;
  UI cui(&m);
  std::map<std::string, std::string>&test = m.get_node_list();
  test.insert(std::pair<std::string, std::string>("TestUser1", "172.17.0.3"));
  test.insert(std::pair<std::string, std::string>("TestUser2", "172.17.0.4"));
  test.insert(std::pair<std::string, std::string>("TestUser3", "172.17.0.5"));
  test.insert(std::pair<std::string, std::string>("TestUser4", "172.17.0.6"));
  test.insert(std::pair<std::string, std::string>("TestUser5", "172.17.0.7"));
  std::thread interface_th(&UI::interface_thread, cui);
  interface_th.join();

  return 0;
}
*/
