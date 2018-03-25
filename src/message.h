#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include <string>


class Message
{
public : 
	//Sructure
	std::string from_ID, to_ID, msg;
	std::string SIG_HEAD, SIG_TAIL;
	char from_len, to_len;

public : 
	Message(std::string from, std::string to, std::string MSG);

	std::string Make_array(); //Convert Message to string
	

	void Show_msg();
};

#endif
