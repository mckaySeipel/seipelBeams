/*
 * Filter.c
 *
 *  Created on: Feb 2, 2016
 *      Author: seipelm
 */

// Filtering routines for the laser-tag project.
// Filtering is performed by a two-stage filter, as described below.
// 1. First filter is a decimating FIR filter with a configurable number of taps and decimation factor.
// 2. The output from the decimating FIR filter is passed through a bank of 10 IIR filters. The
// characteristics of the IIR filter are fixed.

/*********************************************************************************************************
 ****************************************** Main Filter Functions *****************************************
 **********************************************************************************************************/


//Dependant files

#include <stdint.h>
#include "filter.h"
#include <math.h>
#include <stdio.h>

//Filter Sizes
#define FIR_FILTER_TAP_COUNT 81
#define FILTER_FREQUENCY_COUNT 10
#define IIR_A_COEFFICIENT_COUNT 10
#define IIR_B_COEFFICIENT_COUNT 11
#define FILTER_IIR_FILTER_COUNT 10
///FILTER COEFIECIENTS

//decimation
#define DECIMATION_RATE 10
//Initial Fill value
#define FILTER_FILL_VALUE 0.0
//filter offset
#define FILTER_OFFSET 1


//power value
#define FILTER_POWER_EXPONENT 2
double currentPowerValues[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
double oldestPowerValues[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//////////////////////Queues
//Queue Sizes
#define X_QUEUE_SIZE 81
#define Y_QUEUE_SIZE 11
#define Z_QUEUE_SIZE 10
#define OUTPUT_QUEUE_SIZE 2000

//Queues
static queue_t xQueue;
static queue_t yQueue;

static queue_t zQueues[FILTER_IIR_FILTER_COUNT];
static queue_t outputQueues[FILTER_IIR_FILTER_COUNT];


//test helpers
#define FILTER_POWER_TEST_FILTER 3
#define FILTER_POWER_TEST_FILTER_COMPARE 2

#define FILTER_POWER_TEST_TWO 2
#define FILTER_POWER_TEST_ONE 1
#define FILTER_POWER_TEST_THREE 3



//FIR filter
const static double firCoefficients[FIR_FILTER_TAP_COUNT] = {
2.3387232094716393e-06,
8.5315251331824729e-06,
2.9482243115186545e-05,
6.6995669314761429e-05,
1.1143517823294958e-04,
1.3627702838176129e-04,
9.4984200531769315e-05,
-7.5261293958915054e-05,
-4.3856955309036282e-04,
-1.0321583857928057e-03,
-1.8278069361237518e-03,
-2.6919685676468248e-03,
-3.3593874332169672e-03,
-3.4381761329037928e-03,
-2.4624168642370875e-03,
3.2269212105893229e-18,
4.1911971510320634e-03,
9.9842426300420974e-03,
1.6724744159297936e-02,
2.3148568432119904e-02,
2.7431184779580609e-02,
2.7408407492086048e-02,
2.0976409165389766e-02,
6.6348991805864552e-03,
-1.5910123533234607e-02,
-4.5206951506442723e-02,
-7.7723915635271729e-02,
-1.0785111737871510e-01,
-1.2832426138482328e-01,
-1.3108242080517335e-01,
-1.0847998641886621e-01,
-5.4682785771912171e-02,
3.2989411400510635e-02,
1.5303826270370072e-01,
2.9931807467498528e-01,
4.6128131253989058e-01,
6.2497577759752210e-01,
7.7469057316290069e-01,
8.9501700149790175e-01,
9.7299725966515915e-01,
1.0000000000000000e+00,
9.7299725966515915e-01,
8.9501700149790120e-01,
7.7469057316290124e-01,
6.2497577759752232e-01,
4.6128131253989058e-01,
2.9931807467498533e-01,
1.5303826270370069e-01,
3.2989411400510163e-02,
-5.4682785771912219e-02,
-1.0847998641886622e-01,
-1.3108242080517346e-01,
-1.2832426138482331e-01,
-1.0785111737871515e-01,
-7.7723915635271743e-02,
-4.5206951506442702e-02,
-1.5910123533234725e-02,
6.6348991805863702e-03,
2.0976409165389728e-02,
2.7408407492086107e-02,
2.7431184779580630e-02,
2.3148568432119876e-02,
1.6724744159297939e-02,
9.9842426300420888e-03,
4.1911971510320651e-03,
-2.7977523840435701e-17,
-2.4624168642370793e-03,
-3.4381761329038002e-03,
-3.3593874332169741e-03,
-2.6919685676468209e-03,
-1.8278069361237577e-03,
-1.0321583857928049e-03,
-4.3856955309036288e-04,
-7.5261293958916897e-05,
9.4984200531769952e-05,
1.3627702838176192e-04,
1.1143517823294634e-04,
6.6995669314758136e-05,
2.9482243115188449e-05,
8.5315251331821409e-06,
2.3387232094716393e-06};

//the iir a Coefficients that we will use to calculate our values
const static double iirACoefficientConstants[FILTER_FREQUENCY_COUNT][IIR_A_COEFFICIENT_COUNT] = {
{-5.9047104265069450e+00, 1.8744369762834090e+01, -3.9138014487159964e+01, 5.9096571757477790e+01, -6.6562077721040964e+01, 5.6741148047249325e+01, -3.6080288397640032e+01, 1.6591168846245445e+01, -5.0180940564712042e+00, 8.1597668002427848e-01},
{-4.5918642672579653e+00, 1.3231643103100250e+01, -2.5372334871154358e+01, 3.7052229059172362e+01, -4.0906824214757449e+01, 3.5575496843443304e+01, -2.3390138680410708e+01, 1.1711742396745455e+01, -3.9023771062861456e+00, 8.1597668002428358e-01},
{-3.0288356503796354e+00, 8.4666002500905364e+00, -1.3848723384192677e+01, 2.0447433195696203e+01, -2.1089951766333307e+01, 1.9632551715352150e+01, -1.2766843818676275e+01, 7.4941060862414526e+00, -2.5740436155798787e+00, 8.1597668002427892e-01},
{-1.3932389482246779e+00, 5.5732080156112014e+00, -5.5638930353094285e+00, 1.1473194149423316e+01, -8.1171293533470994e+00, 1.1016006867180515e+01, -5.1292490791236984e+00, 4.9331057919917676e+00, -1.1840384337808019e+00, 8.1597668002427959e-01},
{8.1198710318235146e-01, 5.0604381679831327e+00, 3.1593769430257215e+00, 9.9701351546828612e+00, 4.5699337253828300e+00, 9.5728557330326929e+00, 2.9125722928492843e+00, 4.4792427743860248e+00, 6.9006392559390606e-01, 8.1597668002428414e-01},
{2.6812674200407520e+00, 7.6726577676710246e+00, 1.1833697589443165e+01, 1.7901539287865809e+01, 1.7824359318204525e+01, 1.7188132609908426e+01, 1.0909241915432363e+01, 6.7913714528710409e+00, 2.2786641735919870e+00, 8.1597668002428347e-01},
{4.8989811224568758e+00, 1.4397683432058708e+01, 2.8212025454676496e+01, 4.1461477784067185e+01, 4.6042887114628883e+01, 3.9808994126716286e+01, 2.6007963605435883e+01, 1.2743828399460146e+01, 4.1633791122096389e+00, 8.1597668002427903e-01},
{6.1090828056506696e+00, 1.9726586599709677e+01, 4.1692985671745063e+01, 6.3343372159185591e+01, 7.1517241261433185e+01, 6.0818667683556583e+01, 3.8435633273391709e+01, 1.7460548938678084e+01, 5.1917790887607245e+00, 8.1597668002428270e-01},
{7.3359132993024634e+00, 2.6325163213323780e+01, 5.9749112198106246e+01, 9.4375712094960846e+01, 1.0800265485372331e+02, 9.0614016064511588e+01, 5.5080971679768609e+01, 2.3301083259506363e+01, 6.2343959766024950e+00, 8.1597668002427737e-01},
{8.4893898596691546e+00, 3.3627566817021453e+01, 8.1545022824819085e+01, 1.3379567242752114e+02, 1.5504223931021414e+02, 1.2846260855445644e+02, 7.5173865235407575e+01, 2.9764589159239815e+01, 7.2146733236288707e+00, 8.1597668002428370e-01}
};

//the iir b Coefficients that we will use to calculate our values
const static double iirBCoefficientConstants[FILTER_FREQUENCY_COUNT][IIR_B_COEFFICIENT_COUNT] = {
{2.7688712213941621e-08, -0.0000000000000000e+00, -1.3844356106970810e-07, -0.0000000000000000e+00, 2.7688712213941620e-07, -0.0000000000000000e+00, -2.7688712213941620e-07, -0.0000000000000000e+00, 1.3844356106970810e-07, -0.0000000000000000e+00, -2.7688712213941621e-08},
{2.7688712800132612e-08, 0.0000000000000000e+00, -1.3844356400066304e-07, 0.0000000000000000e+00, 2.7688712800132608e-07, 0.0000000000000000e+00, -2.7688712800132608e-07, 0.0000000000000000e+00, 1.3844356400066304e-07, 0.0000000000000000e+00, -2.7688712800132612e-08},
{2.7688714316859953e-08, 0.0000000000000000e+00, -1.3844357158429979e-07, 0.0000000000000000e+00, 2.7688714316859958e-07, 0.0000000000000000e+00, -2.7688714316859958e-07, 0.0000000000000000e+00, 1.3844357158429979e-07, 0.0000000000000000e+00, -2.7688714316859953e-08},
{2.7688714030813837e-08, 0.0000000000000000e+00, -1.3844357015406917e-07, 0.0000000000000000e+00, 2.7688714030813835e-07, 0.0000000000000000e+00, -2.7688714030813835e-07, 0.0000000000000000e+00, 1.3844357015406917e-07, 0.0000000000000000e+00, -2.7688714030813837e-08},
{2.7688714056502999e-08, 0.0000000000000000e+00, -1.3844357028251500e-07, 0.0000000000000000e+00, 2.7688714056502999e-07, 0.0000000000000000e+00, -2.7688714056502999e-07, 0.0000000000000000e+00, 1.3844357028251500e-07, 0.0000000000000000e+00, -2.7688714056502999e-08},
{2.7688714287376903e-08, -0.0000000000000000e+00, -1.3844357143688450e-07, -0.0000000000000000e+00, 2.7688714287376901e-07, -0.0000000000000000e+00, -2.7688714287376901e-07, -0.0000000000000000e+00, 1.3844357143688450e-07, -0.0000000000000000e+00, -2.7688714287376903e-08},
{2.7688717722446196e-08, -0.0000000000000000e+00, -1.3844358861223097e-07, -0.0000000000000000e+00, 2.7688717722446195e-07, -0.0000000000000000e+00, -2.7688717722446195e-07, -0.0000000000000000e+00, 1.3844358861223097e-07, -0.0000000000000000e+00, -2.7688717722446196e-08},
{2.7688722037552109e-08, 0.0000000000000000e+00, -1.3844361018776056e-07, 0.0000000000000000e+00, 2.7688722037552111e-07, 0.0000000000000000e+00, -2.7688722037552111e-07, 0.0000000000000000e+00, 1.3844361018776056e-07, 0.0000000000000000e+00, -2.7688722037552109e-08},
{2.7688696872724591e-08, 0.0000000000000000e+00, -1.3844348436362296e-07, 0.0000000000000000e+00, 2.7688696872724592e-07, 0.0000000000000000e+00, -2.7688696872724592e-07, 0.0000000000000000e+00, 1.3844348436362296e-07, 0.0000000000000000e+00, -2.7688696872724591e-08},
{2.7688715722625518e-08, 0.0000000000000000e+00, -1.3844357861312760e-07, 0.0000000000000000e+00, 2.7688715722625520e-07, 0.0000000000000000e+00, -2.7688715722625520e-07, 0.0000000000000000e+00, 1.3844357861312760e-07, 0.0000000000000000e+00, -2.7688715722625518e-08}
};



//initialize helper function
void filter_initZQueues() {
    //loop through for each iir filter count
    for (int i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
        //Initialize a new queue
        queue_init(&(zQueues[i]), Z_QUEUE_SIZE);
        //push generic zeros not each queue
        for (int j = 0; j < Z_QUEUE_SIZE; j++)
            queue_overwritePush(&(zQueues[i]), FILTER_FILL_VALUE);
    }
}

//initialize helper function
void filter_initOutputQueues() {
    //loop throug for each iir filter count
    for (int i = 0; i < FILTER_IIR_FILTER_COUNT; i++) {
        //Initialize a new queue
        queue_init(&(outputQueues[i]), OUTPUT_QUEUE_SIZE);
        //push generic zeros not each queue
        for (int j = 0; j < OUTPUT_QUEUE_SIZE; j++)
            queue_overwritePush(&(outputQueues[i]), FILTER_FILL_VALUE);
    }
}

// Must call this prior to using any filter functions.
void filter_init() {
    //initialize xQueue
    queue_init(&xQueue, X_QUEUE_SIZE);
    //fill with zeros
    filter_fillQueue(&xQueue, FILTER_FILL_VALUE);
    //initialize y Queue
    queue_init(&yQueue, Y_QUEUE_SIZE);
    //fil with zeros
    filter_fillQueue(&yQueue, FILTER_FILL_VALUE);
    //initialize z Queues
    filter_initZQueues();
    //initialize output Queues
    filter_initOutputQueues();
}

// Use this to copy an input into the input queue of the FIR-filter (xQueue).
void filter_addNewInput(double x) {
    //overwrite push onto the x Queue
    queue_overwritePush(&xQueue, x);
}

// Fills a queue with the given fillValue.
void filter_fillQueue(queue_t* q, double fillValue) {
    //get the size of the queue in question and overwrite push fill value
    for (uint32_t i = 0; i < q->size; i++) {
        //overwrite the value onto the queue that has been passed in
        queue_overwritePush(q, fillValue);
    }
}

// Invokes the FIR-filter. Input is contents of xQueue.
// Output is returned and is also pushed on to yQueue.
double filter_firFilter() {
    //create a variable y
    double y = FILTER_FILL_VALUE;
    //loop through each filter
    for (int i = 0; i < FIR_FILTER_TAP_COUNT; i++) {
        //add filter_value * queue to y
        y += queue_readElementAt(&xQueue, FIR_FILTER_TAP_COUNT - FILTER_OFFSET - i)
                * firCoefficients[i]; // iteratively adds the (b * input) products.
    }
    //push value to y Queue
    queue_overwritePush(&yQueue, y);
    //return the y value
    return y;
}

// Use this to invoke a single iir filter. Input comes from yQueue.
// Output is returned and is also pushed onto zQueue[filterNumber].
double filter_iirFilter(uint16_t filterNumber) {
    // create our a and b values
    double a = FILTER_FILL_VALUE;
    double b = FILTER_FILL_VALUE;
    //loop through our y queue
    for (int i = 0; i < Y_QUEUE_SIZE; i++) {
        //add and multiply the y queue value and filter coefficient
        b += queue_readElementAt(&yQueue, Y_QUEUE_SIZE - FILTER_OFFSET - i) * iirBCoefficientConstants[filterNumber][i]; // iteratively adds the (b * input) products.
    }
    //loop through our z queue
    for (int i = 0; i < Z_QUEUE_SIZE; i++) {
        //add and multiply the z queue value and filter coefficient
        a += ((queue_readElementAt(&zQueues[filterNumber], Z_QUEUE_SIZE - FILTER_OFFSET - i)* iirACoefficientConstants[filterNumber][i])); // iteratively adds the (b * input) products.
    }
    //push value onto the z queue
    queue_overwritePush(&zQueues[filterNumber], (b - a));
    //push value onto the output queue
    queue_overwritePush(&outputQueues[filterNumber], (b - a));
    //return calculated value
    return (b - a);
}

// Use this to compute the power for values contained in a queue.
// If force == true, then recompute everything from scratch.
double filter_computePower(uint16_t filterNumber, bool forceComputeFromScratch,
        bool debugPrint) {

    double power = 0;
    //go to that filter and get all the values, square and add
    if(forceComputeFromScratch)
    {
        //loop through all the values in the queue
        for(int i = 0; i < OUTPUT_QUEUE_SIZE; i++)
        {
            //square and add the power
            power += pow(queue_readElementAt(&outputQueues[filterNumber], i), FILTER_POWER_EXPONENT) ;
        }
    }
    else
    {
        //subtract oldest power value and add newest to previous lab
        power = currentPowerValues[filterNumber] - oldestPowerValues[filterNumber] + pow(queue_readElementAt(&outputQueues[filterNumber], OUTPUT_QUEUE_SIZE - FILTER_OFFSET), FILTER_POWER_EXPONENT);
    }
    //store the new oldest value, is initially 0
    oldestPowerValues[filterNumber] = pow(queue_readElementAt(&outputQueues[filterNumber], 0), FILTER_POWER_EXPONENT);
    //set the new current power values
    currentPowerValues[filterNumber] = power;
    //return 0 for now
    return power;
}

// Returns the last-computed output power value for the IIR filter [filterNumber].
double filter_getCurrentPowerValue(uint16_t filterNumber) {
    //return last computed power value
    return currentPowerValues[filterNumber];
}

// Get a copy of the current power values.
void filter_getCurrentPowerValues(double* powerValues) {
    //do nothing for now
    //loop through current power values and add to powerValues as an array
    for(int i = 0; i < FILTER_IIR_FILTER_COUNT; i ++)
    {
        //set all power values into here
        powerValues[i] = currentPowerValues[i];
    }
    return;
}

// Uses the last computed-power values, scales them to the provided lowerBound and upperBound args, returns the index of element containing the max value.
// The caller provides the normalizedArray that will contain the normalized values. indexOfMaxValue indicates the channel with max. power.
void filter_getNormalizedPowerValues(double normalizedArray[],
        uint16_t* indexOfMaxValue) {
    //loop through the whole thing
    uint16_t max = 0;
    for(uint16_t i = 0;  i < FILTER_IIR_FILTER_COUNT; i++)
    {
        //loop through and compare the different arrays
        if(currentPowerValues[i] > currentPowerValues[max])
        {
            //if i is greater than current max set max to i
            max = i;
        }
    }
    *indexOfMaxValue = max;
    for(uint16_t i = 0;  i < FILTER_IIR_FILTER_COUNT; i++)
    {
        //loop through and compare the different arrays

        //normalize everything
            normalizedArray[i] /= normalizedArray[max];

    }

    //exit
    return;
}

/*********************************************************************************************************
 ********************************** Verification-assisting functions. *************************************
 ********* Test functions access the internal data structures of the filter.c via these functions. ********
 *********************** These functions are not used by the main filter functions. ***********************
 **********************************************************************************************************/

// Returns the array of FIR coefficients.
const double* filter_getFirCoefficientArray() {
    //return firCoefficients
    return firCoefficients;
}

// Returns the number of FIR coefficients.
uint32_t filter_getFirCoefficientCount() {
    //return FIR_FILTER_TAP_COUNT
    return FIR_FILTER_TAP_COUNT;
}

// Returns the array of a-coefficients for a particular filter number.
const double* filter_getIirACoefficientArray(uint16_t filterNumber) {
    //return iirACoefficientConstants at given filter number
    return iirACoefficientConstants[filterNumber];
}

// Returns the number of a-coefficients.
uint32_t filter_getIirACoefficientCount() {
    //return IIR_A_COEFFICIENT_COUNT
    return IIR_A_COEFFICIENT_COUNT;
}

// Returns the array of b-coefficients for a particular filter number.
const double* filter_getIirBCoefficientArray(uint16_t filterNumber) {
    //return iirBCoefficientConstants at given fitler number
    return iirBCoefficientConstants[filterNumber];
}

// Returns the number of b-coefficients.
uint32_t filter_getIirBCoefficientCount() {
    //return IIR_B_COEFFICIENT_COUNT
    return IIR_B_COEFFICIENT_COUNT;
}

// Returns the size of the yQueue.
uint32_t filter_getYQueueSize() {
    //return Y_QUEUE_SIZE
    return Y_QUEUE_SIZE;
}

// Returns the decimation value.
uint16_t filter_getDecimationValue()
{
    //return DECIMATION_RATE
    return DECIMATION_RATE;
}

// Returns the address of xQueue.
queue_t* filter_getXQueue() {
    //return x queue
    return &xQueue;
}

// Returns the address of yQueue.
queue_t* filter_getYQueue() {
    //return y queue
    return &yQueue;
}

// Returns the address of zQueue for a specific filter number.
queue_t* filter_getZQueue(uint16_t filterNumber) {
    //return z queue at filter number
    return &zQueues[filterNumber];
}

// Returns the address of the IIR output-queue for a specific filter-number.
queue_t* filter_getIirOutputQueue(uint16_t filterNumber) {
    //return outpout queuees at filter number
    return &outputQueues[filterNumber];
}

//runs a custom test for the power
void filter_runPowerTest()
{
    //say what we are starting
    printf("running Power Test\n\r");
    //initialize the filters
    filter_init();


    //so first compute the power to make sure that it is empty and reports zero
    //fill our output queue directly
    filter_fillQueue(&outputQueues[FILTER_POWER_TEST_FILTER], 0);
    //should expect 0 as our value, compute from scratch
    filter_computePower(FILTER_POWER_TEST_FILTER, true, false);

    double initialPower = filter_getCurrentPowerValue(FILTER_POWER_TEST_FILTER);
    //add in 2 values
    printf("Power value should be Zero is : %f\n\r", initialPower);

    //now we fill the queue manually with 2s and compute on the fly
    for(int i = 0 ; i < 2000; i++)
    {
        //push a two on the queue and compute manually
        queue_overwritePush(&outputQueues[FILTER_POWER_TEST_FILTER], FILTER_POWER_TEST_TWO);
        filter_computePower(FILTER_POWER_TEST_FILTER, false, false);
        //show that is being computed on the fly
    }

    //should be 8000
    printf("Power value should be 8000 is : %f\n\r", filter_getCurrentPowerValue(FILTER_POWER_TEST_FILTER));
    //now compare with compute from scratch should be the same
    filter_computePower(FILTER_POWER_TEST_FILTER, true, false);
    printf("Power value should be 8000 is : %f\n\r", filter_getCurrentPowerValue(FILTER_POWER_TEST_FILTER));

   //to be robust fill with ones and do the same thing

    //now we fill the queue manually with 2s and compute on the fly
        for(int i = 0 ; i < OUTPUT_QUEUE_SIZE; i++)
        {
            //push a two on the queue and compute manually
            queue_overwritePush(&outputQueues[FILTER_POWER_TEST_FILTER], FILTER_POWER_TEST_ONE);
            //compute the power
            filter_computePower(FILTER_POWER_TEST_FILTER, false, false);

        }

        //should be 8000
        printf("Power value should be 2000 is : %f\n\r", filter_getCurrentPowerValue(FILTER_POWER_TEST_FILTER));
        //now compare with compute from scratch should be the same
        filter_computePower(FILTER_POWER_TEST_FILTER, true, false);
        //tell us what is happening
        printf("Power value should be 2000 is : %f\n\r", filter_getCurrentPowerValue(FILTER_POWER_TEST_FILTER));


       //now we want to make sure that the highest player is shown

        for(int i = 0 ; i < OUTPUT_QUEUE_SIZE; i++)
           {
               //push a two on the queue and compute manually
               queue_overwritePush(&outputQueues[FILTER_POWER_TEST_FILTER_COMPARE], FILTER_POWER_TEST_THREE);
               filter_computePower(FILTER_POWER_TEST_FILTER_COMPARE, false, false);
               //show that is being computed on the fly
               //printf("Power value should be increasing is : %f\n\r", filter_getCurrentPowerValue(FILTER_POWER_TEST_FILTER));
           }

            //I should have the power for two queues now
            uint16_t highestPlayer = 0;
            //if they were all the same the power values pass the test
            filter_getNormalizedPowerValues(currentPowerValues, &highestPlayer);
            //print out the highest player
           printf(" highest player is : %d \n\r", highestPlayer);
}

//this will force a value into the power queue
void filter_forcePower(uint16_t filterNumber, double value)
{
    currentPowerValues[filterNumber] = value;
}
