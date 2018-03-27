CXX = g++
CXXFLAGS = -c -std=gnu++11 -Wall

buildfiles = main
removefiles = *.o

all : main.cpp messenger.o message.o pgp_crypto.o common.o
	$(CXX) -o main main.cpp messenger.cpp message.cpp pgp_crypto.c common.c

clean :
	$(RM) $(removefiles)
