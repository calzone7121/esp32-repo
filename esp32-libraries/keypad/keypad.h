#ifndef KEYPAD_H
#define KEYPAD_H

#include "driver/gpio.h"

#define ROWS 4
#define COLS 4

void keypad_init();
char keypad_scan();

#endif /* KEYPAD_H */
