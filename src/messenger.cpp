#include "message.h"
#include "messenger.h"
//#include "pgp_crypto.h"
#include <iostream>
#include <cstring>

Messenger::Messenger() 
{}

void Messenger::Show_Info() {

	std::cout << "------------Oninon Messenger------------" << std::endl << std::endl;
	std::cout << "1. Send message" << std::endl;
	std::cout << "2. Message history" << std::endl;
	std::cout << "3. Exit" << std::endl << std::endl;
}

//Initial message packet
void Messenger::Init_packet() {

	std::string to, MSG;
        std::string from = "moncadeau92@kaist.ac.kr";   //client ID address
	int check_id = 1;			   //check receiver address

        std::cout << "Write the ID of receiver: ";      //finall receiver ID address
        std::getline(std::cin, to);
//	int check_IP = bool();  		   //check if the reciever ID is valid

//
	if(check_id == 1) {
		std::cout << "Write the messsages: ";   //Messages
		std::getline(std::cin, MSG);
		std::cout << std::endl;
	        Message * message = new Message(from, to, MSG);
		message_enc.push_back(message);
		message_history.push_back(message);
	}
	else {
		std::cout << "Wrong ID..." << std::endl;
	}
}	

//Packet wrapping
void Messenger::Make_packet() { //string from, string to) {

	std::string from, to; //delete later
	Message * message_prev = message_enc.back();
	from = "moncadeau92@kaist.ac.kr";            //IP address of Sender
	to = "skc01012001@naver.com";		   //IP address of receiver

	std::string MSG = message_prev->Make_array();  //MSG should be encrypted

//	enc_check = pgp_enc(MGS.c_str(), MSG.size(), to.c_str(), &MSG_enc);
		
//	std::cout << MSG << std::endl;

	Message * message = new Message(from, to, MSG);
	message_enc.push_back(message);
}

int Messenger::Recv_packet(){  //std::string dec_msg) {

	Message * message_dec = message_enc.back();
	std::string MSG = message_dec->Make_array();
	
	std::string SIG_HEAD = "\xde\xad";
	std::string SIG_TAIL = "\xf0\x0d";
	std::string SIG_HEAD_dec = MSG.substr(0, 2);

	if(SIG_HEAD.compare(SIG_HEAD_dec) != 0){
		
		std::cout << "Format Error!" << std::endl;
		return -1;		
	}
	const char *ptr = MSG.c_str();
	int from_len = *(ptr + 2);
	std::string from = MSG.substr(3, from_len);
	int to_len = *(ptr + 3 + from_len);
	std::string to = MSG.substr(4 + from_len, to_len);
	std::string SIG_TAIL_dec = MSG.substr(4 + from_len + to_len, 2);
	if(SIG_TAIL.compare(SIG_TAIL_dec) != 0) {
		std::cout << "Format Error!" << std::endl;
		return -1;
	}

	std::string msg = MSG.substr(6 + from_len + to_len);

//check whether from ID is mine
/*	if(from.compare() != 0) {
	
		//send packet

		return 0;
	}
*/	


//check if to ID is mine
/*	if(to.compare() == 0) {

		//decode packet
		

		Message * message = new Message(from, to, msg);	

		
		message_history.push_back*(message);
		return 0;
	}
*/

//	std::cout << msg << std::endl;
			
	return 0;	
}

//Delete Message classes
void Messenger::Del_enc_packet() {

	message_enc.clear();
	
}

void Messenger::Del_history_packet() {

	{
		Message* message_del = message_history.back();
		delete message_del;
		message_history.pop_back();
	}
}

void Messenger::Packet_History() {

	std::string who;
	int i = 0;

	std::cout << "----------Histroy----------" << std::endl << std::endl;

	std::cout << "Type the ID : ";
	std::getline(std::cin, who);
	std::cout << std::endl;
	
	for(std::vector<Message*>::iterator Msg = message_history.begin() ; Msg != message_history.end(); Msg++) {
		
		if(who.compare((*Msg)->from_ID) == 0 || who.compare((*Msg)->to_ID) == 0) {
			i++;
			std::cout << i << "'th message" << std::endl;	
			(*Msg)->Show_msg();
			continue;
		}
	}
	if(i == 0) {
		std::cout << "There was no message from " << who << std::endl << std::endl;
	}

	std::cout << "------------End------------" << std::endl << std::endl;
}



int Messenger::Main() {
	int choice;

	while(1) {
		Show_Info();
		std::cout << "Choice : ";
		std::cin >> choice;
		std::cin.ignore();
		std::cout << std::endl;


		switch(choice){
		case 1:	
			Init_packet();
			

			Del_enc_packet();
			break;

		case 2:
			Packet_History();
			break;
		case 3:
			Del_history_packet();
			return 0;

		default :
			std::cout << "Wrong Choice!" << std::endl;
			break;
		}
		

	}
	return 0;		
}

