CXX = g++
CXXFLAGS = -std=gnu++11 -Wall

buildfiles = main
removefiles = *.o

all : main.cpp messenger.o message.o
	$(CXX) -o main main.cpp messenger.o message.o

Messenger.o : messenger.h message.h messenger.cpp
	$(CXX) -c messenger.cpp

Message.o : message.h message.cpp
	$(CXX) -c message.cpp

clean :
	$(RM) $(removefiles)
