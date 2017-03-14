#include "SPBPriorityQueue.h"
#include <stdio.h>
#include <stdlib.h>

struct sp_bp_queue_t
{
	BPQueueElement *elements; // the elemnts will be showed from big to small.
	int size;
	int max_size;
};

#define InsertElemNext(source,i,ind,val) {\
	(source->elements[i]).index = ind;\
 	(source->elements[i]).value = val;\
}

#define CheckIfNULL(source,msg){\
	if ( (source) == NULL)\
		return (msg); \
}

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
SPBPQueue* spBPQueueCreate(int maxSize)
{
	if (maxSize <= 0 )
		return NULL;

	SPBPQueue *spq = (SPBPQueue *) malloc(sizeof(SPBPQueue));
	CheckIfNULL(spq,NULL)
	
	if ((spq->elements = (BPQueueElement*) malloc(sizeof(BPQueueElement) * maxSize)) == NULL)	
		return NULL;

	spq->size = 0;
	spq->max_size = maxSize;
	return spq;
}


/**
 * Copy and elements array from source to target
 *
 * @param target - the destination Queue to insert the elements array.
 * @param source - the source Queue which we copy elemnts from.
 */
void ElementCopy(SPBPQueue* target,SPBPQueue* source){
	int i;
	for (i = 0; i < source->size; ++i)
	{
		target->elements[i].index = source->elements[i].index;
		target->elements[i].value = source->elements[i].value;
	}
}

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
SPBPQueue* spBPQueueCopy(SPBPQueue* source)
{
	SPBPQueue *spcpy = malloc(sizeof(*spcpy));
	CheckIfNULL(spcpy,NULL)

	spcpy->size = source->size;
	spcpy->max_size = source->max_size;


	CheckIfNULL((spcpy->elements = (BPQueueElement*) malloc(sizeof(BPQueueElement) * source->max_size)) , NULL)
	ElementCopy(spcpy,source);

	return spcpy;
}

/**
 * Free all memory allocation associated with the Queue,
 * First, free all data from the cells.
 * Second, free the point to the arrary.
 * Third, free the Queue pointer.
 *
 * @param source - the Queue we want to free all memory aloocation associated.
 * 'Ignore' if source == NULL or queue is empty.
*/
void spBPQueueDestroy(SPBPQueue* source)
{
	if (source == NULL)
		return;

	if(source->elements == NULL)
		return; 	
	
	free(source->elements);
	free(source);
}

/**
 * Remove all elements in the Queue.
 *
 * @param source - the Queue need to clear.
 * 'Ignore' if source == NULL
 */
void spBPQueueClear(SPBPQueue* source)
{
	if ( source == NULL)
		return;
	source->size = 0;

}

/**
 * Give the size of the Queue.
 *
 * @param source - the queue need to check size.
 * @return the size of the queue,
 * In case NULL pointer return -1.
 */
int spBPQueueSize(SPBPQueue* source)
{
	CheckIfNULL(source,-1)
	return source->size;
}

/**
 * Give the upperbound of the elements to insert source queue.
 *
 * @param source - the queue need to check upperbound.
 * @return the upperbound of the array,
 * In case NULL pointer return -1.
 */
int spBPQueueGetMaxSize(SPBPQueue* source){
	CheckIfNULL(source,-1)
	return source->max_size;	
}

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
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue* source, int index, double value)
{
	if (index <0 || value < 0)
		return SP_BPQUEUE_INVALID_ARGUMENT;
	CheckIfNULL(source,SP_BPQUEUE_INVALID_ARGUMENT)

	// size == 0, jsut insert.
	if (source->size == 0)
	{	
		InsertElemNext(source,0,index,value)
		source->size++;
		return SP_BPQUEUE_SUCCESS;
	}
	int i;
	// size = max_size
	if(source->size == source->max_size)
	{	
		if (value > source->elements[0].value)
			return SP_BPQUEUE_FULL;
		
		if ((value == source->elements[0].value) && index >source->elements[0].index)
			return SP_BPQUEUE_FULL;
		
		for ( i = 1; i < source->size; i++ )
			InsertElemNext(source,i-1,source->elements[i].index,source->elements[i].value)
		source->size--;
	}
	//otherwise
	for ( i = source->size-1; i >= 0; i--)
	{
		if (value > source->elements[i].value)
			InsertElemNext(source,i+1,source->elements[i].index,source->elements[i].value)
		
		else if (value == source->elements[i].value)
		{
			if (index > source->elements[i].index)
				InsertElemNext(source,i+1,source->elements[i].index,source->elements[i].value)
			else
			{
				InsertElemNext(source,i+1,source->elements[i].index,source->elements[i].value)
				InsertElemNext(source,i,index,value)
				source->size++;
				return SP_BPQUEUE_SUCCESS;
			}
		}
		else
		{
			InsertElemNext(source,i+1,index,value)
			source->size++;
			return SP_BPQUEUE_SUCCESS;
		}
	}
	InsertElemNext(source,0,index,value)
	source->size++;
	return SP_BPQUEUE_SUCCESS;
}

/**
 * Remove the lowest value element form the queue.
 * 
 * @param source - the queue need to remove lowest value element.
 * @return a new SP_BPQUEUE_MSG object with the information on the Deuque.
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue* source)
{
	CheckIfNULL(source,SP_BPQUEUE_INVALID_ARGUMENT)
	if ( source->size == 0)
		return SP_BPQUEUE_EMPTY;
	source->size--;
	return SP_BPQUEUE_SUCCESS;
}

/**
 * Return a copy of the element with the lowest value.
 *
 * @param source - the queue need to copy the element with the lowest value. 
 * @param res - the pointer to the element with the lowest value.
 * @return a new SP_BPQUEUE_MSG object with the information on the Peek.
 *
 */
SP_BPQUEUE_MSG spBPQueuePeek(SPBPQueue* source, BPQueueElement* res)
{
	CheckIfNULL(source,SP_BPQUEUE_INVALID_ARGUMENT)
	CheckIfNULL(res,SP_BPQUEUE_INVALID_ARGUMENT)
	if (source->size == 0)
		return SP_BPQUEUE_EMPTY;
	res->index = source->elements[source->size -1].index;
	res->value = source->elements[source->size -1].value;
	return SP_BPQUEUE_SUCCESS;

}


/**
 * Return a copy of the element with the highest value.
 *
 * @param source - the queue need to copy the element with the highest value. 
 * @param res - the pointer to the element with the higest value.
 * @return a new SP_BPQUEUE_MSG object with the information on the Peek.
 *
 */
SP_BPQUEUE_MSG spBPQueuePeekLast(SPBPQueue* source, BPQueueElement* res)
{
	CheckIfNULL(source,SP_BPQUEUE_INVALID_ARGUMENT)
	CheckIfNULL(res,SP_BPQUEUE_INVALID_ARGUMENT)
	if (source->size == 0)
		return SP_BPQUEUE_EMPTY;
	res->index = source->elements[0].index;
	res->value = source->elements[0].value;
	return SP_BPQUEUE_SUCCESS;

}

/**
 * @param source - the queue to check.
 * @return the min value of the queue.
 * In case queue is empty or @param is a NULL pointer return -1.
 */
double spBPQueueMinValue(SPBPQueue* source)
{
	if (source == NULL || source->size == 0)
		return -1;
	return source->elements[source->size - 1].value;//the last element it the lowest.
}
/**
 * @param source - the queue to check.
 * @return the max value of the queue.
 * In case queue is empty or @param is a NULL pointer return -1.
 */
double spBPQueueMaxValue(SPBPQueue* source)
{
	if (source == NULL || source->size == 0)
		return -1;
	return source->elements[0].value;//the first element is the biggest.
}

/**
 * say whatever the Queue is empty or not.
 *
 * @param source - the Queue to check.
 * @return true in case Queue is empty or the Queue is a NULL pointer,
 *Otherwise, return false.
 */
bool spBPQueueIsEmpty(SPBPQueue* source)
{
	CheckIfNULL(source,true)
	return (source->size == 0);
}

/**
 * say whatever the Queue is full or not
 *
 * @param source - the Queue to check.
 * @return true in case Queue is full or the Queue is a NULL pointer,
 * Otherwise, return false.
 */
bool spBPQueueIsFull(SPBPQueue* source)
{
	CheckIfNULL(source,true)
	return (source->size == source->max_size);
}

void printq(SPBPQueue* source,int axis)
{
	int i;
	for ( i = source->size -1; i >= 0; i--)
	{
		switch (axis)
		{
			case 0:
				printf("%lf \t", source->elements[i].value);
				break;
			case 1:
				printf("%d", source->elements[i].index +1);
				break;
			case 2:
				printf("%lf \t", source->elements[i].value);
				printf("%d \n", source->elements[i].index +1);
				break;
		}
	}
	printf("\n");
}

int spBPQueueMinValueIndex(SPBPQueue* source)
{
	if (source == NULL || source->size == 0)
		return -1;
	return source->elements[source->size - 1].index;//the last element it the lowest.
}
