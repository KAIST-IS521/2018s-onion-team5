#ifndef __MESSENGER_H_
#define __MESSENGER_H_

#include "message.h"
#include <vector>

class Messenger
{
	std::vector<Message*> message_enc;
	std::vector<Message*> message_history;

public :
	Messenger();

	std::string Timestamp();
	void Show_Info();	
	void Init_packet(int choice);
	void Make_packet();
	int Recv_packet();//std::string dec_msg);
	void Del_enc_packet();
	void Del_history_packet();
	void Packet_History();
	
	int Main();
};

#endif
