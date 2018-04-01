#pragma once
#include <iostream>
#include <ncurses.h>
#include <map>
#include <unistd.h>
#include <thread>
#include <string>
#include <vector>
#include <cstdlib>

#define ESCAPE 27
#define ENTER 10


class UI
{
public :
	int fd;
	std::string USER;
	std::vector<std::string> userlist;
	std::map<std::string, std::vector<std::string>> hist;

public :
	UI();
	void init_testset();
	void hist_map(std::map<std::string, std::vector<std::string> > & hist);
	void init_scr();
	int check_quit(int x);
	WINDOW ** test_list(int start_col, int len);
	int scroll_menu(WINDOW **items, int count, int menu_start_col, int index_num, int x);
	WINDOW ** chat_form(int selected_item, int start_col);
	std::string pack_total(std::string str1, std::string str2, int sel);
	int chat_room(int x, int selected_item, std::map<std::string, std::vector<std::string> > & hist, bool & reff);
	void messenger_UI(std::map<std::string, std::vector<std::string> > & hist, bool & reff);
	std::vector<std::string> receiver(std::string recv);
	void dist_to_hist(std::vector<std::string> vec, std::map<std::string, std::vector<std::string> > & hist);
	void recv_UI(std::map<std::string, std::vector<std::string> > & hist, bool & reff);
	int main();
	~UI();
};
