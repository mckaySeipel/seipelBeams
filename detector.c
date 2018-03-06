/*
 * detector.c
 *
 *  Created on: Mar 15, 2016
 *      Author: seipelm
 */

#include "detector.h"
#include "filter.h"
#include "isr.h"
#include "supportFiles/interrupts.h"
#include "lockOutTimer.h"
#include "hitLedTimer.h"

#include <stdio.h>

#define DETECTOR_DECIMATION_LENGTH 10
#define DECETOR_ADC_MAX 4095
#define DETECTOR_SCALE_SPAN 2
#define DETECTOR_MIN -1
#define DETECTOR_FILTER_SIZE 10
#define DETECTOR_FUDGE_FACTOR 80
#define DETECTOR_MEDIAN_INDEX 5

static bool hitDetected = false;
//hit counts
uint32_t hitCounts[DETECTOR_FILTER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
//decimation
uint16_t decimationCount;

//hit counts
typedef uint16_t detector_hitCount_t;

// Always have to init things.
void detector_init() {
    //set to 0
    decimationCount = 0;
}

//this is a helper function that will scale our acd input
double scale(uint32_t scaleValue) {
    double scaleFactor = (double) DETECTOR_SCALE_SPAN / (double) DECETOR_ADC_MAX;
    double value = DETECTOR_MIN;
    value += (double) scaleValue * scaleFactor;
    return value;
}

//this helper function will sort the values
void detector_sort(double A[]) {
    double x; //swap
    int16_t i, j; //index
    for (i = 1; i < DETECTOR_FILTER_SIZE; i++) {
        x = A[i]; //set
        j = i - 1; //increment
        while (j >= 0 && A[j] > x) {
            A[j + 1] = A[j]; //swap
            j = j - 1; //swap
        }
        A[j + 1] = x; //swap
    }
}

//this is the detection algorithm
bool detecthit(bool ignoreSelf) {
    //make a new array
    double tempPower[DETECTOR_FILTER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
    //get the power values
    filter_getCurrentPowerValues(tempPower);
    //sort them
    detector_sort(tempPower);
    //calculate the threshold
    double threshold = tempPower[DETECTOR_MEDIAN_INDEX] * DETECTOR_FUDGE_FACTOR;
    //if the highest power is greater than the threshold we have a hit
    if (tempPower[DETECTOR_FILTER_SIZE - 1] > threshold) {
        //return true
        return true;
    }
    //else we don't
    return false;
}

// Runs the entire detector: decimating fir-filter, iir-filters, power-computation, hit-detection.
// if interruptsNotEnabled = true, interrupts are not running.
// if ignoreSelf == true, ignore hits that are detected on your frequency.
// Use this to determine whether you should disable and re-enable interrrupts when accessing the adc queue.
void detector(bool interruptsNotEnabled, bool ignoreSelf) {
    //store my element count
    uint32_t elementCount = isr_adcBufferElementCount();
    //decimation count
    //do the following elementCount times
    for (uint32_t i = 0; i < elementCount; i++) {
        //if the intterupts are enabled
        uint32_t rawAdcValue = 0;
        if (interruptsNotEnabled == false) {
            //disable intterupts
            interrupts_disableArmInts();
            //pop from isr queue
            rawAdcValue = isr_removeDataFromAdcBuffer();
            //enable the intterupts again
            interrupts_enableArmInts();
        } else {
            rawAdcValue = isr_removeDataFromAdcBuffer();
        }

        //get a value from -1 to 1
        double scaledValue = scale(rawAdcValue);
//        printf("raw:%ld, scaled:%le\n\r", rawAdcValue, scaledValue);
        //ad the input to the filter
        filter_addNewInput(scaledValue);
        //increment
        decimationCount++;
        //if we are decimation factor length
        if (decimationCount == DETECTOR_DECIMATION_LENGTH) {
            //reset count
            decimationCount = 0;
            //run fir filter
            filter_firFilter();
            //run the iir filter for each channel
            for (uint32_t j = 0; j < DETECTOR_FILTER_SIZE; j++) {
                //run the filter
                filter_iirFilter(j);
                //at the same time compute the power for each filter
                filter_computePower(j, false, false);
            }
        }
    }
    if (!lockoutTimer_running()) {
        //see if we got a hit
        if (detecthit(ignoreSelf)) {

            //also increment the value of the detected location
            double currentValue[DETECTOR_FILTER_SIZE] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
            //get the original unsorted values
            uint16_t index = 0;
            filter_getNormalizedPowerValues(currentValue, &index);
            //loop through the original unsorted values to get the desired index

            //increment the hit count for the frequency
            hitCounts[index]++;
            //break out of this

            lockoutTimer_start();
            hitLedTimer_start();
            hitDetected = true;
        }
    }
}

// Returns true if a hit was detected.
bool detector_hitDetected() {
    //return hit Detected
    return hitDetected;
}

// Clear the detected hit once you have accounted for it.
void detector_clearHit() {
    //reset to false
    hitDetected = false;
}

// Get the current hit counts.
void detector_getHitCounts(detector_hitCount_t hitArray[]) {
    for (uint16_t i = 0; i < DETECTOR_FILTER_SIZE; i++) {
        hitArray[i] = hitCounts[i];
    }

}

void detector_runTest() {
    //test data
    double testValueHit[DETECTOR_FILTER_SIZE] = { 80000, 200, 50, 20, 70, 500,
            100, 250, 800, 0 };

    //force this data into the power values
    for (uint32_t i = 0; i < DETECTOR_FILTER_SIZE; i++) {
        printf("Filling with : %f", testValueHit[i]);

        filter_forcePower(i, testValueHit[i]);
    }
    //see if we got hit
    detector(false, false);
    if (detector_hitDetected()) {
        printf("Hit detected\n\r");
    } else {
        printf("didn't work\n\r");

    }

    //test data
    double testValueNoHit[DETECTOR_FILTER_SIZE] = { 850, 200, 50, 20, 70, 90,
            100, 250, 800, 0 };

    for (uint32_t i = 0; i < DETECTOR_FILTER_SIZE; i++) {

        printf("Filling with : %f", testValueNoHit[i]);
        filter_forcePower(i, testValueNoHit[i]);
    }

    if (detector_hitDetected()) {
        printf("It worked\n\r");
    } else {
        printf("It Failed\n\r");
    }
    for (uint32_t i = 0; i < DETECTOR_FILTER_SIZE; i++) {
        //fill with 0
        filter_forcePower(i, 0);
    }

}
