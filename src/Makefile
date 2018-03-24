
CC = g++
OBJS = config.o fetch_key.o main.o
TARGET = Onion5
LIBS = -L/usr/lib/x86_64-linux-gnu -lcurl


.SUFFIXES : .c .o

all: $(TARGET)

$(TARGET): $(OBJS)
	  $(CC) -o $@ $(OBJS) $(LIBS)

clean:
	  rm -f $(OBJS) $(TARGET)
