#include "message.h"
#include "messenger.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include "pgp_crypto.h"
//#include <cstring>
//Test
#define FROM "moncadeau92@naver.com"
#define TO "skc01012001@naver.com"
#define PATH "~/IS521/Test/Enc.txt"
#define PASSPHRASE "ks5202xw!@"

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

    std::cout << "Write the ID of receiver: ";      //final receiver ID address
    std::getline(std::cin, to);


	std::cout << "Write the messsages: ";   //Messages
	std::getline(std::cin, MSG);
	std::cout << std::endl;
	Message * message = new Message(from, to, MSG, timestamp);
	message_enc.push_back(message);
	message_history.push_back(message);
}	

//Packet wrapping
void Messenger::Make_packet() { //string from, string to) {

	std::string timestamp = Timestamp();
	std::string from, to; 		//delete later
	Message * message_prev = message_enc.back();
	from = FROM;            	//ID(address) of Sender
	to = TO;	//IP(address) of receiver
	std::string MSG2 = message_prev->Make_array();  //MSG should be encrypted

	std::vector<char> ch(MSG2.begin(), MSG2.end());
	//make file for encrypt
	std::ofstream enc_prev_file;
	enc_prev_file.open("./Enc.pub");
	enc_prev_file << MSG;
	enc_prev_file.close();
	std::string MSG_enc;
	ch.clear();	

	if(pgp_enc("Enc.pub", to.c_str(), "Dec.pub") == 0 ){
		exit(0);
		std::ifstream enc_file;
		enc_file.open("./Dec.pub");
		
		while(!enc_file.eof()) {
			std::getline(enc_file, MSG_enc);
		}
		enc_file.close();
		Message * message = new Message(from, to, MSG_enc, timestamp);
		message->Show_msg();
		message_enc.push_back(message);
		message_history.push_back(message);
		system("rm *.pub");
	}
	else
		std::cout << "Encrpyt failed" << std::endl;

}

int Messenger::Recv_packet(){  //std::string dec_msg) {
//Test
	Message * message_dec = message_history.back();
	std::string MSG = message_dec->Make_array();
//	
	std::string msg_dec = "";
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
	std::cout << msg << std::endl;
	std::cout << "1111" << std::endl;
	std::vector<char> ch(msg.begin(), msg.end());
	char * MSG_prev = &ch[0];

	std::cout << MSG_prev << std::endl;
	std::ofstream dec_prev_file;
    dec_prev_file.open("./Enc.pub");
//    dec_prev_file << MSG_prev;
	dec_prev_file.close();
    ch.clear();
	//decode packet
/*    if(pgp_dec("Enc.pub", PASSPHRASE, "Dec.pub") == 0) {
		std::ifstream dec_file;
		dec_file.open("./Dec.pub");
		dec_file >> msg_dec;
		dec_file.close();
		Message * message = new Message(from, to, msg_dec, timestamp);
//	message_history.push_back(message);
		message->Show_msg();
		system("rm *.pub");
	}
	else
		std::cout << "Decrpyt failed" << std::endl;
*/			
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
	char choice;
	int test;

	while(1) {
		Show_Info();
		std::cout << "Choice : ";
//		std::getline(std::cin, check);
		std::cin >> choice;
		std::cin.ignore();
		std::cout << std::endl;


		switch(choice){
		case '1':	
			Init_packet();
			system("clear");
			Make_packet();
			Del_enc_packet();
			break;

		case '2':
			Packet_History();
			break;
		case '3':
			Del_history_packet();
			return 0;

///Receive test
		case '4':
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
	
