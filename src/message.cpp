#include "message.h"
#include <iostream>

Message :: Message (string from, string to, string MSG) : from_ID(from), to_ID(to), msg(MSG)
{
	from_ID_len = (unsigned char)from.size() & 0xFF;
	to_ID_len = (unsigned char)to.size() & 0xFF;
}

string Message::Make_string() {
	
	string total;
	string from_len = to_sring(from_ID_len);
	string to_len = to_string(to_ID_len);

	total = from_len + "," + from_ID + to_len + "," + to_ID + msg;

	return total;
}
