/*
 * buttons.h
 *
 *  Created on: Sep 15, 2015
 *      Author: seipelm
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_
#include <stdio.h>
#include <stdint.h>

#endif /* BUTTONS_H_ */




// Initializes the button driver software and hardware. Returns one of the defined status values (above).
int buttons_init();

// Returns the current value of all 4 buttons as the lower 4 bits of the returned value.
// bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
uint32_t buttons_read();

// Runs a test of the buttons. As you push the buttons, graphics and messages will be written to the LCD
// panel. The test will until all 4 pushbuttons are simultaneously pressed.
void buttons_runTest();
