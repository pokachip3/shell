CC = gcc
CFLAGS = -Wall -Wextra -O2
OBJS = main.o builtin.o executor.o support.o
TARGET = myshell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

builtin.o: builtin.c
	$(CC) $(CFLAGS) -c builtin.c

executor.o: executor.c
	$(CC) $(CFLAGS) -c executor.c

support.o: support.c
	$(CC) $(CFLAGS) -c support.c

clean:
	rm -f $(OBJS) $(TARGET)
