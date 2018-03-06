/*
* queue.c
*
*/

#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

// Standard queue implementation that leaves one spot empty so easier to check for full/empty.
void queue_init(queue_t* q, queue_size_t size) {
	q->indexIn = 0;
	q->indexOut = 0;
	q->elementCount = 0;
	q->size = size + 1;	// Add one additional location for the empty location.
	q->data = (queue_data_t *)malloc(q->size * sizeof(queue_data_t));
}

//gets the queue size
queue_size_t queue_size(queue_t* q)
{
	//return the queue size
	return q->size - 1;
}

// Returns true if the queue is full.
bool queue_full(queue_t* q)
{
	//if the queue size is greater than 1, and the index in and index out are
	//the same then it is full
	if (q->elementCount == q->size - 1)
	{
		//return true
		return true;
	}
	//return false
	return false;
}

// Pushes a new element into the queue. Reports an error if the queue is full.
void queue_push(queue_t* q, queue_data_t value)
{
	//checks to see if the queue is full
	if (queue_full(q))
	{
		//error
		//leave the method without doing anything
		return;
	}
	//add queue data to the queue
	q->data[q->indexIn] = value;
	//increment the element count
	q->elementCount++;

	//handles incrementing the queue value
	//if the index incremented one is greater than the last index of the array
	if (q->indexIn + 1 > q->size - 2)
	{
		//the index in wraps around to 0
		q->indexIn = 0;
	}
	else
	{
		//increment the index in by 1
		q->indexIn++;
	}
}

// Removes the oldest element in the queue.
queue_data_t queue_pop(queue_t* q)
{
	if (!queue_empty(q))
	{
		q->elementCount--;
		queue_data_t returnData = q->data[q->indexOut];

		//handles incrementing the queue value
		//if the index incremented one is greater than the last index of the array
		if (q->indexOut + 1 > q->size - 2)
		{
			//the index in wraps around to 0
			q->indexOut = 0;
		}
		else
		{
			//increment the index in by 1
			q->indexOut++;
		}



		return returnData;
	}
	return 0;
}

void queue_overwritePush(queue_t* q, queue_data_t value)
{
	if (queue_full(q))
	{
		queue_pop(q);
	}
	queue_push(q, value);
}

// Returns true if the queue is empty.
bool queue_empty(queue_t* q)
{
	//if the element count is zero and the index in = index out
	if (q->elementCount == 0 && q->indexIn == q->indexOut)
	{
		//return true
		return true;
	}
	//return false
	return false;
}

queue_data_t queue_readElementAt(queue_t* q, queue_index_t index)
{
	//set modifier to index in  plus the index passed in
	queue_index_t modifier = q->indexIn + index;
	//if the modifier is equal to or greater than the queue size -1
	if (modifier >= q->size - 1)
	{
		//adjust modifier by queue size
		modifier -= q->size - 1;
	}
	//return the value at modifier
	return q->data[modifier];
}

// Returns a count of the elements currently contained in the queue.
queue_size_t queue_elementCount(queue_t* q)
{
	//return the element count
	return q->elementCount;
}

// Prints the current contents of the queue. Handy for debugging.
// This must print out the contents of the queue in the order of oldest element first to newest element last.
void queue_print(queue_t* q)
{
	//get the start index
	queue_index_t index = q->indexIn;
	//loop through the number of elements
	for (uint16_t i = 0; i < q->elementCount; i++)
	{
	    //get the value
        queue_data_t value = q->data[index];
		//if the index plus i == the queue size modify it
        index++;

		if (index == q->size -1)
		{
			//reset index
			index = 0;
		}
		//print the value
		printf("data[%d]:%le\n\r", i, value);
	}
}



// Just free the malloc'd storage.
void queue_garbageCollect(queue_t* q) {
	free(q->data);
}

#define queueSize 4
#define testQeueueIteration 10
//my personal test on the queue
int16_t myTest()
{
    queue_t myQueue;
    //should have 4
        queue_init(&myQueue, queueSize);
        //iterate through
        for(uint16_t i = 0; i < testQeueueIteration; i++)
        {
            //overwrite push
            queue_overwritePush(&myQueue, i);
        }
        //should print 6, 7, 8, 9
        queue_print(&myQueue);
}




#define SMALL_QUEUE_SIZE 10
#define SMALL_QUEUE_COUNT 10
static queue_t smallQueue[SMALL_QUEUE_COUNT];
static queue_t largeQueue;

// smallQueue[SMALL_QUEUE_COUNT-1] contains newest value, smallQueue[0] contains oldest value.
// Thus smallQueue[0](0) contains oldest value. smallQueue[SMALL_QUEUE_COUNT-1](SMALL_QUEUE_SIZE-1) contains newest value.
// Presumes all queue come initialized full of something (probably zeros).
static double popAndPushFromChainOfSmallQueues(double input) {
	// Grab the oldest value from the oldest small queue before it is "pushed" off.
	double willBePoppedValue = queue_readElementAt(&(smallQueue[0]), 0);
	// Sequentially pop from the next newest queue and push into next oldest queue.
	for (int i = 0; i<SMALL_QUEUE_COUNT - 1; i++) {
		queue_overwritePush(&(smallQueue[i]), queue_pop(&(smallQueue[i + 1])));
	}
	queue_overwritePush(&(smallQueue[SMALL_QUEUE_COUNT - 1]), input);
	return willBePoppedValue;
}

static bool compareChainOfSmallQueuesWithLargeQueue(uint16_t iterationCount) {
	bool success = true;
	static uint16_t oldIterationCount;
	static bool firstPass = true;
	// Start comparing the oldest element in the chain of small queues, and the large queue
	// and move towards the newest values.
	for (uint16_t smallQIdx = 0; smallQIdx<SMALL_QUEUE_COUNT; smallQIdx++) {
		for (uint16_t smallQEltIdx = 0; smallQEltIdx<SMALL_QUEUE_SIZE; smallQEltIdx++) {
			double smallQElt = queue_readElementAt(&(smallQueue[smallQIdx]), smallQEltIdx);
			double largeQElt = queue_readElementAt(&largeQueue, (smallQIdx*SMALL_QUEUE_SIZE) + smallQEltIdx);
			if (smallQElt != largeQElt) {
				if (firstPass || (iterationCount != oldIterationCount)) {
					printf("Iteration:%d\n\r", iterationCount);
					oldIterationCount = iterationCount;
					firstPass = false;
				}
				printf("largeQ(%d):%lf", (smallQIdx*SMALL_QUEUE_SIZE) + smallQEltIdx, largeQElt);
				printf(" != ");
				printf("smallQ[%d](%d): %lf\n\r", smallQIdx, smallQEltIdx, smallQElt);
				success = false;
			}
		}
	}
	return success;
}

#define TEST_ITERATION_COUNT 105
#define FILLER 5
int16_t queue_runTest() {
	int success = 0;  // Be optimistic.
	// Let's make this a real torture test by testing queues against themselves.
	// Test the queue against an array to make sure there is agreement between the two.
	double testData[SMALL_QUEUE_SIZE + FILLER];
	queue_t q;
	queue_init(&q, SMALL_QUEUE_SIZE);
	// Generate test values and place the values in both the array and the queue.
	for (int i = 0; i<SMALL_QUEUE_SIZE + FILLER; i++) {
		double value = (double)rand() / (double)RAND_MAX;
		queue_overwritePush(&q, value);
		testData[i] = value;
	}
	// Everything is initialized, compare the contents of the queue against the array.
	for (int i = 0; i<SMALL_QUEUE_SIZE; i++) {
		double qValue = queue_readElementAt(&q, i);
		if (qValue != testData[i + FILLER]) {
			printf("testData[%d]:%lf != queue_readElementAt(&q, %d):%lf\n\r", i, testData[i + FILLER], i + FILLER, qValue);
			success = -1;
		}
	}
	if (success) {
		printf("Test 1 failed. Array contents not equal to queue contents.\n\r");
	}
	else {
		printf("Test 1 passed. Array contents match queue contents.\n\r");
	}
	success = 0;  // Remain optimistic.
	// Test 2: test a chain of 5 queues against a single large queue that is the same size as the cumulative 5 queues.
	for (int i = 0; i<SMALL_QUEUE_COUNT; i++)
		queue_init(&(smallQueue[i]), SMALL_QUEUE_SIZE);
	for (int i = 0; i<SMALL_QUEUE_COUNT; i++) {
		for (int j = 0; j<SMALL_QUEUE_SIZE; j++)
			queue_overwritePush(&(smallQueue[i]), 0.0);
	}
	queue_init(&largeQueue, SMALL_QUEUE_SIZE * SMALL_QUEUE_COUNT);
	for (int i = 0; i<SMALL_QUEUE_SIZE*SMALL_QUEUE_COUNT; i++)
		queue_overwritePush(&largeQueue, 0.0);
	for (int i = 0; i<TEST_ITERATION_COUNT; i++) {
		double newInput = (double)rand() / (double)RAND_MAX;
		popAndPushFromChainOfSmallQueues(newInput);
		queue_overwritePush(&largeQueue, newInput);
		if (!compareChainOfSmallQueuesWithLargeQueue(i)) {  // i is passed to print useful debugging messages.
			success = -1;
		}
	}

	if (!success)
		printf("Test 2 passed. Small chain of queues behaves identical to single large queue.\n\r");
	else
		printf("Test 2 failed. The content of the chained small queues does not match the contents of the large queue.\n\r");
	return success;
}
