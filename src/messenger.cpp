#include "message.h"
#include "messenger.h"
//#include "pgp_crypto.h"
#include <iostream>
#include <fstream>
//#include <cstring>
//Test
#define FROM "moncadeau92@kasit.ac.kr"
#define TO "moncadeau92@gmail.com"
#define PATH "~/IS521/Test/Enc.txt"

Messenger::Messenger() 
{}

std::string Messenger::Timestamp() {

	time_t ltime;
	ltime = time(NULL);
	std::string Time = asctime(localtime(&ltime));

	return Time;

}


void Messenger::Show_Info() {

	std::cout << "------------Oninon Messenger------------" << std::endl << std::endl;
	std::cout << "1. Send message" << std::endl;
	std::cout << "2. Chatting Room" << std::endl;
	std::cout << "3. Exit" << std::endl ;//<< std::endl;
//
	std::cout << "4. Receive test" << std::endl << std::endl;
}

//Initial message packet
void Messenger::Init_packet() {

	std::string to, MSG, file_path;
	std::string timestamp = Timestamp();
    std::string from = FROM;   //client ID address
//	int check_id = 1;			   //check receiver address

    std::cout << "Write the ID of receiver: ";      //final receiver ID address
    std::getline(std::cin, to);
//	int check_IP = bool();  		   //check if the reciever ID is valid

//
//	if(check_id == 1) {
	
		std::cout << "Write the messsages: ";   //Messages
		std::getline(std::cin, MSG);
		std::cout << std::endl;
      	Message * message = new Message(from, to, MSG, timestamp);
		message_enc.push_back(message);
		message_history.push_back(message);
//	}
//	else {
//		std::cout << "Wrong ID..." << std::endl;
//	}
}	

//Packet wrapping
void Messenger::Make_packet() { //string from, string to) {

	std::string timestamp = Timestamp();
	std::string from, to; 		//delete later
	Message * message_prev = message_enc.back();
	from = FROM;            	//ID(address) of Sender
	//to = "skc01012001@naver.com";	//IP(address) of receiver
	char to1[] = "skc01012001@naver.com";
	std::string MSG2 = message_prev->Make_array();  //MSG should be encrypted

	std::vector<char> ch(MSG2.begin(), MSG2.end());
	char * MSG = &ch[0];

	//make file for encrypt
	std::ofstream enc_prev_file;
	enc_prev_file.open("./Enc.pub");
	enc_prev_file << MSG;
	enc_prev_file.close();
	std::cout << to << std::endl;
//test
	Message * message = new Message(from, to, MSG, timestamp);
	message_enc.push_back(message);


//	int enc_check = pgp_enc("Enc.pub", to1, "Enc.pub");
//	std::string MSG_enc;
/*	if(enc_check == 0) {
		std::ifstream enc_file;
		enc_file.open("./Enc.pub");
		std::string MSG_enc;
		enc_file >> MSG_enc;
		std::cout << "Encypt success!" << std::endl;
		std::cout << MSG_enc << std::endl;
		enc_file.close();
		Message * message = new Message(from, to, MSG_enc, timestamp);
		message_enc.push_back(message);
	}
	else
		std::cout << "Encrpyt failed" << std::endl;
*/

}

int Messenger::Recv_packet(){  //std::string dec_msg) {
//Test
	Message * message_dec = message_history.back();
	std::string MSG = message_dec->Make_array();
//	
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
	int Time_len = *(ptr + 4 + from_len + to_len);
	std::string timestamp = MSG.substr(5 + from_len + to_len, Time_len);
	std::string SIG_TAIL_dec = MSG.substr(5 + from_len + to_len + Time_len, 2);
	if(SIG_TAIL.compare(SIG_TAIL_dec) != 0) {
		std::cout << "Format Error!" << std::endl;
		return -1;
	}
	std::string msg = MSG.substr(7 + from_len + to_len + Time_len);
//check whether from ID is mine
/*	if(from.compare(FROM) == 0) {
	
		//send packet

		return 0;
	}
*/	

//check if to ID is mine
//Test
	if(to.compare(TO) == 0) {

		//decode packet
		Message * message = new Message(from, to, msg, timestamp);
//		message_history.push_back(message);
		message->Show_msg();
		return 0;
	}


//	std::cout << msg << std::endl;
			
	return 0;	
}

//Delete Message classes
void Messenger::Del_enc_packet() {

	for(std::vector<Message*>::iterator Msg = message_enc.begin() + 1; Msg != message_enc.end(); Msg++) {
		delete *Msg;
	}
//	message_enc.erase(message_enc.begin() + 1, message_enc.end());
	message_enc.clear();
}

void Messenger::Del_history_packet() {

	while(!message_history.empty()) {
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
		}
	}
	if(i == 0) {
		std::cout << "There was no message from " << who << std::endl << std::endl;
	}

	std::cout << "------------End------------" << std::endl << std::endl;
}



int Messenger::Main() {
//	std::string check;
	int choice;
	int test;

	while(1) {
		Show_Info();
		std::cout << "Choice : ";
//		std::getline(std::cin, check);
		std::cin >> choice;
		std::cin.ignore();
		std::cout << std::endl;


		switch(choice){
		case 1:	
			Init_packet();
			
			Make_packet();
			Del_enc_packet();
			break;

		case 2:
			Packet_History();
			break;
		case 3:
			Del_history_packet();
			return 0;

///Receive test
		case 4:
			test = Recv_packet();
			if(test)
				break;
			else
				break;

		default :
			std::cout << "Wrong Choice!" << std::endl;
			break;
		}
		

	}
	return 0;		
}
	
