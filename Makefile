CC = gcc
LDFLAGS = -lportaudio
LIB = $(wildcard lib/*.c)
main: main.c $(LIB)
	$(CC) -Wall -o $@ $^ $(LDFLAGS)
debug: main.c $(LIB)
	$(CC) -Wall -g $^ $(LDFLAGS) && gdb ./a.out
