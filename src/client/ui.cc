#include "ui.h"

#include "messanger.h"
#include <iostream>
#include <ncurses.h>
#include <map>
#include <thread>
#include <string>

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
	init_pair(3, COLOR_BLACK, COLOR_YELLOW);
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

  return key == 'y';
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

  GPG &g = this->msgr.getGPG();
  return g.verify_passphrase(githubid, password);
}

void UI::interface_thread() {
  this->init_scr();

  if (!this->login_screen()) {
    // login fail
    return;
  }

  while (true) {
    if (this->checkbox_quit()) {
      break;
    }
  }

}

int main(int argc, char *argv[]) {
  Messanger m;
  UI cui(m);

  std::thread interface_th(&UI::interface_thread, cui);
  interface_th.join();

  return 0;
}
