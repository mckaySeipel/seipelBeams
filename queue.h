/*
* queue.h
*
*/

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>
#include <stdbool.h>

typedef uint32_t queue_index_t;
typedef double queue_data_t;
typedef uint32_t queue_size_t;

typedef struct {
	queue_index_t indexIn;
	queue_index_t indexOut;
	queue_size_t size;
	queue_size_t elementCount;
	queue_data_t * data;
} queue_t;

// Allocates the memory to you queue (the data* pointer) and initializes all parts of the data structure.
void queue_init(queue_t* q, queue_size_t size);

// Returns the size of the queue..
queue_size_t queue_size(queue_t* q);

// Returns true if the queue is full.
bool queue_full(queue_t* q);

// Returns true if the queue is empty.
bool queue_empty(queue_t* q);

// Pushes a new element into the queue. Reports an error if the queue is full.
void queue_push(queue_t* q, queue_data_t value);

// Removes the oldest element in the queue.
queue_data_t queue_pop(queue_t* q);

// If the queue is full, remove the oldest element and then push the new element.
void queue_overwritePush(queue_t* q, queue_data_t value);

// Provides random-access read capability to the queue.
// Low-valued indexes access older queue elements while higher-value indexes access newer elements
// (according to the order that they were added).
queue_data_t queue_readElementAt(queue_t* q, queue_index_t index);

// Returns a count of the elements currently contained in the queue.
queue_size_t queue_elementCount(queue_t* q);

// Frees the storage that you malloc'd before.
void queue_garbageCollect(queue_t* q);

// Prints the current contents of the queue. Handy for debugging.
// This must print out the contents of the queue in the order of oldest element first to newest element last.
void queue_print(queue_t* q);

// Performs a comprehensive test of all queue functions. Returns -1 if the test fails, 0 otherwise.
int16_t queue_runTest();

int16_t myTest();

#endif /* QUEUE_H_ */
