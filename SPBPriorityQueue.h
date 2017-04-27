#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include <stdbool.h>

/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t SPBPQueue;

typedef struct sp_bpq_element_t {
	int index;
	double value;
} BPQueueElement;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocate a new Queue in the memory.
 * New SPBPQueue such that:
 *
 * - size = the current size of the queue, set to zero.
 * - elements = pointer to the first element in the queue.
 * - max_size = the upper bould of elements in the Queue.
 * 
 * @param  maxSize - int, which refer to the upperbound to no. of elements
 * @return spq - pointer to SPBPQueue
 * NULL in case of allocation failure.
 * NULL in case of non-positive maxSize.
 */
SPBPQueue* spBPQueueCreate(int maxSize);


/**
 * Copy and elements array from source to target
 *
 * @param target - the destination Queue to insert the elements array.
 * @param source - the source Queue which we copy elemnts from.
 */
void ElementCopy(SPBPQueue* target,SPBPQueue* source);

/**
 * Allocate a copu of the source SPBPQueue.
 * Copy the source SPBPQueue into a new Queue called spcpy such that:
 *
 * - is_empty(spcpy) = is_empty(source) (spcpy and source has the same flag for emptyness). 
 * - size(spcpy) = size(source) (spcpy and source have the same size).
 * - max_size(spcpy) = max_size(source) (spcpy and source have the same upper bound of elements).
 * - elements(spcpy) = elements(source) (spcpy and source have the same elements, which include the same index and value inside).
 *
 * @param source - the SPBPQueue we want to copy.
 * @return spcpy - copy of source.
 * NULL in case of allocation failure.  
 */
SPBPQueue* spBPQueueCopy(SPBPQueue* source);

/**
 * Free all memory allocation associated with the Queue,
 * First, free all data from the cells.
 * Second, free the point to the arrary.
 * Third, free the Queue pointer.
 *
 * @param source - the Queue we want to free all memory aloocation associated.
 * 'Ignore' if source == NULL or queue is empty.
*/
void spBPQueueDestroy(SPBPQueue* source);

/**
 * Remove all elements in the Queue.
 *
 * @param source - the Queue need to clear.
 * 'Ignore' if source == NULL
 */
void spBPQueueClear(SPBPQueue* source);

/**
 * Give the size of the Queue.
 *
 * @param source - the queue need to check size.
 * @return the size of the queue,
 * In case NULL pointer return -1.
 */
int spBPQueueSize(SPBPQueue* source);

/**
 * Give the upperbound of the elements to insert source queue.
 *
 * @param source - the queue need to check upperbound.
 * @return the upperbound of the array,
 * In case NULL pointer return -1.
 */
int spBPQueueGetMaxSize(SPBPQueue* source);

/**
 * Insert a new BPQueueElement (which will create) to the queue with the corret fields.
 * The Queue is ordinized from big to small,cell[0] is the biggest.
 * Indexing start from 1 to source->size.
 *
 * @param source - the queue.
 * @param index - the index given.
 * @param value - the value given.
 * @return a new SP_BPQUEUE_MSG object with the information on the insert.
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value);

/**
 * Remove the lowest value element form the queue.
 * 
 * @param source - the queue need to remove lowest value element.
 * @return a new SP_BPQUEUE_MSG object with the information on the Deuque.
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source);

/**
 * Return a copy of the element with the lowest value.
 *
 * @param source - the queue need to copy the element with the lowest value. 
 * @param res - the pointer to the element with the lowest value.
 * @return a new SP_BPQUEUE_MSG object with the information on the Peek.
 *
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res);

/**
 * Return a copy of the element with the highest value.
 *
 * @param source - the queue need to copy the element with the highest value. 
 * @param res - the pointer to the element with the higest value.
 * @return a new SP_BPQUEUE_MSG object with the information on the Peek.
 *
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res);

/**
 * @param source - the queue to check.
 * @return the min value of the queue.
 * In case queue is empty or @param is a NULL pointer return -1.
 */
double spBPQueueMinValue(SPBPQueue* source);

/**
 * @param source - the queue to check.
 * @return the max value of the queue.
 * In case queue is empty or @param is a NULL pointer return -1.
 */
double spBPQueueMaxValue(SPBPQueue* source);

/**
 * say whatever the Queue is empty or not.
 *
 * @param source - the Queue to check.
 * @return true in case Queue is empty or the Queue is a NULL pointer,
 *Otherwise, return false.
 */
bool spBPQueueIsEmpty(SPBPQueue* source);

/**
 * say whatever the Queue is full or not
 *
 * @param source - the Queue to check.
 * @return true in case Queue is full or the Queue is a NULL pointer,
 * Otherwise, return false.
 */
bool spBPQueueIsFull(SPBPQueue* source);

/**
 * our own method. 
 * print the info according to axis,
 * which can be 0,1 or 2.
 *
 * @param source - the queue need to be printed, assumed not NULL and well defined.
 * @axis - can be 0,1 or 2.
 * if axis is 0 print only values.
 * if axis is 1 print only index.
 * if axis is 2 print both. 
 */
void printq(SPBPQueue* source,int axis);

int spBPQueueMinValueIndex(SPBPQueue* source);

int spBPQueueElementGetIndex (BPQueueElement *elem);

#endif