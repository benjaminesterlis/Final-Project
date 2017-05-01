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
#define FREE_POINTS_ARRAY(i, size, arr) \
do { \
	for ( i= 0; i< size; i++) \
		spPointDestroy(arr[i]); \
	free(arr); \
} while(0)

#define CHECK_NOT(cond, error) CHECK_RET(!(cond), error)


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
	KDTreeNode* left;
	KDTreeNode* right;
	KDTreeNode* left_left;

	CHECK_NOT(KDTreeInit(kdarr, &root, MAX_SPREAD, 0), INIT_ERROR);

	left = root->left;
	right = root->right;
	left_left = left->left;

	ASSERT_TRUE(2 == root->dim);
	ASSERT_TRUE((double)48 == *root->val);
	ASSERT_TRUE(2 == left->dim);
	ASSERT_TRUE((double)10 == *left->val);
	ASSERT_TRUE(0 == left_left->dim);
	ASSERT_TRUE((double)3 == *left_left->val);
	ASSERT_TRUE(2 == right->dim);
	ASSERT_TRUE((double)70 == *right->val);


	KDTreeNodeDestroy(root);
	FREE_POINTS_ARRAY(i, POINT_NUM, p_arr);
	
	return true;
}

static bool KDTreeIncrementalTest()
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
	KDTreeNode* left;
	KDTreeNode* right;
	KDTreeNode* left_left;
	KDTreeNode* left_left_left;

	CHECK_NOT(KDTreeInit(kdarr, &root, MAX_SPREAD, 0), INIT_ERROR);

	left = root->left;
	right = root->right;
	left_left = left->left;
	left_left_left = left_left->left;

	ASSERT_TRUE(2 == root->dim);
	ASSERT_TRUE((double)48 == *(root->val));
	ASSERT_TRUE(2 == left->dim);
	ASSERT_TRUE((double)10 == *(left->val));
	ASSERT_TRUE(0 == left_left->dim);
	ASSERT_TRUE((double)3 == *(left_left->val));
	ASSERT_TRUE(2 == right->dim);
	ASSERT_TRUE((double)70 == *(right->val));


	KDTreeNodeDestroy(root);
	FREE_POINTS_ARRAY(i, POINT_NUM, p_arr);
}
int main(int argc, char const *argv[])
{
	RUN_TEST(KDTreeSpeardTest);
	RUN_TEST(KDTreeIncrementalTest);
	return 0;
}
