CXX = g++
CXXFLAGS = -std=gnu++11 -Wall

buildfiles = main
removefiles = *.o

all : main.cpp messenger.o message.o
	$(CXX) -o main main.cpp messenger.cpp message.cpp

clean :
	$(RM) $(removefiles)
