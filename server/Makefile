
CC = g++
OBJS = config.o main.o
TARGET = Onion5_Server
LIBS = -pthread
CFLAGS= -std=c++11


.SUFFIXES : .cc .o

all: $(TARGET)

%.o: %.cc
	$(CC) $(CFLAGS) -c -o $@ $<

$(TARGET): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET)
