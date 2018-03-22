#include "message.h"
#include "messenger.h"
#include <iostream>

using namespace std;

Messenger::Messenger()
{}

void Messenger::Init_packet() {

	string from, MSG;
    string from = "moncadeau92@kaist.ac.kr";
//  int check_IP;

	cout << "Write the ID of receiver: ";
    getline(cin, to);

//	if(check_IP == 1) {
        cout << "Write the messsages: ";
        getline(cin, MSG);

        Message * message = new Message(from, to, MSG);
    	message_total.push_back(message);
	}
	else
		cout << "Wrong ID..." << endl;

}	

void Messenger::Make_packet() {

	string from, to;
	Message * message_prev = message_total.back();

	from = "skc01012001@naver.com";
	to = "moncadeau92@gmail.com";

	string MSG = message_prev->Make_string();

	cout << MSG << endl;

	Message * message = new Message(from, to, MSG);
	
	message_total.push_back(message);
}

void Messenger::Recv_packet() {

}


void Messenger::Del_packet() {
	
	while(!message_total.empty())
	{
		Message * message_del = message_total.back();
		delete message_del;
		message_total.pop_back();
	}
}
