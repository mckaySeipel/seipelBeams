#ifndef DETECTOR_H_
#define DETECTOR_H_

#include <stdint.h>
#include <stdbool.h>



typedef uint16_t detector_hitCount_t;

// Always have to init things.
void detector_init();

// Runs the entire detector: decimating fir-filter, iir-filters, power-computation, hit-detection.
// if interruptsNotEnabled = true, interrupts are not running.
// if ignoreSelf == true, ignore hits that are detected on your frequency.
// Use this to determine whether you should disable and re-enable interrrupts when accessing the adc queue.
void detector(bool interruptsNotEnabled, bool ignoreSelf);

// Returns true if a hit was detected.
bool detector_hitDetected();

// Clear the detected hit once you have accounted for it.
void detector_clearHit();

// Get the current hit counts.
void detector_getHitCounts(detector_hitCount_t hitArray[]);

//costum test
void detector_runTest();

#endif /* DETECTOR_H_ */
