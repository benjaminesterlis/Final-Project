#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdbool.h>
#include "unit_test_util.h"
#include "../KDTree.h"

#define INIT_ERROR "KDTreeInit"
#define POINT_NUM 5
#define DIM 3
#define SEND_ERROR(error) { printf("Error!, line: %d, %s\n", __LINE__, error); return false;}  
#define CHECK_RET(cond, error) \
do { \
	if (!(cond)) \
		SEND_ERROR(error); \
} while(0)

#define CHECK_NOT(cond, error) CHECK_RET(!(cond), error)


int printSPPoint (SPPoint* p)
{
	printf("******************** Start printf kdarr ********************\n");
	int i = 0;
	if( p == NULL)
		SEND_ERROR("pointer is null");
	printf("point Dim: %d\n",spPointGetDimension(p));
	printf("point index :%d\n", spPointGetIndex(p));
	for (i = 1; i < spPointGetDimension(p) + 1; i++)
		printf("coordinate no: %d value is: %f\n", i, spPointGetAxisCoor(p, i));

	printf("******************** End printf kdarr ********************\n");

	return 0;
}

int printKDArray(KDArray* arr)
{
	printf("******************** Start printf kdarr ********************\n");
	int i,j;
	SPPoint* cer;

	printf("Size is: %d\n", GetKDArraySize(arr));
	printf("Dim is: %d\n", GetKDArrayDim(arr));
	
	for ( j = 0; j < arr->size; ++j){
		printf("no. %d\t", j);
		cer = arr->copied_arr[j];
		printf("%d\n", spPointGetIndex(cer));
	}
	printf("Dim: %d\n", spPointGetDimension(arr->copied_arr[0]));
	
	for( i = 0; i < spPointGetDimension(arr->copied_arr[0]); i++){
		for ( j = 0; j < arr->size ; ++j){
			printf("%d\t", arr->mat[i][j]);
		}
		printf("\n");
	}
	printf("******************** End printf kdarr ********************\n");
	return 1;
}

void printKDTree( KDTreeNode* root, int depth, int side)
{
	// to get side
	if (side == 0)
		printf("********************** Start Node at side %s depth: %d **********************\n", "left:", depth);
	else if (side == -1)
		printf("********************** Start Node at side %s depth: %d **********************\n", "root:", depth);

	else
		printf("********************** Start Node at side %s depth: %d **********************\n", "right:", depth);
	
	// to check if leaf or not
	if( getKDTreeData(root) != NULL){ /* not leaf */
		printSPPoint(getKDTreeData(root));
	}
	else {
		printf("dim:%d\n", getKDTreeDim(root));
		printf("val:%lf\n", *(getKDTreeVal(root)));
		printKDTree(getKDTreeLeft(root),depth + 1, 0);
		printKDTree(getKDTreeRight(root), depth + 1, 1);
	}

	// to get side
	if (side == 0)
		printf("********************** End Node at side %s depth: %d **********************\n", "left:", depth);
	else if (side == -1)
		printf("********************** End Node at side %s depth: %d **********************\n", "root:", depth);
	else
		printf("********************** End Node at side %s depth: %d **********************\n", "right:", depth);
	
}

static bool KDTreeSpeardTest()
{
	int i;
	double data[POINT_NUM][DIM] = { {0,2,70}, {3,1,10}, {13,9,7}, {903,124,1001}, {18,37,48} };
	/*********************** Created KDArray ***********************/	
	SPPoint** p_arr = (SPPoint**)malloc(sizeof(SPPoint*) * POINT_NUM);
	for ( i = 0; i < POINT_NUM; ++i){
		p_arr[i] = spPointCreate(data[i], DIM, 97 + i);
	};
	KDArray* kdarr = Init(p_arr,POINT_NUM);

	/***************************************************************/
	KDTreeNode* root;
	CHECK_NOT(KDTreeInit(kdarr, &root, MAX_SPREAD, 0), INIT_ERROR);

	printKDTree(root, 0, -1);
	KDTreeNodeDestroy(root);

	return true;
}

int main(int argc, char const *argv[])
{
	RUN_TEST(KDTreeSpeardTest);
	return 0;
}
