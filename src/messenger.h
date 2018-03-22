#ifndef __MESSENGER_H_
#define __MESSENGER_H_

#include "message.h"
#include <vector>

class Messenger
{
	vector<Message*> message_total;
	int msgNum;
public :
	Messenger();
	
	void Init_packet();
	void Make_packet();
	void Recv_packet();
	void Del_packet();
};

#endif
