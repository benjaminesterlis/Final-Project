#include "KNN_Search.h"
#include "SPBPriorityQueue.h"
#include <stdio.h>

#define SEND_ERROR(error) { printf("Error!, line: %d, %s\n", __LINE__, error); return false;}  

#define POW(x,y) (x-y)*(x-y)
#define Condition_0(bpq) (spBPQueueIsFull(bpq) == 0 )
#define Condition_1(bpq, curr, p) (POW(*getKDTreeVal(curr), spPointGetAxisCoor(p, getKDTreeDim(curr))) <= spBPQueueMaxValue(bpq))
#define Condition_2(bpq, curr, p) (Condition_0(bpq) || Condition_1(bpq, curr, p))
#define LeftSize(curr, bpq, p) \
	if (kNearestNeighbors_Recursive(getKDTreeLeft(curr), bpq, p) != 0) \
		return -1;
#define RightSize(curr, bpq, p) \
	if (kNearestNeighbors_Recursive(getKDTreeRight(curr), bpq, p) != 0) \
		return -1;

int printSPPoint (SPPoint* p)
{
	printf("******************** Start printf SPPoint ********************\n");
	int i = 0;
	if( p == NULL)
		SEND_ERROR("pointer is null");
	printf("point Dim: %d\n",spPointGetDimension(p));
	printf("point index :%d\n", spPointGetIndex(p));
	for (i = 1; i < spPointGetDimension(p) + 1; i++)
		printf("coordinate no: %d value is: %f\n", i, spPointGetAxisCoor(p, i));

	printf("******************** End printf SPPoint ********************\n");

	return 0;
}


void printKDTree(KDTreeNode* arr, int side, int depth)
{
	if (arr == NULL)
		return;

	if (0 == side)
		printf("********************* Start KDTree is %s with depth of %d *********************\n", "root", depth);
	else if (1 == side)
		printf("********************* Start KDTree is %s with depth of %d *********************\n", "right", depth);
	else
		printf("********************* Start KDTree is %s with depth of %d *********************\n", "left", depth);

	if(getKDTreeData(arr) != NULL) /* not leaf */
	{
		printSPPoint(getKDTreeData(arr));
	}
	else
	{
		printf("Dim is: %d\n", getKDTreeDim(arr));
		printf("Val is: %lf\n", *getKDTreeVal(arr));
		printKDTree(getKDTreeLeft(arr), -1, depth + 1);
		printKDTree(getKDTreeRight(arr), 1, depth + 1);
	}
	
	if (0 == side)
		printf("********************* End KDTree is %s with depth of %d *********************\n", "root", depth);
	else if( 1 == side)
		printf("********************* End KDTree is %s with depth of %d *********************\n", "right", depth);
	else
		printf("********************* End KDTree is %s with depth of %d *********************\n", "left", depth);
}


void printKDTreeNode(KDTreeNode* arr)
{
	printf("~~~~~~~~~~~~ Start Node ~~~~~~~~~~~~\n");
	if (arr->data == NULL)
	{
	printf("Dim is: %d\n", arr->dim);
	printf("Val is: %lf\n", *arr->val);
	}
	else 
		printSPPoint(arr->data);
	printf("~~~~~~~~~~~~ End Node ~~~~~~~~~~~~\n");
} 

static
int kNearestNeighbors_Recursive(KDTreeNode* curr, SPBPQueue** bpq, SPPoint* p)
{
	SP_BPQUEUE_MSG msg;
	BPQueueElement q;
	SPPoint* curr_p; 
	if (curr == NULL)
		return -1;

	// check if curr is leaf
	if ((curr_p = getKDTreeData(curr)) != NULL)
	{
		msg = spBPQueueEnqueue(*bpq, spPointGetIndex(curr_p) ,spPointL2SquaredDistance(curr_p, p));
		spBPQueuePeek(*bpq ,&q);
		return (msg == SP_BPQUEUE_SUCCESS) ? 0 : -1;
	}
	if (spPointGetAxisCoor(p, getKDTreeDim(curr)) <= *getKDTreeVal(curr)){
		LeftSize(curr, bpq, p)
		if(Condition_2(*bpq, curr, p))
			RightSize(curr, bpq, p)
	} else {
		RightSize(curr, bpq, p)
		if(Condition_2(*bpq, curr, p))
			LeftSize(curr, bpq, p)	
	}
	return 0;
}

int kNearestNeighbors(KDTreeNode* curr, SPBPQueue** bpq, SPPoint* p, int size)
{	
	*bpq = spBPQueueCreate(size);
	return kNearestNeighbors_Recursive(curr, bpq, p); 
}