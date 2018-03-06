/*
 * intervalTimer.h
 *
 *  Created on: Sep 21, 2015
 *      Author: seipelm
 */
//generated with the class
#ifndef INTERVALTIMER_H_
#define INTERVALTIMER_H_




// my include statements
//removed 2/26/18
// #include "xil_io.h"
//#include "xparameters.h"	removed 3/5/18 swr
#include <stdio.h>
#include <stdint.h>
//generated with the class
#endif /* INTERVALTIMER_H_ */

//starts timers
uint32_t intervalTimer_start(uint32_t timerNumber);
//stops timers
uint32_t intervalTimer_stop(uint32_t timerNumber);
//resets timers
uint32_t intervalTimer_reset(uint32_t timerNumber);
// initializes timers
uint32_t intervalTimer_init(uint32_t timerNumber);
// initializes all timers
uint32_t intervalTimer_initAll();
// resets all timers
uint32_t intervalTimer_resetAll();
//test all timers
uint32_t intervalTimer_testAll();
// test timers
uint32_t intervalTimer_runTest(uint32_t timerNumber);
//gets totla value of timers in seconds
uint32_t intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber, double *seconds);
//reads address of timers
uint32_t intervalTimer_readAddress(int timer, uint32_t registerOffset);
// sets bits in timer registers
void intervalTimer_setBit(int32_t bitValue, uint32_t address);
// clears bits in timer registers
void intervalTimer_clearBit(int32_t bitValue, uint32_t address);
//loops doing nothing for about 2 minutes
void waitALongTime();
