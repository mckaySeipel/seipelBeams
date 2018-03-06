/*
 * lockOutTimer.c
 *
 *  Created on: Feb 27, 2016
 *      Author: seipelm
 */



#include <stdio.h>
#include <stdint.h>
#include "lockOutTimer.h"
#include "intervalTimer.h"

//global flag
//running flag
volatile static bool timerStartFlag = false;

//timer
uint32_t onTimerLock = 0;

//Timer
#define LOCK_TIMER 0

enum lock_st_t {
    init_st,        // Start here, stay in this state for just one tick.
    lock_st,          // transmit on
    lock_fin_st        //end state
} currentStateLock = init_st;


// Standard init function. Implement even if you don't find it necessary at present.
// Might be handy later.
void lockoutTimer_init()
{
    //do nothing for now
}

// Calling this starts the timer.
void lockoutTimer_start()
{
    //set to true
    timerStartFlag = true;
}

// Returns true if the timer is running.
bool lockoutTimer_running()
{
    //return timerstart flag
    return timerStartFlag;
}

// Standard tick function.
void lockoutTimer_tick()
{
    switch (currentStateLock) {
          case init_st:
              //wait here for the machine to be enabled
              onTimerLock = 0;
              break;
          case lock_st:
                 //on for 1/2 second
              //increment Timer
              onTimerLock++;
              break;
          case lock_fin_st:
              //reset timer
              //reset the flag
              timerStartFlag = false;
              onTimerLock = 0;
              break;
          default:
              printf("transmitter_tick state action: hit default\n\r");
              break;
          }

          // Perform state update next.
          switch (currentStateLock) {
          case init_st:
              //if it should strt
              if(timerStartFlag)
              {
                  //move to next state
                  currentStateLock = lock_st;

              }
              break;
          case lock_st:
              if(onTimerLock == LOCKOUT_TIMER_EXPIRE_VALUE)
              {
                  //if the timer expires move to next state
                  currentStateLock = lock_fin_st;

              }
              break;
          case lock_fin_st:
              // this is here just to be robust
              //move to next state
              currentStateLock = init_st;
              break;
          default:
              printf("Lock_tick state action: hit default\n\r");
              break;
          }
}


void lockOutTimer_test()
{
    //test the lockOut timer
    intervalTimer_init(LOCK_TIMER);
    //make sure it is reset
    intervalTimer_reset(LOCK_TIMER);

    //start lock and timer
    lockoutTimer_start();
    //start the timer
    intervalTimer_start(LOCK_TIMER);
    //loop
    while(lockoutTimer_running())
    {
        //do nothing just wait
    }
    //variable to hold time
    double totalTime = 0;
    //get the elapsed time
    intervalTimer_getTotalDurationInSeconds(LOCK_TIMER, &totalTime);
    //say what happened should be .5 seconds
    printf("Total time lock out %f seconds.\n\r", totalTime);
    //end of the test


}
