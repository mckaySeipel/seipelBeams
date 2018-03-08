/*
 * intervalTimer.c
 *
 *  Created on: Sep 28, 2015
 *      Author: seipelm
 */

// Change whole file. Logic is sound. removed

//includes the associated header file
#include "intervalTimer.h"
/**
/////////////////////////SET / CLEAR BIT VALUES/////////////////////////////

#define INTERVAL_TIMER_CASC_SET_VALUE  0x0800
#define INTERVAL_TIMER_ENTO_SET_VALUE  0x0080
#define INTERVAL_TIME_ENT0_CLEAR_VALUE 0xff7f // inverted to clear
#define INTERVAL_TIMER_UDT_CLEAR_VALUE  0xfffd// inverted to clear
#define INTERVAL_TIMER_LOAD_SET_VALUE 0x0020

////////////////////////////////////////////////////TIMER 0//////////////////
#define INTERVAL_TIMER_TIMER_1 0
/////////////COUNTER 1
#define INTERVAL_TIMER_1_TCSR0_ADDRESS (XPAR_AXI_TIMER_0_BASEADDR + 0x00)
#define INTERVAL_TIMER_1_TLR0_ADDRESS (XPAR_AXI_TIMER_0_BASEADDR + 0x04)
#define INTERVAL_TIMER_1_TRC0_ADDRESS (XPAR_AXI_TIMER_0_BASEADDR + 0x08)
////////////COUNTER 2
#define INTERVAL_TIMER_1_TCSR1_ADDRESS (XPAR_AXI_TIMER_0_BASEADDR + 0x10)
#define INTERVAL_TIMER_1_TLR1_ADDRESS (XPAR_AXI_TIMER_0_BASEADDR + 0x14)
#define INTERVAL_TIMER_1_TRC1_ADDRESS (XPAR_AXI_TIMER_0_BASEADDR + 0x18)
//////////////////////////////////////////////////////TIMER 2////////////////
#define INTERVAL_TIMER_TIMER_2 1
/////////////COUNTER 1
#define INTERVAL_TIMER_2_TCSR0_ADDRESS (XPAR_AXI_TIMER_1_BASEADDR + 0x00)
#define INTERVAL_TIMER_2_TLR0_ADDRESS (XPAR_AXI_TIMER_1_BASEADDR + 0x04)
#define INTERVAL_TIMER_2_TRC0_ADDRESS (XPAR_AXI_TIMER_1_BASEADDR + 0x08)
////////////COUNTER 2
#define INTERVAL_TIMER_2_TCSR1_ADDRESS (XPAR_AXI_TIMER_1_BASEADDR + 0x10)
#define INTERVAL_TIMER_2_TLR1_ADDRESS (XPAR_AXI_TIMER_1_BASEADDR + 0x14)
#define INTERVAL_TIMER_2_TRC1_ADDRESS (XPAR_AXI_TIMER_1_BASEADDR + 0x18)

//////////////////////////////////////////////////////TIMER 3///////////////
#define INTERVAL_TIMER_TIMER_3 2
/////////////COUNTER 1
#define INTERVAL_TIMER_3_TCSR0_ADDRESS (XPAR_AXI_TIMER_2_BASEADDR + 0x00)
#define INTERVAL_TIMER_3_TLR0_ADDRESS (XPAR_AXI_TIMER_2_BASEADDR + 0x04)
#define INTERVAL_TIMER_3_TRC0_ADDRESS (XPAR_AXI_TIMER_2_BASEADDR + 0x08)
////////////COUNTER 2
#define INTERVAL_TIMER_3_TCSR1_ADDRESS (XPAR_AXI_TIMER_2_BASEADDR + 0x10)
#define INTERVAL_TIMER_3_TLR1_ADDRESS (XPAR_AXI_TIMER_2_BASEADDR + 0x14)
#define INTERVAL_TIMER_3_TRC1_ADDRESS (XPAR_AXI_TIMER_2_BASEADDR + 0x18)

///////////////////////////// STRING REPORT VALUES//////////////////////////

#define INTERVAL_TIMER_SHOULD_BE_ZERO "timer TCR0 should be changing at this point:%ld\n\r"
#define INTERVAL_TIMER_SHOULD_BE_CHANGING "timer TCR0 should be changing at this point:%ld\n\r"
#define INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING "timer TCR0 should not be changing at this point:%ld\n\r"
#define INTERVAL_TIMER_DEFAULT_ERROR_MESSAGE "Tried to access a timer that does not exist\n\r"

uint32_t intervalTimer_start(uint32_t timerNumber) {
    // switches between each timer
    switch (timerNumber) {
    //clear the ENT0 bit in the TCSR0 register.
    case 0:
        //write a 1 to the ENT0 bit of the TCSR0 register. When you do this, you must not disturb the other bits in TCSR0.
        intervalTimer_setBit(INTERVAL_TIMER_ENTO_SET_VALUE,
                INTERVAL_TIMER_1_TCSR0_ADDRESS);
        break;
    case 1:
        //write a 1 to the ENT0 bit of the TCSR0 register. When you do this, you must not disturb the other bits in TCSR0.
        intervalTimer_setBit(INTERVAL_TIMER_ENTO_SET_VALUE,
                INTERVAL_TIMER_2_TCSR0_ADDRESS);
        break;
    case 2:
        //write a 1 to the ENT0 bit of the TCSR0 register. When you do this, you must not disturb the other bits in TCSR0.
        intervalTimer_setBit(INTERVAL_TIMER_ENTO_SET_VALUE,
                INTERVAL_TIMER_3_TCSR0_ADDRESS);
        break;
    default:
        //default error message
        printf("Tried to access a timer that does not exist\n\r");
        break;
    }
    return 0;

}

uint32_t intervalTimer_stop(uint32_t timerNumber) {
    // switches between each timer
    switch (timerNumber) {
    case 0:
        //clear the ENT0 bit in the TCSR0 register.
        intervalTimer_clearBit(INTERVAL_TIME_ENT0_CLEAR_VALUE,
                INTERVAL_TIMER_1_TCSR0_ADDRESS);
        break;
    case 1:
        //clear the ENT0 bit in the TCSR0 register.
        intervalTimer_clearBit(INTERVAL_TIME_ENT0_CLEAR_VALUE,
                INTERVAL_TIMER_1_TCSR0_ADDRESS);
        break;
    case 2:
        //clear the ENT0 bit in the TCSR0 register.
        intervalTimer_clearBit(INTERVAL_TIME_ENT0_CLEAR_VALUE,
                INTERVAL_TIMER_1_TCSR0_ADDRESS);
        break;
    default:
        //default error message
        printf(INTERVAL_TIMER_DEFAULT_ERROR_MESSAGE);
        break;
    }
    return 0;
}

uint32_t intervalTimer_reset(uint32_t timerNumber) {
    // switches between each timer
    switch (timerNumber) {
    case 0:
        printf("Reseting Timer 1\n\r");
        //write a 0 into the TLR0 register.
        Xil_Out32(INTERVAL_TIMER_1_TLR0_ADDRESS, 0);
        //write a 1 into the LOAD0 bit in the TCSR0.
        intervalTimer_setBit(INTERVAL_TIMER_LOAD_SET_VALUE,
                INTERVAL_TIMER_1_TCSR0_ADDRESS);
        //write a 0 into the TLR1 register.
        Xil_Out32(INTERVAL_TIMER_1_TLR1_ADDRESS, 0);
        //write a 1 into the LOAD1 bit in the TCSR01
        intervalTimer_setBit(INTERVAL_TIMER_LOAD_SET_VALUE,
                INTERVAL_TIMER_1_TCSR1_ADDRESS);
        //re-init the timer
        intervalTimer_init(INTERVAL_TIMER_TIMER_1);
        break;
    case 1:
        //write a 0 into the TLR0 register.
        Xil_Out32(INTERVAL_TIMER_2_TLR0_ADDRESS, 0);
        //write a 1 into the LOAD0 bit in the TCSR0.
        intervalTimer_setBit(INTERVAL_TIMER_LOAD_SET_VALUE,
                INTERVAL_TIMER_2_TCSR0_ADDRESS);
        //write a 0 into the TLR1 register.
        Xil_Out32(INTERVAL_TIMER_2_TLR1_ADDRESS, 0);
        //write a 1 into the LOAD1 bit in the TCSR01
        intervalTimer_setBit(INTERVAL_TIMER_LOAD_SET_VALUE,
                INTERVAL_TIMER_2_TCSR1_ADDRESS);
        //re-init the timer
        intervalTimer_init(INTERVAL_TIMER_TIMER_2);
        break;
    case 2:
        //write a 0 into the TLR0 register.
        Xil_Out32(INTERVAL_TIMER_3_TLR0_ADDRESS, 0);
        //write a 1 into the LOAD0 bit in the TCSR0.
        intervalTimer_setBit(INTERVAL_TIMER_LOAD_SET_VALUE,
                INTERVAL_TIMER_3_TCSR0_ADDRESS);
        //write a 0 into the TLR1 register.
        Xil_Out32(INTERVAL_TIMER_3_TLR1_ADDRESS, 0);
        //write a 1 into the LOAD1 bit in the TCSR01
        intervalTimer_setBit(INTERVAL_TIMER_LOAD_SET_VALUE,
                INTERVAL_TIMER_3_TCSR1_ADDRESS);
        //re-init the timer
        intervalTimer_init(INTERVAL_TIMER_TIMER_3);
        break;
    default:
        //default error message
        printf(INTERVAL_TIMER_DEFAULT_ERROR_MESSAGE);
        break;
    }
    return 0;
}

uint32_t intervalTimer_init(uint32_t timerNumber) {
    // switches between each timer
    switch (timerNumber) {
    case 0:
        //write a 0 to the TCSR0 register.
        Xil_Out32(INTERVAL_TIMER_1_TCSR0_ADDRESS, 0);
        //write a 0 to the TCSR1 register.
        Xil_Out32(INTERVAL_TIMER_1_TCSR1_ADDRESS, 0);
        //set the CASC bit and clear the UDT0 bit in the TCSR0 register (cascade mode and up counting).
        intervalTimer_setBit(INTERVAL_TIMER_CASC_SET_VALUE,
                INTERVAL_TIMER_1_TCSR0_ADDRESS);
        intervalTimer_clearBit(INTERVAL_TIMER_UDT_CLEAR_VALUE,
                INTERVAL_TIMER_1_TCSR0_ADDRESS);
        break;
    case 1:
        //write a 0 to the TCSR0 register.
        Xil_Out32(INTERVAL_TIMER_2_TCSR0_ADDRESS, 0);
        //write a 0 to the TCSR1 register.
        Xil_Out32(INTERVAL_TIMER_2_TCSR1_ADDRESS, 0);
        //set the CASC bit and clear the UDT0 bit in the TCSR0 register (cascade mode and up counting).
        intervalTimer_setBit(INTERVAL_TIMER_CASC_SET_VALUE,
                INTERVAL_TIMER_2_TCSR0_ADDRESS);
        intervalTimer_clearBit(INTERVAL_TIMER_UDT_CLEAR_VALUE,
                INTERVAL_TIMER_2_TCSR0_ADDRESS);
        break;
    case 2:
        //write a 0 to the TCSR0 register.
        Xil_Out32(INTERVAL_TIMER_3_TCSR0_ADDRESS, 0);
        //write a 0 to the TCSR1 register.
        Xil_Out32(INTERVAL_TIMER_3_TCSR1_ADDRESS, 0);
        //set the CASC bit and clear the UDT0 bit in the TCSR0 register (cascade mode and up counting).
        intervalTimer_setBit(INTERVAL_TIMER_CASC_SET_VALUE,
                INTERVAL_TIMER_3_TCSR0_ADDRESS);
        intervalTimer_clearBit(INTERVAL_TIMER_UDT_CLEAR_VALUE,
                INTERVAL_TIMER_3_TCSR0_ADDRESS);
        break;
    default:
        //default error message
        printf(INTERVAL_TIMER_DEFAULT_ERROR_MESSAGE);
        break;
    }

    return 0;
}

uint32_t intervalTimer_initAll() {
    //initialize timer 1
    intervalTimer_init(INTERVAL_TIMER_TIMER_1);
    //initialize timer 2
    intervalTimer_init(INTERVAL_TIMER_TIMER_2);
    //initialize timer 3
    intervalTimer_init(INTERVAL_TIMER_TIMER_3);
    return 0;
}

uint32_t intervalTimer_resetAll() {
    //reset timer 1
    intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
    //reset timer 2
    intervalTimer_reset(INTERVAL_TIMER_TIMER_2);
    //reset timer 3
    intervalTimer_reset(INTERVAL_TIMER_TIMER_3);
    return 0;
}
uint32_t intervalTimer_testAll() {
    //test timer 1
    intervalTimer_runTest(INTERVAL_TIMER_TIMER_1);
    //test timer 2
    intervalTimer_runTest(INTERVAL_TIMER_TIMER_2);
    //test timer 3
    intervalTimer_runTest(INTERVAL_TIMER_TIMER_3);
    return 0;
}

#define DELAY_COUNT 3
// helper function that waits //starts timers
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
for about 2 minutes
void waitALongTime() {
    volatile int32_t a = 0;
    int32_t i, j;
    for (i = 0; i < DELAY_COUNT; i++)
        for (j = 0; j < INT32_MAX; j++)
            a++;
}

uint32_t intervalTimer_runTest(uint32_t timerNumber) {
    // switches between each timer
    switch (timerNumber) {
    case 0:
        //initialize Timer
        intervalTimer_init(INTERVAL_TIMER_TIMER_1);

        //Reset Timer
        intervalTimer_reset(INTERVAL_TIMER_TIMER_1);

        // Show that the timer is reset.
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));

        //start Timer
        intervalTimer_start(INTERVAL_TIMER_TIMER_1);

        // Show that the timer is running.
        printf(
                "The following register values should be changing while reading them.\n\r");
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));

        //Wait about 2 minutes so that you roll over to TCR1.
        // If you don't see a '2' in TCR1 after this long wait you probably haven't programmed the timer correctly.
        waitALongTime();

        // value after long wait
        printf("timer_0 TCR0 value after wait:%lx\n\r",
                Xil_In32(INTERVAL_TIMER_1_TCSR0_ADDRESS));

        // show timer is still active
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC1_ADDRESS));

        //stop timer
        intervalTimer_stop(INTERVAL_TIMER_TIMER_1);

        //show timer has stopped
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        //reset timer
        intervalTimer_reset(INTERVAL_TIMER_TIMER_1);
        //show timer has reset
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS));

        break;
    case 1:
        //initialize Timer
        intervalTimer_init(INTERVAL_TIMER_TIMER_2);

        //Reset Timer
        intervalTimer_reset(INTERVAL_TIMER_TIMER_2);

        // Show that the timer is reset.
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));

        //start Timer
        intervalTimer_start(INTERVAL_TIMER_TIMER_2);

        // Show that the timer is running.
        printf(
                "The following register values should be changing while reading them.\n\r");
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));

        //Wait about 2 minutes so that you roll over to TCR1.
        // If you don't see a '2' in TCR1 after this long wait you probably haven't programmed the timer correctly.
        waitALongTime();

        // value after long wait
        printf("timer_0 TCR0 value after wait:%lx\n\r",
                Xil_In32(INTERVAL_TIMER_2_TCSR0_ADDRESS));

        // show timer is still active
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC1_ADDRESS));

        //stop timer
        intervalTimer_stop(INTERVAL_TIMER_TIMER_2);

        //show timer has stopped
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        //reset timer
        intervalTimer_reset(INTERVAL_TIMER_TIMER_2);
        //show timer has reset
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS));

        break;
    case 2:
        //initialize Timer
        intervalTimer_init(INTERVAL_TIMER_TIMER_3);

        //Reset Timer
        intervalTimer_reset(INTERVAL_TIMER_TIMER_3);

        // Show that the timer is reset.
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));

        //start Timer
        intervalTimer_start(INTERVAL_TIMER_TIMER_3);

        // Show that the timer is running.
        printf(
                "The following register values should be changing while reading them.\n\r");
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));

        //Wait about 2 minutes so that you roll over to TCR1.
        // If you don't see a '2' in TCR1 after this long wait you probably haven't programmed the timer correctly.
        waitALongTime();

        // value after long wait
        printf("timer TCR0 value after wait:%lx\n\r",
                Xil_In32(INTERVAL_TIMER_3_TCSR0_ADDRESS));

        // show timer is still active
        printf(INTERVAL_TIMER_SHOULD_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC1_ADDRESS));

        //stop timer
        intervalTimer_stop(INTERVAL_TIMER_TIMER_3);

        //show timer has stopped
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOUD_NOT_BE_CHANGING,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        //reset timer
        intervalTimer_reset(INTERVAL_TIMER_TIMER_3);
        //show timer has reset
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));
        printf(INTERVAL_TIMER_SHOULD_BE_ZERO,
                Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS));

        break;
    default:
        //default error message
        printf(INTERVAL_TIMER_DEFAULT_ERROR_MESSAGE);
        break;
    }

    return 0;
}

uint32_t intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber,
        double *seconds) {

    int32_t counterOne;                    //value from counter 1
    int32_t counterTwo;                    //value from counter 2
    int64_t totalValue;              //Concatenated value of timer 1 and timer 2
    // switches between each timer
    switch (timerNumber) {
    case 0:
        //get value from counter 1
        counterOne = Xil_In32(INTERVAL_TIMER_1_TRC0_ADDRESS);
        //get value from counter 2
        counterTwo = Xil_In32(INTERVAL_TIMER_1_TRC1_ADDRESS);
        //Concatenated the values
        totalValue = counterTwo << 32 | counterOne;
        //divide by frequency and assign to seconds
        *seconds = ((double) totalValue / (double) XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ);
        break;
    case 1:
        //get value from counter 1
        counterOne = Xil_In32(INTERVAL_TIMER_2_TRC0_ADDRESS);
        //get value from counter 2
        counterTwo = Xil_In32(INTERVAL_TIMER_2_TRC1_ADDRESS);
        //Concatenated the values
        totalValue = counterTwo << 32 | counterOne;
        //divide by frequency and assign to seconds
        *seconds = ((double) totalValue/ (double) XPAR_AXI_TIMER_1_CLOCK_FREQ_HZ);
        break;

    case 2:
        //get value from counter 1
        counterOne = Xil_In32(INTERVAL_TIMER_3_TRC0_ADDRESS);
        //get value from counter 2
        counterTwo = Xil_In32(INTERVAL_TIMER_3_TRC1_ADDRESS);
        //Concatenated the values
        totalValue = counterTwo << 32 | counterOne;
        //divide by frequency and assign to seconds
        *seconds = ((double) totalValue / (double) XPAR_AXI_TIMER_2_CLOCK_FREQ_HZ);
        break;
    default:
        //default error message
        printf(INTERVAL_TIMER_DEFAULT_ERROR_MESSAGE);
        break;
    }
    return 0;
}

void intervalTimer_setBit(int32_t bitValue, uint32_t address) {
    //get the value in the address
    int32_t value = Xil_In32(address);
    //get the new value
    u32 newValue = (value | bitValue);
    //write the new value to the address
    Xil_Out32(address, newValue);
}

void intervalTimer_clearBit(int32_t bitValue, uint32_t address) {
    //get the value in the address
    int32_t value = Xil_In32(address);
    //get the new value
    u32 newValue = (value & bitValue);
    //write the new value to the address
    Xil_Out32(address, newValue);
}
*/

unsigned timer_0;
unsigned timer_1;
unsigned timer_2;
unsigned timer_3;

//helper function that reads the system tick
static inline unsigned ccnt_read(void)
{
	unsigned cc;
	asm volatile ("mrc p15, 0, %0, c15, c12, 1" : "=r" (cc));
	return cc;
}


//starts timers
uint32_t intervalTimer_start(uint32_t timerNumber)
{
	unsigned startTime = ccnt_read();
	switch (timerNumber)
	{
	case 0:
		timer_0 = startTime;
		break;
	case 1:
		timer_1 = startTime;
		break;
	case 2:
		timer_2 = startTime;
		break;
	case 3:
		timer_3 = startTime;
		break;
	default:
		//something bad has happened report
		printf("A faulty timer has been called \n");
		break;
	}

}
//stops timers
uint32_t intervalTimer_stop(uint32_t timerNumber)
{
	//deprecated with the move to the pi???????? we'll have to see what calls this
	return 0;
}
//resets timers
uint32_t intervalTimer_reset(uint32_t timerNumber)
{
	intervalTimer_init(i);
}
// initializes timers
uint32_t intervalTimer_init(uint32_t timerNumber)
{
	switch (timerNumber)
	{
	case 0:
		timer_0 = 0;
		break;
	case 1:
		timer_1 = 0;
		break;
	case 2:
		timer_2 = 0;
		break;
	case 3:
		timer_3 = 0;
		break;
	default:
		//something bad has happened report
		printf("A faulty timer has been called \n");
		break;
	}
	return 0;
}
// initializes all timers
uint32_t intervalTimer_initAll()
{
	//give access to lower level processes
	asm volatile ("mcr p15,  0, %0, c15,  c9, 0\n" : : "r" (1));
	uint_8 i;
	for (i = 0; i < 4; i++)
	{
		intervalTimer_init(i);
	}
	return 0;
}
// resets all timers
uint32_t intervalTimer_resetAll()
{
	uint_8 i;
	for (i = 0; i < 4; i++)
	{
		intervalTimer_init(i);
	}
}
//test all timers
uint32_t intervalTimer_testAll()
{]
return 0;}
// test timers
uint32_t intervalTimer_runTest(uint32_t timerNumber)
{
	intervalTimer_init(timerNumber);
	intervalTimer_start(timerNumber);
	sleep(1);
	intervalTimer_start(timerNumber);
	double seconds;
	intervalTimer_getTotalDurationInSeconds(timerNumber, &seconds);

	return 0;
}
//gets totla value of timers in seconds
uint32_t intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber, double *seconds)
{
	unsigned timerVal = 0;
	//get the timer
	switch (timerNumber)
	{
	case 0:
		timerVal = timer_0;
		break;
	case 1:
		timerVal = timer_1;
		break;
	case 2:
		timerVal = timer_2;
		break;
	case 3:
		timerVal = timer_3;
		break;
	default:
		//something bad has happened report
		printf("A faulty timer has been called \n");
		break;
	}
	//compare to current time
	unsigned endTime = ccnt_read();
	unsigned elapsed_tick_time = endTime - timerVal;
	printf("the number of ticks elapsed: %u\n")
		double timeInSeconds = (double)elapsed_tick_time / 1200000000;
	printf("the time elapsed in seconds: %d\n");
	return 0;
}
//reads address of timers
uint32_t intervalTimer_readAddress(int timer, uint32_t registerOffset)
{
	//shouldn't have to do anything with the move to the pi
	return 0;
}
// sets bits in timer registers
void intervalTimer_setBit(int32_t bitValue, uint32_t address){
	//shouldn't have to do anything with the move to the pi

}
// clears bits in timer registers
void intervalTimer_clearBit(int32_t bitValue, uint32_t address){
//shouldn't have to do anything with the move to the pi
}
//loops doing nothing for about 2 minutes
void waitALongTime(){
	//shouldn't have to do anything with the move to the pi
}
