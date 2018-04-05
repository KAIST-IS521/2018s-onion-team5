#pragma once

#include <iostream>
#include <ncurses.h>
#include <map>
#include <unistd.h>
#include <string>
#include <vector>
#include <cstdlib>

#define ESCAPE 27
#define ENTER 10

class UI
{
private :
	char * USER, * PASSWORD;									//Gets when login

	std::vector<std::string> userlist;							//Gets from the server 
	std::map<std::string, std::vector<std::string>> hist;		//History Container

public :
	UI();

	void clear();
	void get_USER(char * id);
	void get_PASSWORD(char * password);

	void set_userlist();
	void set_hist(std::vector<std::string> userlist);
	void store_hist(std::string message, std::string receiver);
	void show_hist(std::vector < std::string> * hist, WINDOW * chat);

	void init_scr();

	int check_quit(int x);
	WINDOW ** make_list(int start_col, int len);
	int scroll_menu(WINDOW **items, int count, int menu_start_col, int index_num, int x);
	WINDOW ** chat_form(int selected_item, int start_col);
	int chat_room(int x, int selected_item);
	void messenger_UI();
//
	// void dist_to_hist(std::vector<std::string> vec, std::map<std::string, std::vector<std::string> > & hist);
	// void recv_UI(std::map<std::string, std::vector<std::string> > & hist, bool & reff);

	~UI();
};
