#include "ui.hpp"
//Test
#include <iostream>
#include <ncurses.h>
#include <map>
#include <unistd.h>
#include <string>
#include <vector>
#include <cstdlib>

WINDOW ** UI::chat_form(int selected_item, int start_col){

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

int UI::chat_room(int x, int selected_item, std::map<std::string, std::vector<std::string> > & hist, bool & reff) {

	std::vector<std::string> * history = & hist[userlist[selected_item]];
	int key, offset;
	int sel - 0;
	char * message;
	std::string msg_total, show_msg, msg_send;

	message = new char[200];
	WINDOW ** chat;
	chat = chat_form(selected_item, x/2 - 20);

	while(1){
		memset(message, 0, 200);
		msg_total = USER + ": ";
		mtx.lock();
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
		mtx.unlock();

		mvwprintw(chat[3], 0, 0, ">>");
		echo();
		for(int i = 0; i < 199; i ++){
			keypad(chat[3], TRUE);
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
					i = -1;
					continue;
				}
				else {
					i--;
					mvwprintw(chat[3], (i + 2)/40, (i + 2) - 40 * ((i + 2)/40), " ");
					message[i] = 0;
					delch();
					wmove(chat[3], (i + 2)/40, (i+2) - 40 * ((i + 2)/40));
					wnoutrefresh(chat[3]);
					doupdate();
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
				echo();
				sel = 1;
				mvwprintw(chat[3], 0, 0, "file?: ");
				wgetnstr(chat[3], message, 199);
				for(int z = 0; z < 199; z++){
					if(message[z] == 0){
						i = z;
						break;
					}
				}
				message[i + 1] = '\0';
				noecho();
				continue;

			}

			else {
				message[i] = key;
				wprintw(chat[3], &message[i]);
				continue;
			}
		}
		std::string msg(message);
		msg_total += message;
//Store in history
		mtx.lock();
		hist[userlist[selected_item]].push_back(msg_total);
		mtx.unlock();
		//sockect send
		msg_send = pack_total(userlist[selected_item], msg_total, sel);
		write(fd, msg_send.c_str(), msg_send.size());
		sel = 0;
		memset(message, 0, 200);

	}
	delete message;
}

void UI::messanger_UI (std::map<std::string, std::vector<std::string> > & hist, bool & reff) {

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

		if(chat_room(x, selected_item, hist, reff) == 1) {
			touchwin(stdscr);
      refresh();
			continue;
		}
	}

	endwin();

}

std::vector<std::string> UI::receiver(std::string recv) {

	std::vector<std::string> msg_tot;
	std::string str1, str2;
	unsigned char check, len1, len2;

	while(!recv.empty()){
		check = recv[0];
		len1 = recv[1];
		str1 = recv.substr(2, len1);
		len2 = recv[2+len];
		str2 = recv.substr(len1 + 3, len2);
		if(check == '0'){

			msg_tot.push_back(str2);

		}
		else if(check == '1'){
			str2.replace(str.find(":") + 2, str.end() - str.find(":") - 2, "sent file!");
			msg_tot.push_back(str2);
		}
		else if(check == '3'){
			continue
		}
		else if(check == '4'){
			//list

		}
		recv.erase(0, 3 + len1 + len2);
	}

	msg_tot.push_back(msg);
	return msg_tot;
}

void UI::dist_to_hist(std::vector<std::string> vec, std::map<std::string, std::vector<std::string> > & hist){
	unsigned int found;
	for(std::vector<std::string>::iterator it = vec.begin() ; it != vec.end(); it++){
		found = it->find(":");
		std::string &msg = *it;
		std::string name = it->substr(0, found -1);
		mtx.lock();
		for(std::map<std::string, std::vector<std::string> >::iterator at = hist.begin() ; at != hist.end(); at++){
			if((at->first).compare(name) == 0){
				(at->second).push_back(msg);
			}
		}
		mtx.unlock();
	}
}

void UI::recv_UI(std::map<std::string, std::vector<std::string> > & hist, bool & reff) {
	std::vector<std::string> newmsg;

//socket read

	std::vector<std::string> newmsg;
	std::string read_soc;
	newmsg = receiver(read_soc);
	mtx.lock();
	if(reff == false){
//Distribute newmsg to hist;
		dist_to_hist(newmsg, hist);
		reff = true;
	}
	mtx.unlock();

}

int main() {
	bool reff = false;
	std::string USER;
	std::vector<std::string> userlist;
	std::map<std::string, std::vector<std::string> > hist;

  UI ui;

	std::thread t1(ui.messanger_UI, std::ref(hist), std::ref(reff));
	std::thread t2(ui.recv_UI, std::ref(hist), std::ref(reff));

	t1.join();
	t2.join();

	return 0;
}

UI::~UI()
{
	delete password;
	delete message;
	delete gitid;
	userlist.clear();
	hist.clear();
}
