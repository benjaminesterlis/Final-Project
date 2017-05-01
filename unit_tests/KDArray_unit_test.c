#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../SPPoint.h"
#include "../KDArray.h"
#include "unit_test_util.h" 

#define POINT_NUM 6
#define DIM 5
#define SEND_ERROR(error) {printf("%s\n", (error)); exit(-1);}
#define FREE_KDARRAY_ARRAY(i, size, arr) \
do { \
	for ( i = 0; i < size; i++) \
		KDArrayDestroy(arr[i]); \
	free(arr); \
} while(0) 
#define FREE_POINTS_ARRAY(i, size, arr) \
do { \
	for ( i= 0; i< size; i++) \
		spPointDestroy(arr[i]); \
	free(arr); \
} while(0)

static bool KDArrayTest()
{

	double data[POINT_NUM][DIM] = { {1,2,3,4,5}, {123,70,50,40,72}, {2,7,19,5,46}, {9,11,13,15,58}, {3,4,5,6,23}, {15,46,72,93,19} };
	int i;

	SPPoint** p_arr = (SPPoint**)malloc(sizeof(SPPoint**) * POINT_NUM);
	for ( i = 0; i < POINT_NUM; ++i){
		p_arr[i] = spPointCreate(data[i], DIM, 97 + i);
	}
	printf("%s\n", "Start_init");
	KDArray* kdarr = Init(p_arr,POINT_NUM);
	printf("%s\n", "Done_init");
	// printKDArray(kdarr);
	KDArray* left;
	KDArray* right;
	KDArray** total;
	printf("%s\n", "Start_splits");
	total = split(kdarr, 0);
	left = total[0];
	right = total[1];
	printf("\n");
	// printKDArray(left);
	printf("\n");
	// printKDArray(right);
	KDArray* left2;
	KDArray* right2;
	KDArray** total2;
	total2 = split(right, 0);
	left2 = total2[0];
	right2 = total2[1];

	// Free section
	FREE_KDARRAY_ARRAY(i, 2, total);
	FREE_KDARRAY_ARRAY(i, 2, total2);
	KDArrayDestroy(kdarr);
	FREE_POINTS_ARRAY(i, POINT_NUM, p_arr); 

	return true;
}

int main(int argc, char const *argv[])
{
	RUN_TEST(KDArrayTest);
	return 0;
}
