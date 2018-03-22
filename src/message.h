#ifndef __MESSAGE_H_
#define __MESSAGE_H_

#include <string>

using namespace std;

class Message
{
private : 
	//Sructure
	string from_ID;
	string to_ID;
	string msg;

public : 
	Message(string from, string to, string MSG);

	string Make_string(); //Convert Message to string
};

#endif
