#include "message.h"
#include <iostream>


Message :: Message (string from, string to, string MSG)
{
	from_ID = from;
	to_ID = to;
	msg = MSG;
}

string Message::Make_string() {
	
	string total;
	
	total = from_ID + to_ID + msg;

	return total;
}
