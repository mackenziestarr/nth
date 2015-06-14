CC = gcc
LDFLAGS = -lportaudio
main: main.c waveform.c
	$(CC) -Wall -o $@ $^ $(LDFLAGS)
debug: waveform.c main.c
	$(CC) -Wall -g $^ $(LDFLAGS) && gdb ./a.out
