#include <stdio.h>
#include <malloc.h>
#include <unistd.h>
#include <stdlib.h>
#include "SPPoint.h"
#include "KDArray.h"

#define POINT_NUM 5
#define DIM 2
#define SEND_ERROR(error) {printf("%s\n", (error)); exit(-1);}

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

int main(int argc, char const *argv[])
{
	// int i, j;
	// int* indexes;
	// SPPoint** p_arr;
	// int index = 100;
	// double data[POINT_NUM][DIM] = {{1,2,0}, {123,70,1}, {2,7,2}, {9,11,3}, {3,4,4}};
	
	// p_arr = (SPPoint**)malloc(sizeof(SPPoint*) * POINT_NUM);

	// for (i = 0; i <  POINT_NUM; i++)
	// 	p_arr[i] = spPointCreate(data[i], DIM, index + i);
	

	// indexes = spPointSortByIndex(p_arr, 0, POINT_NUM); 
	// for (i = 0; i < POINT_NUM; i++)
	// 	printf("%d\t", indexes[i]);

	// printf("\n");
	// for ( i = 0; i < POINT_NUM; ++i){	
	// 	spPointDestroy(p_arr[i]);
	// }
	// free(p_arr);

	// return 0;


	double data[POINT_NUM][DIM] = {{1,2}, {123,70}, {2,7}, {9,11}, {3,4}};
	int i;

	SPPoint** p_arr = (SPPoint**)malloc(POINT_NUM*sizeof(SPPoint**));
	for ( i = 0; i < POINT_NUM; ++i){
		p_arr[i] = spPointCreate(data[i],DIM,97+i);
	}
	printf("%s\n", "Start_init");
	KDArray* kdarr = Init(p_arr,POINT_NUM);
	printf("%s\n", "Done_init");
	//printKDArray(kdarr);
	KDArray* left;
	KDArray* right;
	KDArray** total;
	printf("%s\n", "Start_splits");
	total = split(kdarr, 0);
	left = total[0];
	right = total[1];
	printf("\n");
	printKDArray(left);
	printf("\n");
	printKDArray(right);


	// Free section
	for ( i = 0; i < 2; i++)
		KDArrayDestroy(total[i]);

	free(total);

	printf("hello\n");
	return 0;
}


