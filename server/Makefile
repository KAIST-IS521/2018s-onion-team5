
CC = g++
OBJS = main.o node_manage.o\
broadcast_server.o rest_server.o \
rest_client.o ../common/http_client.o \
../common/dumphex.o ../common/node.pb.o \
../common/config.o

TARGET = Onion5_server
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
