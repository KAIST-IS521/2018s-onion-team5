
CC = g++
OBJS = fetch_key.o listen.o udp_client.o  main.o\
../common/config.o ../common/sha1.o ../common/dumphex.o
TARGET = Onion5_client
LIBS = -lprotobuf -lpsocksxx -lcurl
CFLAGS= -std=c++11

.SUFFIXES : .cc .o

all: $(TARGET)

%.o: %.cc
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)
