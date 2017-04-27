#include "KNN_Search.h"
#include "SPBPriorityQueue.h"
#include <stdio.h>


#define abs_minus(x,y) (x > y) ? (x - y) : (y - x)
#define Condition_0(bpq) (spBPQueueIsFull(bpq) == 1 )
#define Condition_1(bpq, curr, p) (abs_minus(*getKDTreeVal(curr), spPointGetAxisCoor(p, getKDTreeDim(curr))) <= spBPQueueMaxValue(bpq))
#define Condition_2(bpq, curr, p) (Condition_0(bpq) || Condition_1(bpq, curr, p))
#define LeftSize(curr, bpq, p) \
	if (kNearestNeighbors_Recursive(getKDTreeLeft(curr), bpq, p) != 0) \
		return -1;
#define RightSize(curr, bpq, p) \
	if (kNearestNeighbors_Recursive(getKDTreeRight(curr), bpq, p) != 0) \
		return -1;

static
int kNearestNeighbors_Recursive(KDTreeNode* curr, SPBPQueue* bpq, SPPoint* p)
{
	SP_BPQUEUE_MSG msg;
	SPPoint* curr_p; 
	if (curr == NULL)
		return -1;

	// check if curr is leaf
	if ((curr_p = getKDTreeData(curr)) != NULL)
	{
		msg = spBPQueueEnqueue(bpq, spPointGetIndex(curr_p) ,spPointL2SquaredDistance(curr_p, p));
		if (msg == SP_BPQUEUE_SUCCESS)
			return 0;
		return -1;
	}

	if (spPointGetAxisCoor(p, getKDTreeDim(curr)) <= *getKDTreeVal(curr)){
		LeftSize(curr, bpq, p)
		if(Condition_2(bpq, curr, p))
			RightSize(curr, bpq, p)
	} else {
		RightSize(curr, bpq, p)
		if(Condition_2(bpq, curr, p))
			LeftSize(curr, bpq, p)	
	}
	return 0;
}

int kNearestNeighbors(KDTreeNode* curr, SPBPQueue* bpq, SPPoint* p, int size)
{	
	bpq = spBPQueueCreate(size);
	return kNearestNeighbors_Recursive(curr, bpq, p); 
}