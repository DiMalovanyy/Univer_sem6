CC=gcc
CFLAGS=-Wall -g


all: keylog

keylog: keyboard_event_file.o keyboard_intercept.o keylogger.o
	$(CC) $^ $(CFLAGS) -o $@
	
keyboard_event_file.o: keyboard_event_file.c
	$(CC) -c $^ -o $@ $(CFLAGS)

keyboard_intercept.o: keyboard_intercept.c
	$(CC) -c $^ -o $@ $(CFLAGS)

keylogger.o: keylogger.c
	$(CC) -c $^ -o $@ $(CFLAGS)

.PHONY: clean
clean:
	rm -f *.o
	rm -f keylog


