#include "ui.hpp"

//Test

UI::UI(){
	this->clear();
}

void UI::clear() {
	
	this->USER = new char[80];
	this->PASSWORD = new char[80];

	this->userlist.clear();
	this->hist.clear();
}

void UI::get_USER(char * id){
	this->USER = id;
}

void UI::get_PASSWORD(char * password){
	this->PASSWORD = password;
}

//userlist
/*
void UI::set_userlist(){
	

}
*/

void UI::set_userlist(){
	this->userlist.push_back("TestUser1");
	this->userlist.push_back("TestUser2");
	this->userlist.push_back("TestUser3");
	this->userlist.push_back("TestUser4");
}

void UI::set_hist(std::vector<std::string> userlist) {
	for(int i = 0; i < userlist.size(); i++){
		std::vector<std::string> history;
		this->hist[userlist[i]] = history;
	}
}

void UI::store_hist(std::string message, std::string receiver){
	this->hist[receiver].push_back(message);
}

void UI::show_hist(std::vector<std::string> * history, WINDOW * chat) {
	int offset = 0;
	std::string msg;

	if(hist.size() != 0){
		for(int i = 0; i < history->size(); i++) {
			msg = *(history->begin() + i);
			mvwprintw(chat, offset, 0, "%s", msg.c_str());
			offset += msg.size() / 40 + 1;
		}	
		wnoutrefresh(chat);
		doupdate();
	}
}

void UI::init_scr() {
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

int UI::check_quit(int x)		//Todo : int y
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

WINDOW ** UI::make_list(int start_col, int len)
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

int UI::scroll_menu(WINDOW **items, int count, int menu_start_col, int index_num, int x)  //Todo : int y
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
//Todo : int y
int UI::chat_room(int x, int selected_item) {

	int key;
	char * message;
	std::string msg_total;

	message = new char[200];
	WINDOW ** chat;
	chat = chat_form(selected_item, x/2 - 20);

	while(1){
		memset(message, 0, 200);
		msg_total = this->USER;
		msg_total += ": ";
		show_hist(&this->hist[this->userlist[selected_item]], chat[2]);

		mvwprintw(chat[3], 0, 0, ">>");
		noecho();
		for(int i = 0; i < 199; i ++){
			keypad(chat[3], TRUE);
			key = wgetch(chat[3]);
			if ( key == ESCAPE) {
				if(i == 0) {
					//delete message;
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
		store_hist(msg_total, this->userlist[selected_item]);
		memset(message, 0, 200);
	}
	delete message;
}

void UI::messenger_UI () {
	set_userlist();
	set_hist(this->userlist);
	int x, y, key, selected_item;					/*center position*/
	char * password, * id;
	unsigned int list_size = this->userlist.size() - 1;

	password = new char[80];
	id = new char[80];

	//Todo : socket open
	//getlist

	init_scr();
	getmaxyx(stdscr, y, x);

	WINDOW * passwindow = newwin(6, 80, y/2 - 3, x/2 - 40);
	refresh();
	box(passwindow, 0, 0);
	wbkgd(passwindow, COLOR_PAIR(3));
	echo();
	mvwprintw(passwindow, 1, 5, "Enter the ID: ");
	wmove(passwindow, 1, 20);
	wgetnstr(passwindow, id, 80);
	get_USER(id);
	noecho();
	mvwprintw(passwindow, 3, 5, "Passphrase: ");
	wmove(passwindow, 3, 18);
	wgetnstr(passwindow, password, 80);
	get_PASSWORD(password);

	delwin(passwindow);
	touchwin(stdscr);
	refresh();

	while(1){
		noecho();
		WINDOW ** user_list;
		user_list = make_list(x/2 - 20, list_size + 1);
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
	delete id;
	delete password;
}

// void UI::dist_to_hist(std::vector<std::string> vec, std::map<std::string, std::vector<std::string> > & hist){
// 	unsigned int found;
// 	for(std::vector<std::string>::iterator it = vec.begin() ; it != vec.end(); it++){
// 		found = it->find(":");
// 		std::string &msg = *it;
// 		std::string name = it->substr(0, found -1);
// 		mtx.lock();
// 		for(std::map<std::string, std::vector<std::string> >::iterator at = hist.begin() ; at != hist.end(); at++){
// 			if((at->first).compare(name) == 0){
// 				(at->second).push_back(msg);
// 			}
// 		}
// 		mtx.unlock();
// 	}
// }

// void UI::recv_UI(std::map<std::string, std::vector<std::string> > & hist, bool & reff) {
// 	std::vector<std::string> newmsg;

// //socket read

// 	std::vector<std::string> newmsg;
// 	std::string read_soc;
// 	newmsg = receiver(read_soc);
// 	mtx.lock();
// 	if(reff == false){
// //Distribute newmsg to hist;
// 		dist_to_hist(newmsg, hist);
// 		reff = true;
// 	}
// 	mtx.unlock();

// }

UI::~UI()
{
	delete this->USER;
	delete this->PASSWORD;
	this->userlist.clear();
	this->hist.clear();
}