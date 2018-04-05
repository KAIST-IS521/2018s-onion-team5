CC = g++
OBJS = fetch_key.o listen.o udp_client.o  main.o\
gpg_wrapper.o message_wrapper.o \
../common/dumphex.o ../common/node.pb.o \
../common/config.o ../common/http_client.o \
../common/sha1.o ../common/util.o \
../common/tcp_server.o ../common/tcp_client.o \
messanger.o

TARGET = Onion5_client
LIBS = -lprotobuf -lpsocksxx -lcurl -lpthread -levent -lcurses
CFLAGS= -std=c++11

.SUFFIXES : .cc .o

all: $(TARGET)

%.o: %.cc
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)


#CXX = g++
#CXXFLAGS = -c -std=gnu++11 -Wall

#buildfiles = main
#removefiles = *.o

#all : main.cpp messanger.o message.o pgp_crypto.o common.o
#	$(CXX) -o main main.cpp messanger.cpp message.cpp pgp_crypto.c common.c

#clean :
#	$(RM) $(removefiles)
