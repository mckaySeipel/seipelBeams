#include <stdio.h>


#include "src/Queue/queue.h"
#include "src/Filter/filter.h"
#include "src/Filter/filterTest.h"
#include "src/Filter/transmitter.h"
#include "Filter/isr.h"
#include "supportFiles/interrupts.h"
#include "src/switchesAndButtonsLab/buttons.h"
#include "src/Filter/trigger.h"
#include "src/Filter/hitLedTimer.h"
#include "src/Filter/lockOutTimer.h"

#define BUTTON_3  0x08 // 0000 1000
#include "src/Filter/detector.h"
#include "src/Filter/runningModes.h"
#include <assert.h>

#define BUTTONS_BTN2_MASK 0x4   // Bit mask for BTN2



// The program comes up in continuous mode.
// Hold BTN2 while the program starts to come up in shooter mode.
int main() {

    isr_init();
    hitLetTimer_init();
    filter_init();
    detector_init();
    lockoutTimer_init();
    transmitter_init();
    trigger_init();
    detector_runTest();
  buttons_init();  // Init the buttons.
    if (buttons_read() & BUTTONS_BTN2_MASK) // Read the buttons to see if BTN2 is drepressed.
     {
        runningModes_shooter();
     }// Run shooter mode if BTN2 is depressed.
    else
      runningModes_continuous();            // Otherwise, go to continuous mode.
}

//void isr_function() {
// Empty for now.
//}
