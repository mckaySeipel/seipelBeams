/*
 * runningModes.c
 *
 *  Created on: Mar 11, 2015
 *      Author: hutch
 */
#include <stdio.h>
//Removed 2/26/18
//#include "supportFiles/leds.h"
//Removed 2/26/18
//#include "supportFiles/globalTimer.h"
//Removed 2/26/18
//#include "supportFiles/interrupts.h"
#include <stdbool.h>
#include "intervalTimer.h"
#include "queue.h"
//Removed 2/26/18
//#include "xparameters.h"
#include "filter.h"
#include "detector.h"
#include "transmitter.h"
#include <stdlib.h>
#include "buttons.h"
#include "switches.h"
#include "isr.h"
//Removed 2/26/18
//#include "supportFiles/mio.h"
#include <string.h>
//Removed 2/26/18
//#include "supportFiles/display.h"
#include "trigger.h"
#include "lockOutTimer.h"
#include "hitLedTimer.h"
#include <stdint.h>
//Removed 2/26/18
//#include "supportFiles/utils.h"


#define BUTTONS_BTN3_MASK 0x08 // 0000 1000

#define DETECTOR_HIT_ARRAY_SIZE FILTER_FREQUENCY_COUNT  // The array contains one location per user frequency.

#define HISTOGRAM_BAR_COUNT FILTER_FREQUENCY_COUNT
//#define ISR_CUMULATIVE_TIMER INTERRUPT_CUMULATIVE_ISR_INTERVAL_TIMER_NUMBER
#define ISR_CUMULATIVE_TIMER 0  // Not currently defined in the student's version of the code.
#define TOTAL_RUNTIME_TIMER 1   // Used to compute total run-time.
#define MAIN_CUMULATIVE_TIMER 2 // Used to compute cumulative run-time in main.

#define SYSTEM_TICKS_PER_HISTOGRAM_UPDATE 50000	// Effectively 3+ times per second.

/*****************************************************************************
 * Uncomment the line below if you want the detector to avoid the frequency
 * that is being tranmitted as indicated by the slide switches.
 *****************************************************************************/
//#define IGNORE_OWN_FREQUENCY

static uint32_t countInterruptsViaInterruptsIsrFlag = 0;  // Keep track of interrupts.
static uint32_t detectorInvocationCount = 0;  // Keep track of detector invocations.

#define PERCENTAGE_MULTIPLIER 100.0  // Need to multiply by this to get a percentage.

// Prints out various run-time statistics on the TFT display.
// Assumes the following:
// main is keeping track of detected interrupts with countInterruptsViaInterruptsIsrFlag,
// interval_timer(0) is the cumulative run-time of the ISR,
// interval_timer(1) is the total run-time,
// interval_timer(2) is the time spent in main running the filters, updating the display, and so forth.
// No comments in the code, the print statements are self-explanatory.
void runningModes_printRunTimeStatistics() {
  display_setTextSize(1);
  display_setTextColor(DISPLAY_WHITE);
  display_setCursor(0, 0);
  display_fillScreen(DISPLAY_BLACK);
  display_print("Elements remaining in ADC queue:");
  display_print(isr_adcBufferElementCount());
  display_println(); display_println();
  double runningSeconds, isrRunningSeconds, mainLoopRunningSeconds;
  intervalTimer_getTotalDurationInSeconds(TOTAL_RUNTIME_TIMER, &runningSeconds);
  display_print("Measured run time in seconds: ");
  display_print(runningSeconds);
  display_println(); display_println();
  intervalTimer_getTotalDurationInSeconds(ISR_CUMULATIVE_TIMER, &isrRunningSeconds);
  display_print("Cumulative run time in timerIsr: ");
  display_print(isrRunningSeconds); display_print(" ("); display_print(isrRunningSeconds/runningSeconds*100); display_println("%)"); display_println();
  intervalTimer_getTotalDurationInSeconds(MAIN_CUMULATIVE_TIMER, &mainLoopRunningSeconds);
  display_print("Cumulative run-time in detector: ");
  display_print(mainLoopRunningSeconds); display_print(" ("); display_print((mainLoopRunningSeconds/runningSeconds)*100); display_println("%)"); display_println();
  uint32_t interruptCount = interrupts_isrInvocationCount();
  display_print("Total interrupts:            "); display_println(interruptCount); display_println();
  display_print("Detector invocation count: ");
  display_println(detectorInvocationCount); display_println();
  display_print("Detector invocation to interrupt ratio: ");
  display_print((((double) detectorInvocationCount) / ((double) interruptCount)) * PERCENTAGE_MULTIPLIER);
  display_print("%");
}

// Group all of the inits together to reduce visual clutter.
void runningModes_initAll() {
  buttons_init();
  switches_init();
  mio_init(false);
  display_init();
  intervalTimer_initAll();
  histogram_init(HISTOGRAM_BAR_COUNT);
  leds_init(true);
  transmitter_init();
  detector_init();
  filter_init();
  isr_init();
  hitLetTimer_init();
  trigger_init();
}

// Returns the current switch-setting
uint16_t runningModes_getFrequencySetting() {
  uint16_t switchSetting = switches_read() & 0xF;  // Bit-mask the results.
  // Provide a nice default if the slide switches are in error.
  if (!(switchSetting < FILTER_FREQUENCY_COUNT))
    return FILTER_FREQUENCY_COUNT - 1;
  else
    return switchSetting;
}

// This mode runs continuously until btn3 is pressed.
// When btn3 is pressed, it exits and prints performance information to the TFT.
// During operation, it continuously displays that received power on each channel, on the TFT.
void runningModes_continuous() {
  runningModes_initAll();  // All necessary inits are called here.
  // Prints an error message if an internal failure occurs because the argument = true.
  interrupts_initAll(true);                   // Init all interrupts (but does not enable the interrupts at the devices).
  interrupts_enableTimerGlobalInts();		      // Allows the timer to generate interrupts.
  interrupts_startArmPrivateTimer();		      // Start the private ARM timer running.
  uint16_t histogramSystemTicks = 0;          // Only update the histogram display every so many ticks.
  intervalTimer_reset(ISR_CUMULATIVE_TIMER);  // Used to measure ISR execution time.
  intervalTimer_reset(TOTAL_RUNTIME_TIMER);   // Used to measure total program execution time.
  intervalTimer_reset(MAIN_CUMULATIVE_TIMER); // Used to measure main-loop execution time.
  intervalTimer_start(TOTAL_RUNTIME_TIMER);   // Start measuring total execution time.
  transmitter_setContinuousMode(true);        // Run the transmitter continuously.
  interrupts_enableArmInts();                 // The ARM will start seeing interrupts after this.
  transmitter_run();                          // Start the transmitter.
  detectorInvocationCount = 0;                // Keep track of detector invocations.
  while (!(buttons_read() & BUTTONS_BTN3_MASK)) {	// Run until you detect btn3 pressed.
    transmitter_setFrequencyNumber(runningModes_getFrequencySetting());
    detectorInvocationCount++; // Used for run-time statistics.
    histogramSystemTicks++;    // Keep track of ticks so you know when to update the histogram.
    // Run filters, compute power, etc.
    intervalTimer_start(MAIN_CUMULATIVE_TIMER); // Measure run-time when you are doing something.
#ifdef IGNORE_OWN_FREQUENCY
    detector(true, true);   // true, true means interrupts are enabled, ignore your set frequency.
#else
    detector(true, false);  // true, false means interrupts are enabled, don't ignore your set frequency.
#endif
    intervalTimer_stop(MAIN_CUMULATIVE_TIMER);
    // If enough ticks have transpired, update the histogram.
    if (histogramSystemTicks >= SYSTEM_TICKS_PER_HISTOGRAM_UPDATE) {
      double powerValues[FILTER_FREQUENCY_COUNT];    // Copy the current power values to here.
      filter_getCurrentPowerValues(powerValues);     // Copy the current power values.
      histogram_plotUserFrequencyPower(powerValues); // Plot the power values on the TFT.
      histogramSystemTicks = 0;		                   // Reset the tick count and wait for the next update time.
    }
  }
  interrupts_disableArmInts();            // Stop interrupts.
  runningModes_printRunTimeStatistics();  // Print the run-time statistics.
}

// Game-playing mode. Each shot is registered on the histogram on the TFT.
#define MAX_HIT_COUNT 20  // Shooter mode terminates after this many shots.
void runningModes_shooter() {
  uint16_t hitCount = 0;
  detectorInvocationCount = 0;                // Keep track of detector invocations.
  runningModes_initAll();
  trigger_enable();	// Makes the trigger state machine responsive to the trigger.
  interrupts_initAll(true);             // Inits all interrupts but does not enable them.
  interrupts_enableTimerGlobalInts();		// Allows the timer to generate interrupts.
  interrupts_startArmPrivateTimer();		// Start the private ARM timer running.
  uint16_t histogramSystemTicks = 0;		// Only update the histogram display every so many ticks.
  intervalTimer_reset(ISR_CUMULATIVE_TIMER);  // Used to measure ISR execution time.
  intervalTimer_reset(TOTAL_RUNTIME_TIMER);   // Used to measure total program execution time.
  intervalTimer_reset(MAIN_CUMULATIVE_TIMER); // Used to measure main-loop execution time.
  intervalTimer_start(TOTAL_RUNTIME_TIMER);   // Start measuring total execution time.
  interrupts_enableArmInts();		// The ARM will start seeing interrupts after this.
  lockoutTimer_start();					// Ignore erroneous hits at startup (when all power values are essentially 0).
  while ((!(buttons_read() & BUTTONS_BTN3_MASK)) && hitCount < MAX_HIT_COUNT) {	// Run until you detect btn3 pressed.
    if (interrupts_isrFlagGlobal) {  // Only do something if an interrupt has occurred.
      intervalTimer_start(MAIN_CUMULATIVE_TIMER);  // Measure run-time when you are doing something.
      histogramSystemTicks++;	// Keep track of ticks so you know when to update the histogram.
      countInterruptsViaInterruptsIsrFlag++;	// Keep track of the interrupt-count based on the global flag.
      interrupts_isrFlagGlobal = 0;		// Reset the global flag.
      // Run filters, compute power, run hit-detection.
      detectorInvocationCount++; // Used for run-time statistics.
      detector(true, false);	// true, false means: interrupts are enabled, do not ignore your set frequency.
      //  detector(true, true);	// true, true means: interrupts are enabled, ignore hits on your set frequency.
      if (detector_hitDetected()) {  // Hit detected
        hitCount++;  // increment the hit count.
        detector_clearHit();  // Clear the hit.
        detector_hitCount_t hitCounts[DETECTOR_HIT_ARRAY_SIZE];	// Store the hit-counts here.
        detector_getHitCounts(hitCounts);  // Get the current hit counts.
        histogram_plotUserHits(hitCounts); // Plot the hit counts on the TFT.
      }
      uint16_t switchValue = switches_read();	// Read the switches and switch frequency as required.
      transmitter_setFrequencyNumber(switchValue);
    }
    intervalTimer_stop(MAIN_CUMULATIVE_TIMER);  // All done with actual processing.
  }
  interrupts_disableArmInts();	// Done with loop, disable the interrupts.
  hitLedTimer_turnLedOff();     // Save power :-)
  runningModes_printRunTimeStatistics();  // Print the run-time statistics to the TFT.
  printf("Shooter mode terminated after detecting %d shots.\n\r", MAX_HIT_COUNT);
}
