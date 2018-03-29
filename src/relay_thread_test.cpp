#include <thread>
#include <iostream>
#include "relay3.c"
using namespace std;

void func1() {
	while(1) {
		printf("hihi\n");
		usleep(1000000);
	}
}

int main() {
	thread t1(&func1);
	thread t(&relay);
	t1.join();

	return 0;
}
