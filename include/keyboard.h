#ifndef EDUOS_KEYBOARD_H
#define EDUOS_KEYBOARD_H

#include "common.h"

void keyboard_initialize(void);
char keyboard_get_char(void);
void keyboard_read_line(char* buffer, size_t max_length);

#endif /* EDUOS_KEYBOARD_H */
