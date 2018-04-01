#include <ncurses.h>
#include <map>
#include <unistd.h>
#include <thread>
#include "ui_receiver.hpp"
#include <cstring>

#define ESCAPE 27
#define ENTER 10

//Test
//std::string ssstr = 


std::string USER;
std::vector<std::string> userlist;
std::vector<std::string> newmsg;
std::map<std::string, std::vector<std::string> > hist;

//this can be deleted
void init_testset() {

	userlist.push_back("");
	userlist.push_back("TestUser1");
	userlist.push_back("TestUser2");
	userlist.push_back("TestUser3");
	userlist.push_back("TestUser4");
	userlist.push_back("hojoon");
	userlist.push_back("Ahnmo");
}

void hist_map()
{
	//init_testset can be deleted
	init_testset();
	for(int i = 0; i < userlist.size(); i++)
	{
		std::vector<std::string> history;
		hist[userlist[i]] = history;
	}

}

void init_scr() {

	initscr();
	start_color();
	init_pair(1, COLOR_WHITE, COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLUE);
	init_pair(3, COLOR_BLACK, COLOR_YELLOW);
	init_pair(4, COLOR_BLACK, COLOR_WHITE); 
	curs_set(2);
	bkgd(COLOR_PAIR(1));
	keypad(stdscr, TRUE);
}

int check_quit(int x)
{
    WINDOW * check;
    int key;
    check = newwin(3, 40, 3, x/2 - 20);
    wmove(check, 1, 2);
    wprintw(check, "Exit program (y/N) ? "); 
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


WINDOW ** test_list(int start_col, int len)
{
    int i;
    WINDOW **items;

    items = new WINDOW * [len];
    items[0]=newwin(20, 40, 5, start_col);
    wbkgd(items[0], COLOR_PAIR(3));
    box(items[0], ACS_VLINE, ACS_HLINE);
    for (i =1; i < len; i++){
    	items[i] = subwin(items[0], 1, 17, 5 + i, start_col + 1);
        wprintw(items[i], "%s", userlist[i].c_str());
    }
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

std::string pack_total(std::string str1, std::string str2, int sel){
//0: message 1: message 3: id,passphrase 4: list
	std::string packet;
	if(sel == 0){
		packet += '0';
	}
	else if(sel == 1)
		packet += '1';
	else if(sel == 3)
		packet += '3';
	else if(sel == 4)
		packet += '4';
	packet += str1.size();
	packet += str1;
	packet += str2.size();
	packet += str2;

	return packet;
}

int chat_room(int x, int selected_item) {

	std::vector<std::string> * history = & hist[userlist[selected_item]];
	int key, offset;
	int sel = 0;
	char * message;
	std::string msg_total, show_msg, msg_send;

	
	WINDOW ** chat;
	chat = chat_form(selected_item, x/2 - 20);
	message = new char[200];
	while(1){

		memset(message, 0, 200);
		msg_total = USER + ": ";
		if(history->size() != 0) {
			offset = 0;
			for(int i = 0; i < history->size(); i++) {
				show_msg = *(history->begin() + i);
				mvwprintw(chat[2], offset, 0, "%s", show_msg.c_str());
				offset += show_msg.size() / 40 + 1;
			}	
			wnoutrefresh(chat[2]);
			doupdate();
		}

		mvwprintw(chat[3], 0, 0, ">>");
		wmove(chat[3], 0, 2);
		noecho();
		keypad(chat[3], TRUE);
		for(int i = 0; i < 199; i ++){
			message[i] = {0,};
			key = wgetch(chat[3]);
			if ( key == ESCAPE) {
				if(i == 0) {
					delete chat;
					return 1;
				}
				else {
					memset(message, 0, 200);
					werase(chat[3]);
					mvwprintw(chat[3], 0, 0, ">>");
					wmove(chat[3], 0, 2);
					wnoutrefresh(chat[2]);
					wnoutrefresh(chat[3]);
					doupdate();
					i = -1;
					continue;
				}
			}
			else if( key == ENTER ) {
				if(i == 0) {
					i = -1;
					continue;
				}
				werase(chat[3]);
				wnoutrefresh(chat[2]);
				wnoutrefresh(chat[3]);
				doupdate();
				message[i] = '\0';
				break;
			}

			else if( key == KEY_BACKSPACE || key == 127)
			{
				if(i == 0){
					message[i] = 0;
					continue;
				}
				else {
					i--;
					delch();
					if(i != 0){
						i--;
						continue;
					}
					else if(i == 0){
						memset(message, 0, 200);
						i = -1;
						continue;
					}
				}

			}
			else if( key == KEY_F(2) && i == 0)
			{
				sel = 1;
				mvwprintw(chat[3], 0, 0, "file?: ");
				wgetnstr(chat[3], message, 199);
				//i = strlen(message);
				continue;

			}

			else {
				message[i] = key;
				wprintw(chat[3], &message[i]);
				continue;
			}
		}
		std::string msg(message);
		msg_total += msg;
//Store in history
		hist[userlist[selected_item]].push_back(msg_total);

		//Todo : socket send
		msg_send = pack_total(userlist[selected_item], msg_total, sel);
	}
	delete message;
}

int main() {

    hist_map();
	int x, y, key, selected_item;					/*center position*/
    std::string packet;
	char * password, * gitid;
	unsigned int list_size = userlist.size() - 1;

	password = new char[100];
	gitid = new char[100];

	//To do : socket open
	//get list

	init_scr();
	getmaxyx(stdscr, y, x);

	WINDOW * passwindow = newwin(6, 80, y/2 - 3, x/2 - 40);
	refresh();
	box(passwindow, 0, 0);
	wbkgd(passwindow, COLOR_PAIR(3));
	echo();
	mvwprintw(passwindow, 1, 5, "Enter the ID: ");
	wmove(passwindow, 1, 20);
	wgetnstr(passwindow, gitid, 80);
	USER = gitid;
	noecho();
	mvwprintw(passwindow, 3, 5, "Passphrase: ");
	wmove(passwindow, 3, 18);
	wgetnstr(passwindow, password, 80);
	//socket send id, passphrase
	packet = pack_total(gitid, password, 3);

	delwin(passwindow);
	delete password;
	touchwin(stdscr);
	refresh();
	

	while(1){
		noecho();
		WINDOW ** user_list;
		user_list = test_list(x/2 - 20, userlist.size());
		selected_item = scroll_menu(user_list, list_size, 0, 1, x);

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
	delete gitid;
	userlist.clear();	
	hist.clear();
	return 0;
}

std::vector<std::string> receiver(std::string recv) {
	std::vector<std::string> msg_tot;
	std::string str1, str2;
	unsigned char check, len1, len2;

	while(!recv.empty()){
		check = recv[0];
		len1 = recv[1];
		str1 = recv.substr(2, len1);
		len2 = recv[2 + len1];
		str2 = recv.substr(len1 + 3, len2);
		if(check == '0'){
			
			msg_tot.push_back(str2);

		}
		else if(check == '1'){
//			str2.replace(recv.find(":") + 2, recv.end() - recv.find(":") - 2, "sent file!");
			msg_tot.push_back(str2);
		}
		else if(check == '4'){
			//list
			
		}
		recv.erase(0, 3 + len1 + len2);
	}

	msg_tot.push_back(str2);
	return msg_tot;
}

void dist_to_hist(std::vector<std::string> vec){
	unsigned int found;
	for(std::vector<std::string>::iterator it = vec.begin() ; it != vec.end(); it++){
		found = it->find(":");
		std::string &msg = *it;
		std::string name = it->substr(0, found -1);
		for(std::map<std::string, std::vector<std::string> >::iterator at = hist.begin() ; at != hist.end(); at++){
			if((at->first).compare(name) == 0){
				(at->second).push_back(msg);
			}
		}
	}
}


int recv(bool &running) {

	std::vector<std::string> newmsg;

//socket read

	std::string read_soc;

	newmsg = receiver(read_soc);
//Distribute newmsg to hist;
	dist_to_hist(newmsg);

	return 0;
}

/*int main() {

	std::thread t1(messenger_UI);
	std::thread t2(recv);





	t1.join();
	t2.join();

	return 0;
}*/

