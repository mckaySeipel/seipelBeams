/*
 * switches.c
 *
 *  Created on: Sep 15, 2015
 *      Author: seipelm
 */

//include statements
#include "switches.h"
//Removed 2/26/18
#include "xparameters.h"
//Removed 2/26/18
#include "supportFiles/leds.h"
#include <stdio.h>

// address defines
#define  SLIDE_ADRESS XPAR_SLIDE_SWITCHES_BASEADDR
#define GPIO_OFFSET_TRI 0x04
#define GPIO_OFFSET_DATA 0x00
#define SWITCHES_INITIALIZE_INPUT 1

// switch status defines
#define SWITCHES_INIT_STATUS_OK 1
#define SWITCHES_INIT_STATUS_FAIL 0

// bitwise helpers
#define CLEAR_ADRESS  0xf
#define END_CASE  0x0f
#define CLEAR_LEDS 0x00

int switches_init()
{
    //sets the input values to on
    Xil_Out32(XPAR_SLIDE_SWITCHES_BASEADDR + GPIO_OFFSET_TRI, SWITCHES_INITIALIZE_INPUT);
    //returns true
    return SWITCHES_INIT_STATUS_OK;

}

int32_t switches_read()
{
    //reads the value of the buttons from the gpio register
    return  Xil_In32(SLIDE_ADRESS) & CLEAR_ADRESS;
    //returns value
}


void switches_runTest()
{
    // initialize switches
    switches_init();
    // initialize leds
    leds_init(true);
    // run while test is not done
    while(true)
    {
        int32_t read_value = switches_read();
        //read in switch value
        if(read_value == END_CASE) // if all are up
        {
            // blank the leds
            leds_write(CLEAR_LEDS);
            // end the loop
            break;
        }
        // write to the led
        leds_write(read_value);
    }
}


uint32_t switches_readGpioRegister(uint32_t offset)
 {
     //this initializes the address that will read form the switches
     uint32_t *address = (uint32_t *) SLIDE_ADRESS + offset;
     return *address;
 }
