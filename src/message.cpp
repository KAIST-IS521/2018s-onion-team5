#include "message.h"
#include <iostream>

Message :: Message (string from, string to, string MSG) : from_ID(from), to_ID(to), msg(MSG), from_ID_len(from.size()), to_ID_len(to.size())
{}

string Message::Make_string() {
	
	string total;
	string from_len = to_string(from_ID_len);			//int to string
	string to_len = to_string(to_ID_len);				//int to string

	total = from_len + "," + from_ID + to_len + "," + to_ID + msg;

	return total;
}
