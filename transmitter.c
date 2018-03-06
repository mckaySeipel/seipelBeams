

#include <stdint.h>
#include <stdio.h>
#include "transmitter.h"
//removed 2/26/18
//#include "supportFiles/mio.h"
#include "filter.h"
//#include "switches.h"	removed 3/5/18 swr
//#include "buttons.h"	removed 3/5/18 swr
//removed 2/26/18
//#include "supportFiles/utils.h"

#define TRASMITTER_ONE_PERIOD 300

//used to talk with the pins
#define TRANSMITTER_HIGH_VALUE 1
#define TRANSMITTER_LOW_VALUE 0
#define TRANSMITTER_OUTPUT_PIN 13			// JF1 (pg. 25 of ZYBO reference manual).

#define TRANSMITTER_PULSE_WIDTH 20000	// Based on a system tick-rate of 100 kHz.
#define TRANSMITTER_50_DUTY 2           // Used to create a 50% duty cycle

//player frequencies
uint16_t trasnmitterFrequency;

//used for testing
#define trasnmitNumberOfPlayers 10

//controlling signals
bool transmitContious = false;
bool tranmitRunning = false;

//timer variables
uint32_t pulseTimerExpire;
uint32_t pulseTimer;
uint32_t outputTimer;

//switch masking
#define  TRANSMIT_PLAYER_1 0x00
#define  TRANSMIT_PLAYER_2 0x01
#define  TRANSMIT_PLAYER_3 0x02
#define  TRANSMIT_PLAYER_4 0x03
#define  TRANSMIT_PLAYER_5 0x04
#define  TRANSMIT_PLAYER_6 0x05
#define  TRANSMIT_PLAYER_7 0x06
#define  TRANSMIT_PLAYER_8 0x07
#define  TRANSMIT_PLAYER_9 0x08
#define  TRANSMIT_PLAYER_10 0x09

//player numbers
#define TRANSMIT_PLAYER_ID_1 0
#define TRANSMIT_PLAYER_ID_2 1
#define TRANSMIT_PLAYER_ID_3 2
#define TRANSMIT_PLAYER_ID_4 3
#define TRANSMIT_PLAYER_ID_5 4
#define TRANSMIT_PLAYER_ID_6 5
#define TRANSMIT_PLAYER_ID_7 6
#define TRANSMIT_PLAYER_ID_8 7
#define TRANSMIT_PLAYER_ID_9 8
#define TRANSMIT_PLAYER_ID_10 9

// end button mask
#define BUTTON_1  0x02 // 0000 0010
#define BUTTON_2  0x04 // 0000 0100



//global to keep track of player
uint16_t currentPlayer;


enum transmitter_st_t {
    init_st,        // Start here, stay in this state for just one tick.
    on_st,          // transmit on
    off_st,         // transmit off
    trans_fin_st        //end state
} currentStateTrans = init_st;

void transmitter_set_jf1_to_one() {
  //mio_writePin(TRANSMITTER_OUTPUT_PIN, TRANSMITTER_HIGH_VALUE); // Write a '1' to JF-1.	removed 3/5/18 swr
}

void transmitter_set_jf1_to_zero() {
  //mio_writePin(TRANSMITTER_OUTPUT_PIN, TRANSMITTER_LOW_VALUE); // Write a '0' to JF-1.	removed 3/5/18 swr
}

// The transmitter state machine generates a square wave output at the chosen frequency
// as set by transmitter_setFrequencyNumber(). The step counts for the frequencies
// are provided in filter.h

// Standard init function.
void transmitter_init()
{
    //make sure all the variables are initialized
     pulseTimerExpire = 0;
     //set to zero
     pulseTimer = 0;
     //set to zero
     outputTimer = 0;
     //set to zero
     trasnmitterFrequency = 0;
     //set to zero
     currentPlayer = 0;
     //initialize the pins
     //mio_init(false);								removed 3/5/18 swr
     //set the pins
     //mio_setPinAsOutput(TRANSMITTER_OUTPUT_PIN);	removed 3/5/18 swr
}

// Starts the transmitter.
void transmitter_run()
{
    //set bool of running
    tranmitRunning = true;
}

// Returns true if the transmitter is still running.
bool transmitter_running()
{
    //if return our running boolean
    return tranmitRunning;
}

// Sets the frequency number. If this function is called while the
// transmitter is running, the frequency will not be updated until the
// transmitter stops and transmitter_run() is called again.
void transmitter_setFrequencyNumber(uint16_t frequencyNumber)
{
    //set the current frequency number to the given index

    //set the frequency
    trasnmitterFrequency = filter_frequencyTickTable[frequencyNumber];
    currentPlayer = frequencyNumber;

}

// Standard tick function.
void transmitter_tick()
{
    //this state machine will be simple
    //initial, on, off, final

    //output timer must tick every tick
    outputTimer++;
    switch (currentStateTrans) {
       case init_st:
          // wait for running signal
           break;
       case on_st:
           //increment timer
           pulseTimer++;
           transmitter_set_jf1_to_one();
           break;
       case off_st:
           //increment timer
           pulseTimer++;
           transmitter_set_jf1_to_zero();
           break;
       case trans_fin_st:
           //make sure that it is not transmitting after we have finished our pulse
           transmitter_set_jf1_to_zero();
           break;
       default:
           //faulty state
           printf("transmitter_tick state action: hit default\n\r");
           break;
       }

       // Perform state update next.
       switch (currentStateTrans) {
       case init_st:
           if(tranmitRunning || transmitContious)
           {

               currentStateTrans = on_st;
               //set expire
               pulseTimerExpire = (trasnmitterFrequency / TRANSMITTER_50_DUTY);
               //reset pulse timer
               pulseTimer = 0;
               //reset output timer
               outputTimer = 0;
           }
           break;
       case on_st:
           if(pulseTimer == pulseTimerExpire)
           {
               //move state and reset
               currentStateTrans = off_st;
               pulseTimer = 0;
           }
           else if (outputTimer >= TRANSMITTER_PULSE_WIDTH)
           {
               // time to end the cycling
               currentStateTrans = trans_fin_st;
           }
           break;
       case off_st:
           if(pulseTimer == pulseTimerExpire)
           {
               //move state and reset
               currentStateTrans = on_st;
               pulseTimer = 0;
           }
           else if (outputTimer >= TRANSMITTER_PULSE_WIDTH)
           {
               // time to end the cycling
               currentStateTrans = trans_fin_st;
           }
           break;
       case trans_fin_st:
           //move to init state
           currentStateTrans = init_st;
           //stop running the machine
           tranmitRunning = false;
           break;
       default:
           //faulty state
           printf("transmitter_tick state action: hit default\n\r");
           break;
       }
}
//removed 3/5/18 swr
/**
// Tests the transmitter.
void transmitter_runTest()
{

//this will show a burst burst until we tell it to stop
    while(1)
    {
        if (buttons_read() == BUTTON_1)
        {
            //make sure we are not in conintuoues mode
            transmitter_setContinuousMode(false);
            //exit out of this test
            break;
        }

        //read the switches
        int32_t  player = switches_read();
        //set the player id
        int16_t playerId = 0;
        //switch
        switch (player) {

             case TRANSMIT_PLAYER_1://player 1
                 playerId = TRANSMIT_PLAYER_ID_1;
                              break;//break
             case TRANSMIT_PLAYER_2://player 2
                 playerId = TRANSMIT_PLAYER_ID_2;
                              break;//break
             case TRANSMIT_PLAYER_3://player 3
                 playerId = TRANSMIT_PLAYER_ID_3;
                             break;//break
             case TRANSMIT_PLAYER_4://player 4
                 playerId = TRANSMIT_PLAYER_ID_4;
                              break;//break
             case TRANSMIT_PLAYER_5://player 5
                 playerId = TRANSMIT_PLAYER_ID_5;
                              break;//break
             case TRANSMIT_PLAYER_6://player 6
                 playerId = TRANSMIT_PLAYER_ID_6;
                              break;//break;
             case TRANSMIT_PLAYER_7://player 7
                 playerId = TRANSMIT_PLAYER_ID_7;
                              break;//break
             case TRANSMIT_PLAYER_8://player 8
                 playerId = TRANSMIT_PLAYER_ID_8;
                             break;//break
             case TRANSMIT_PLAYER_9://player 9
                 playerId = TRANSMIT_PLAYER_ID_9;
                              break;//break
             case TRANSMIT_PLAYER_10://player 10
                 playerId = TRANSMIT_PLAYER_ID_10;
                              break;//break
             default://if it goes over it is player 10
                 playerId = TRANSMIT_PLAYER_ID_10;
                 break;//break
        }


        if(playerId != currentPlayer)
        {
            //make sure that a new player is set
            transmitter_setContinuousMode(false);
            //set the frequency number
            transmitter_setFrequencyNumber(playerId);
        }

            //set player
            transmitter_setFrequencyNumber(playerId);
            //report current player frequency
            //run continuous
            transmitter_setContinuousMode(true);
            //start

            transmitter_run();


    }

    transmitter_setContinuousMode(false);

    while(1)
       {
        //tell us we are moving to the next test
            printf("Next Test\n\r");


            // this will pause and wait 300 ms
            utils_msDelay(TRASMITTER_ONE_PERIOD);
            //wait here until you want to see the transmission
            if (buttons_read() == BUTTON_1)
                {
                    //this will break it out of the test
                    transmitter_setContinuousMode(false);
                    return;
                }

           //read the switches
           int32_t  player = switches_read();
           //set the player id
           int16_t playerId = 0;
           //switch
           switch (player) {

           case TRANSMIT_PLAYER_1://player 1
                     playerId = TRANSMIT_PLAYER_ID_1;
                                  break;//break
                 case TRANSMIT_PLAYER_2://player 2
                     playerId = TRANSMIT_PLAYER_ID_2;
                                  break;//break
                 case TRANSMIT_PLAYER_3://player 3
                     playerId = TRANSMIT_PLAYER_ID_3;
                                 break;//break
                 case TRANSMIT_PLAYER_4://player 4
                     playerId = TRANSMIT_PLAYER_ID_4;
                                  break;//break
                 case TRANSMIT_PLAYER_5://player 5
                     playerId = TRANSMIT_PLAYER_ID_5;
                                  break;//break
                 case TRANSMIT_PLAYER_6://player 6
                     playerId = TRANSMIT_PLAYER_ID_6;
                                  break;//break;
                 case TRANSMIT_PLAYER_7://player 7
                     playerId = TRANSMIT_PLAYER_ID_7;
                                  break;//break
                 case TRANSMIT_PLAYER_8://player 8
                     playerId = TRANSMIT_PLAYER_ID_8;
                                 break;//break
                 case TRANSMIT_PLAYER_9://player 9
                     playerId = TRANSMIT_PLAYER_ID_9;
                                  break;//break
                 case TRANSMIT_PLAYER_10://player 10
                     playerId = TRANSMIT_PLAYER_ID_10;
                                  break;//break
                 default://if it goes over it is player 10
                     playerId = TRANSMIT_PLAYER_ID_10;
                     break;//break
           }
           //set the number
        transmitter_setFrequencyNumber(playerId);
        //run the state machine
        transmitter_run();
        // this will pause and wait 300 ms
        utils_msDelay(TRASMITTER_ONE_PERIOD);

       }
}

*/
// Runs the transmitter continuously.
// if continuousModeFlag == true, transmitter runs continuously, otherwise, transmits one pulse-width and stops.
// To set continuous mode, you must invoke this function prior to calling transmitter_run().
// If the transmitter is in currently in continuous mode, it will stop running if this function is
// invoked with continuousModeFlag == false.
void transmitter_setContinuousMode(bool continuousModeFlag)
{
    //set the bool
    transmitContious = continuousModeFlag;
}

