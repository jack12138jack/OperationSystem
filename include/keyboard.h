#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "common.h"
#include <stdbool.h>

void keyboard_init(void);
bool keyboard_has_char(void);
char keyboard_get_char(void);

#endif /* KEYBOARD_H */
