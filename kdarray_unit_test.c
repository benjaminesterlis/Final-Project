#include <stdio.h>
#include <malloc.h>
#include "SPPoint.h"
#include "KDArray.h"

#define POINT_NUM 1
#define DIM 2
int printKDArray(KDArray* arr){
	int i,j;
	SPPoint* cer;
	printf("%d\n", arr->size);
	for ( j = 0; j < arr->size; ++j){
		printf("%d\n", j);
		cer = arr->copied_arr[j];
		printf("%s\n", "hello");
		printf("%d\t", spPointGetIndex(cer));
	}
		
	
	printf("\n");
	for( i=0;i<spPointGetDimension(arr->copied_arr[0]);i++){
		for ( j = 0; j < arr->size ; ++j){
			printf("%d\t", arr->mat[i][j]);
		}
		printf("\n");
	}
	
	return 1;
}

int main(int argc, char const *argv[])
{
	
	double data[POINT_NUM][DIM] = {{1,2}};//, {123,70}, {2,7}, {9,11}, {3,4}};
	int i;

	SPPoint** p_arr = (SPPoint**)malloc(POINT_NUM*sizeof(SPPoint**));
	for ( i = 0; i < POINT_NUM; ++i){
		p_arr[i] = spPointCreate(data[i],DIM,97+i);
	}
	printf("%s\n", "Start_init");
	KDArray* kdarr;
	i = Init(p_arr,POINT_NUM, kdarr);
	if (i == -1){
		printf("Error at init\n");
		return -1;
	}
	printf("%s\n", "Done_init");
	printKDArray(kdarr);
	KDArray* left;
	KDArray* right;
	i = split(kdarr, 0, left, right);
	if( i == -1){
		printf("Error at split\n");
		return -1;
	}
	printf("\n");
	printKDArray(left);
	printf("\n");
	printKDArray(right);

	printf("hello\n");
	return 0;
}



