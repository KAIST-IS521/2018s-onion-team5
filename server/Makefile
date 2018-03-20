
CC = g++
OBJS = config.o main.o node.pb.o
TARGET = Onion5_Server
LIBS = -pthread -lprotobuf
CFLAGS= -std=c++11


.SUFFIXES : .cc .o

all: $(TARGET)

%.o: %.cc
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)
