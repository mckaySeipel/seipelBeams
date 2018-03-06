/*
 * trigger.c
 *
 *  Created on: Feb 23, 2016
 *      Author: seipelm
 */


#include "buttons.h"
#include <stdint.h>
#include <stdio.h>
#include "trigger.h"

//Removed 2/26/18
#include "supportFiles/mio.h"
#include "transmitter.h"

//necessary PIN info
#define TRIGGER_GUN_TRIGGER_MIO_PIN 10
#define GUN_TRIGGER_PRESSED 1
#define TRIGGER_BUTTON_0_MASK 0x01 // 0000 0001
// initialize our ignore to true
bool ignoreGunInput = true;

//for testing
bool testMode = false;
//test player
#define TRIGGER_TEST_PLAYER 0

//expire value
#define TRIGGER_TIMER_EXPIRE 500
//timer variables
uint32_t dbTimer;



enum trigger_st_t {
    init_wait_st,        // Start here, until press detected
    press_bounce_st,          // Wait here until debounced
    wait_for_release_st,         // wait for the trigger to be realsed
    release_bounce_st,          // debounce realse
    end_st                  // end
} currentTrigger = init_wait_st;




// The trigger state machine debounces both the press and release of gun trigger.
// Ultimately, it will activate the transmitter when a debounced press is detected.

// Init trigger data-structures.
// Determines whether the trigger switch of the gun is connected (see discussion in lab web pages).
// Initializes the mio subsystem.
void trigger_init()
{
    //so we can talk to the pins
    mio_init(false);
    //set the pins
    mio_setPinAsInput(TRIGGER_GUN_TRIGGER_MIO_PIN);
    //stop ignoring the trigger
    trigger_enable();
}


// Enable the trigger state machine. The trigger state-machine is inactive until this function is called.
// This allows you to ignore the trigger when helpful (mostly useful for testing).
// I don't have an associated trigger_disable() function because I don't need to disable the trigger.
void trigger_enable()
{
    //stop ignoring the trigger
    ignoreGunInput = false;
}



// Trigger can be activated by either btn0 or the external gun that is attached to TRIGGER_GUN_TRIGGER_MIO_PIN
// Gun input is ignored if the gun-input is high when the init() function is invoked.
bool triggerPressed() {
    return ((!ignoreGunInput & (mio_readPin(TRIGGER_GUN_TRIGGER_MIO_PIN) == GUN_TRIGGER_PRESSED)) ||
                (buttons_read() & TRIGGER_BUTTON_0_MASK));
}



void trigger_runTest()
{
    //set test mode  to true
    testMode = true;
    //set player frequence to test output
    transmitter_setFrequencyNumber(TRIGGER_TEST_PLAYER);
}


// Standard tick function.
void trigger_tick()
{
    switch (currentTrigger) {
           case init_wait_st:
                //reset the timers and wait here until a button is pressed
               dbTimer = 0;
               break;
           case press_bounce_st:
               //increment timer
               dbTimer++;
               break;
           case wait_for_release_st:
               //reset timer
               dbTimer = 0;
               break;
           case release_bounce_st:
               //increment timer
               dbTimer++;
               break;
           case end_st:
               //do nothing
               break;
           default:
               //print default statement
               printf("transmitter_tick state action: hit default\n\r");
               break;
           }

           // Perform state update next.
           switch (currentTrigger) {
           case init_wait_st:
               if(triggerPressed())
               {
                   //if the trigger is pressed move to next state
                   currentTrigger = press_bounce_st;
               }
               break;
           case press_bounce_st:
               if(triggerPressed() && dbTimer == TRIGGER_TIMER_EXPIRE)
               {
                   //if the trigger is pressed and timer expired
                   currentTrigger = wait_for_release_st;
                   transmitter_run();
                   if(testMode)
                   {
                       //if test mode run in continous mode
                       transmitter_setContinuousMode(true);
                       //run
                       transmitter_run();
                       //print
                       printf("D\n\r");
                   }
               }
               else if(!triggerPressed())
               {
                   //this means the trigger is released before the timer expires to we return to init_wait state
                   //this was a glitch
                   currentTrigger = init_wait_st;
               }
               break;
           case wait_for_release_st:
               if(!triggerPressed())
               {
                   //if the trigger is not pressed move to next state
                   currentTrigger = release_bounce_st;
               }
               break;
           case release_bounce_st:
               if(!triggerPressed() && dbTimer == TRIGGER_TIMER_EXPIRE)
               {
                   //if the trigger is not pressed and timer expired
                   currentTrigger = end_st;
                   if(testMode)
                   {
                       //end the trasnmission
                       transmitter_setContinuousMode(false);
                       //debug print
                       printf("U\n\r");

                   }
               }
               else if(triggerPressed())
               {
                   //this means the trigger is pressed before the timer expires to we return to init_wait state
                   //this was a glitch
                   //so keep waiting until it has been released
                   dbTimer = 0;
               }
               break;
           case end_st:
               //do nothing for now
               //loop back through to the wait state
               currentTrigger = init_wait_st;
               break;
           default:
               //end case
               printf("transmitter_tick state action: hit default\n\r");
               break;
           }

}
