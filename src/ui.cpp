//#include "ui.h"
#include <iostream>
#include <ncurses.h>
#include <cstdlib>
#include <vector>
#include <map>
#include <string>

#define ESCAPE 27
#define ENTER 10

//Test
#define USER "TestUser5"

std::vector<std::string> userlist;

void init_testset() {
	userlist.push_back("");
	userlist.push_back("TestUser1");
	userlist.push_back("TestUser2");
	userlist.push_back("TestUser3");
	userlist.push_back("TestUser4");
}


void init_scr() {

	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_BLACK, COLOR_YELLOW);
	init_pair(4, COLOR_BLACK, COLOR_WHITE); 
	curs_set(2);
	noecho();
	keypad(stdscr, TRUE);
}

int check_quit(int x)
{
    WINDOW * check;
    int key;
    check = newwin(3, 40, 3, x/2 - 20);
    wmove(check, 1, 2);
    wprintw(check, "Exit program (y/n) ? "); 
    wbkgd(check, COLOR_PAIR(2));
    box(check, 0, 0);
    refresh();
    key = wgetch(check);
    delwin(check);
    if (key == 'y')
        return -1;
    else 
        return 0;
}

WINDOW ** test_list(int start_col)
{
    int i;
    WINDOW **items;

    items = new WINDOW * [5];
    items[0]=newwin(20,40,5,start_col);
    wbkgd(items[0], COLOR_PAIR(3));
    box(items[0],ACS_VLINE, ACS_HLINE);

    items[1]=subwin(items[0], 1, 17, 6, start_col+1);
    items[2]=subwin(items[0], 1, 17, 7, start_col+1);
    items[3]=subwin(items[0], 1, 17, 8, start_col+1);
    items[4]=subwin(items[0], 1, 17, 9, start_col+1);
    for (i =1; i < 5; i++)
        wprintw(items[i], "%s", userlist[i].c_str());
    wbkgd(items[1], COLOR_PAIR(2));
    wrefresh(items[0]);
    return items;
}

int scroll_menu(WINDOW **items, int count, int menu_start_col, int index_num, int x)
{
    int key;
    int selected = 0;
    while (1) 
    {
        key = getch();
        if (key == KEY_DOWN || key == KEY_UP) 
        {
            wbkgd(items[selected + 1], COLOR_PAIR(3));
            wnoutrefresh(items[selected + 1]);
            if (key == KEY_DOWN) 
            {
                selected = (selected + 1) % count;
            } 
            else 
            {
                selected = (selected + count - 1) % count;
            }
            wbkgd(items[selected + 1],COLOR_PAIR(2));
            wnoutrefresh(items[selected + 1]);
            doupdate();
        } 
        else if (key == ESCAPE) 
        {
 	       return check_quit(x);
        } 
        else if (key == ENTER) 
        {
            return selected + 1;
        }
    }
}

WINDOW ** chat_form(int selected_item, int start_col){
	
	int key;
	WINDOW ** chat_items;
	unsigned int len = userlist[selected_item].size() / 2;

	chat_items = new WINDOW * [4];
	chat_items[0] = newwin(30, 45, 2, start_col);
	wbkgd(chat_items[0], COLOR_PAIR(3));
	box(chat_items[0], ACS_VLINE, ACS_HLINE);

	chat_items[1] = subwin(chat_items[0], 1, 40, 3, start_col + 2);
	wbkgd(chat_items[1], COLOR_PAIR(3));
	mvwprintw(chat_items[1], 0, 20 - len, "%s", userlist[selected_item].c_str());
	chat_items[2] = subwin(chat_items[0], 20, 40, 4, start_col + 2);
	wbkgd(chat_items[2], COLOR_PAIR(4));
	chat_items[3] = subwin(chat_items[0], 5, 40, 25, start_col + 2);
	wbkgd(chat_items[3], COLOR_PAIR(4));
	wrefresh(chat_items[0]);
	return chat_items;

}

int chat_room(int x, int selected_item) {

	int key;
	WINDOW ** chat;
	chat = chat_form(selected_item, x/2 - 20);

	while(1){
		wmove(chat[3], 0, 0);
		wprintw(chat[3], ">>");
		wnoutrefresh(chat[3]);
		doupdate();
		key = getch();
		if(key == ESCAPE)
		{
			delete chat;
			return 1;
		}
	}
}

int main() {

	std::string PASSPHRASE = "abcd";				/*Test passphrase*/
    init_testset();

	int key;
	int x, y;										/*center position*/
	//std::string password;
	char *password;
	int selected_item;								/*check*/
	unsigned int list_size = userlist.size() - 1;

	password = new char[100];
	init_scr();
	bkgd(COLOR_PAIR(1));
	getmaxyx(stdscr, y, x);

	WINDOW * subwindow = newwin(4, 80, y/2 - 2, x/2 - 40);
	refresh();
	box(subwindow, 0, 0);
	wbkgd(subwindow, COLOR_PAIR(3));
	mvwprintw(subwindow, 1, 30, "Enter the Passphrase");
	wmove(subwindow, 2, 1);
	wgetnstr(subwindow, password, 4);
	if(password != PASSPHRASE) {
		delwin(subwindow);
		endwin();
		exit(0);
	}

	delwin(subwindow);
	touchwin(stdscr);
	refresh();
	

	while(1){
		WINDOW ** user_list;
		user_list = test_list(x/2 - 20);
		selected_item = scroll_menu(user_list, list_size, 0, 1, x); //userlist.size() - 1

		if(selected_item == -1)
			break;
		if(selected_item == 0) {
			touchwin(stdscr);
			refresh();
			continue;
		}

		if(chat_room(x, selected_item) == 1) { 
			touchwin(stdscr);
        	refresh();
			continue;
		}
		else 
	    	touchwin(stdscr);
        	refresh(); 
			continue;

        touchwin(stdscr);
        refresh(); 
        delete user_list;
	}

	endwin();
	delete password;

	return 0;

}