/*
 * switches.h
 *
 *  Created on: Sep 15, 2015
 *      Author: seipelm
 */

#ifndef SWITCHES_H_
#define SWITCHES_H_

//Removed 2/26/18
//#include "xil_io.h"
//Removed 2/26/18
//#include "xparameters.h"
//Removed 2/26/18
#include "supportFiles/leds.h"
#include <stdint.h>

#endif /* SWITCHES_H_ */


// Initializes the SWITCHES driver software and hardware. Returns one of the STATUS values defined above.
int switches_init();

// Returns the current value of all 4 SWITCHESs as the lower 4 bits of the returned value.
// bit3 = SW3, bit2 = SW2, bit1 = SW1, bit0 = SW0.
int32_t switches_read();

// Runs a test of the switches. As you slide the switches, LEDs directly above the switches will illuminate.
// The test will run until all switches are slid upwards. When all 4 slide switches are slid upward,
// this function will return.
void switches_runTest();

// gets register with offset
uint32_t switches_readGpioRegister(uint32_t offset);
