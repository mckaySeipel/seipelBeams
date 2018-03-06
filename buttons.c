/*
 * buttons.c
 *
 *  Created on: Sep 28, 2015
 *      Author: seipelm
 */




/*
 * button.c
 *
 *  Created on: Sep 15, 2015
 *      Author: seipelm
 */


// includes
#include "buttons.h"
#include "supportFiles/leds.h"
#include <stdio.h>

// status defines
#define BUTTONS_INIT_STATUS_OK 1
#define BUTTONS_INIT_STATUS_FAIL 0

////////////////////////////TBD/////////////////////////////

//button value defines
#define BUTTON_0  0x01 // 0000 0001
#define BUTTON_1  0x02 // 0000 0010
#define BUTTON_2  0x04 // 0000 0100
#define BUTTON_3  0x08 // 0000 1000
#define END_CASE 0x0f // 0000 1111



    // Initializes the button driver software and hardware. Returns one of the defined status values (above).
    int buttons_init()
    {
    }

        // Returns the current value of all 4 buttons as the lower 4 bits of the returned value.
        // bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
    uint32_t buttons_read()
    {
    }

        // Runs a test of the buttons. As you push the buttons, graphics and messages will be written to the LCD
        // panel. The test will until all 4 pushbuttons are simultaneously pressed.
    void buttons_runTest()
    {
    }


