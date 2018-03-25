#include "message.h"
#include <iostream>

Message :: Message (std::string from, std::string to, std::string MSG) : from_ID(from), to_ID(to), msg(MSG)
{
	SIG_HEAD = "\xde\xad";
	SIG_TAIL = "\xf0\x0d";
	this->from_len = (char)from_ID.size();
	this->to_len = (char)to_ID.size();
}

std::string Message::Make_array() {
	
	std::string total;

	total += SIG_HEAD;
	total += from_len;
	total += from_ID;
	total += to_len;
	total += to_ID;
	total += SIG_TAIL;;
	total += msg;
	
	return total;
}

void Message::Show_msg() {

	std::cout << "---------------Original Message---------------"<<std::endl;
	std::cout << "From : " << from_ID << std::endl;
	std::cout << "To : " << to_ID << std::endl << std::endl;

	std::cout << msg << std::endl << std::endl;

}
