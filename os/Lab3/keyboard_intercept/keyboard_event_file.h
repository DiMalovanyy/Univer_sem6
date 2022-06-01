#ifndef KEYBOARD_EVENT_FILE
#define KEYBOARD_EVENT_FILE
/**
 * Tries to find the filepath of a connected keyboard
 * \return malloc allocated filepath of keyboard, or NULL if none could be 
 *         detected
 */
char *get_keyboard_event_file(void);

#endif
