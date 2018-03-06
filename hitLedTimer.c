/*
 * hitLedTimer.c
 *
 *  Created on: Feb 25, 2016
 *      Author: seipelm
 */

#include "hitLedTimer.h"
#include "supportFiles/leds.h"
#include "supportFiles/mio.h"
#include "supportFiles/utils.h"
#include "buttons.h"
#include <stdio.h>


//led values
#define LED_HIGH_VALUE 1
#define LED_LOW_VALUE 0
#define LED_0 0x0f
#define LED_RESET 0x00
#define BUTTON_3  0x08 // 0000 1000


//timing
#define TRASMITTER_ONE_PERIOD 500

//timer
uint32_t onTimer = 0;

//running flag
volatile static bool timerStartFlag = false;


enum led_st_t {
    init_st,        // Start here, stay in this state for just one tick.
    on_st,          // transmit on
    led_fin_st        //end state
} currentStateLed = init_st;


// Standard init function. Implement it even if it is not necessary. You may need it later.
void hitLetTimer_init()
{
    //set default values
    timerStartFlag = false;
    leds_init(false);

    //initialize the pins
    mio_init(false);
    //set the pins
    mio_setPinAsOutput(HIT_LED_TIMER_OUTPUT_PIN);
}

// Calling this starts the timer.
void hitLedTimer_start()
{
    //set timer running to true
    timerStartFlag = true;
}

// Returns true if the timer is currently running.
bool hitLedTimer_running()
{
    //return led timer running value
    return timerStartFlag;
}

// Standard tick function.
void hitLedTimer_tick()
{

    switch (currentStateLed) {
       case init_st:
           //wait here for the machine to be enabled
           onTimer = 0;
           break;
       case on_st:
              //on for 1/2 second
           //increment Timer
           onTimer++;
           break;
       case led_fin_st:
           //reset timer
           //reset the flag
           timerStartFlag = false;
           onTimer = 0;
           break;
       default:
           printf("transmitter_tick state action: hit default\n\r");
           break;
       }

       // Perform state update next.
       switch (currentStateLed) {
       case init_st:
           //if it should strt
           if(timerStartFlag)
           {
               //move to next state
               currentStateLed = on_st;
               //turn on the LED
               hitLedTimer_turnLedOn();
           }
           break;
       case on_st:
           if(onTimer == HIT_LED_TIMER_EXPIRE_VALUE)
           {
               //if the timer expires move to next state
               currentStateLed = led_fin_st;
               //turn the timer off
               hitLedTimer_turnLedOff();
           }
           break;
       case led_fin_st:
           // this is here just to be robust
           //move to next state
           currentStateLed = init_st;
           break;
       default:
           printf("transmitter_tick state action: hit default\n\r");
           break;
       }
}

// Turns the gun's hit-LED on.
void hitLedTimer_turnLedOn()
{
    //access the LED and put a voltage to it
    leds_write(LED_0);
    //should illuminate led 1
    mio_writePin(HIT_LED_TIMER_OUTPUT_PIN, LED_HIGH_VALUE); // Write a '1' to JF-11.


}
// Turns the gun's hit-LED off
void hitLedTimer_turnLedOff()
{
    //access the LED and put a voltage to it
    leds_write(LED_RESET);
    //should illuminate led 1
    mio_writePin(HIT_LED_TIMER_OUTPUT_PIN, LED_LOW_VALUE); // Write a '0' to JF-11.


}

 void hitLedTimer_runTest()
 {
     //never let it stop
     while(1)
     {
         //stop it if we press button 3
         if (buttons_read() == BUTTON_3)
                    break;
         //start the timer
         hitLedTimer_start();
         while(hitLedTimer_running())
         {
             //just wait to make sure we are in the next cycle
         }
         //delay for 300ms
         utils_msDelay(TRASMITTER_ONE_PERIOD);

     }
 }
