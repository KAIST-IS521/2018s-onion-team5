
CC = g++
OBJS = config.o main.o node_manage.o node.pb.o
TARGET = Onion5_Server
LIBS = -pthread -lprotobuf -lpsocksxx
CFLAGS= -std=c++11


.SUFFIXES : .cc .o

all: $(TARGET)

%.o: %.cc
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)
