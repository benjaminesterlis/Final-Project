#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include "KDArray.h"


#define RND_HALF(num) (num % 2 == 0) ? (num / 2) : (num / 2 + 1)
#define SEND_ERROR(error) \
	printf("%d %s, Line: %d\n",errno, (error), __LINE__); \
	return -1;

#define SEND_ERROR_INIT(error) \
	// need first to free the SPPoints. \
	if (need_To_Free_Init[2] > 0) \
		for (Just4Free = 0; Just4Free < need_To_Free_Init[2]; Just4Free++) \
		spPointDestroy(kdarr->copied_arr[Just4Free]); \
	// free the copied SPPoint array a.k.a copied_arr. \
	if(need_To_Free_Init[3] == 1) \
			free(kdarr->copied_arr); \
	// need first to free the mat rows (int*) \
	if(need_To_Free_Init[4] > 0)	\
		for (Just4Free = 0; Just4Free < need_To_Free_Init[3]; Just4Free++) \
			free(kdarr->mat[Just4Free]); \
	// free the mat itself \
	if(need_To_Free_Init[5] == 1) \
		free(kdarr->mat); \
	// frre the memory allocated for kdarr pointer \
	if(need_To_Free_Init[0] == 1) \
		free(kdarr); \
	SEND_ERROR(error);

#define SEND_ERROR_SPLIT(error) \
	// To free the perm array \
	if (need_To_Free_Split[11] == 1) \ 
		free(perm); \
	// To free the perm_reverse array \
	if (need_To_Free_Split[10] == 1) \
		free(perm_reverse); \
	// To free the points in kdright->copied_arr. \
	if(need_To_Free_Split[9] > 0) \
		for (Just4Free = 0; Just4Free < need_To_Free_Split[9]; Just4Free++) \
			free(kdright->copied_arr[Just4Free]); \
	// To free the points in kdleft->copied_arr. \
	if(need_To_Free_Split[8] > 0 ) \
		for (Just4Free = 0; Just4Free < need_To_Free_Split[8]; Just4Free++) \
			free(kdleft->copied_arr[Just4Free]); \
	// To free rows in kdright->mat. \
	if(need_To_Free_Split[7] > 0) \
		for ( Just4Free = 0; Just4Free < need_To_Free_Split[7]; Just4Free++) \
			free(kdright->mat[Just4Free]); \
	// To free kdright->mat. \
	if(need_To_Free_Split[6] == 1) \
		free(kdright->mat); \
	// To free kdright->copied arr pointer. \
	if(need_To_Free_Split[5] == 1) \
		free(kdright->copied_arr); \
	// To free kdright pointer. \
	if(need_To_Free_Split[4] == 1) \
		free(kdright); \
	// To free rows in kdleft->mat. \
	if(need_To_Free_Split[3] > 0) \
		for (Just4Free = 0; Just4Free < need_To_Free_Split[3]; Just4Free++) \
			free(kdleft->mat[Just4Free]); \
	// To free kdleft->mat \
	if (need_To_Free_Split[2] == 1) \
		free(kdleft->mat); \
	// To free kdleft->copied_arr pointer. \
	if(need_To_Free_Split[1] == 1) \
		free(kdleft->copied_arr); \
	// To free kdleft pointer \
	if (need_To_Free_Split[0] == 1) \
		free(kdelft); \
	SEND_ERROR(error);


/*
 * need_To_Free_Split is an array which says what we need to free in the split method
 * cell 0 - kdleft pointer.
 * cell 1 - kdleft->copied_arr.
 * cell 2 - kdleft->mat.
 * cell 3 - how many rows in kdleft->mat to remove.
 * cell 4 - kdright pointer.
 * cell 5 - kdright->copied_arr.
 * cell 6 - kdright->mat.
 * cell 7 - how many rows in kdright->mat to remove.
 * cell 8 - how many points to free in kdleft->copied_arr.
 * cell 9 - how many points to free in kdright->copied_arr.
 * cell 10 - perm.
 * cell 11 - perm->reverse.
*/ 
int need_To_Free_Split[12] = {0};

/*
 * This array will tell me if we need to free any memory we used
 * cell 0 - kdarr
 * cell 1 - kdarr->copied_arr (SPoint**)
 * cell 2 - how many SPPoints need to free in kdarr->copied_arr;
 * cell 3 - mat (int**)
 * cell 4 - how many of rows in in mat need to be freed (between 0 to d)
*/
int need_To_Free_Init[5] = {0};

int Init(SPPoint** arr, int size, KDArray* kdarr)
{
	int Just4Free;	
	int* Sorted_Coor_i;
	int i, j;
	int dim;

	if((kdarr = (KDArray*)malloc(sizeof(KDArray))) == NULL)
		SEND_ERROR_INIT("Error, Allocation failure");
	need_To_Free_Init[0]++;

	dim = spPointGetDimension(arr[0]);
	kdarr->size = size;
	kdarr->dim = dim;

	if((kdarr->copied_arr = (SPPoint**)malloc(sizeof(SPPoint*) * size)) == NULL)
		SEND_ERROR_INIT("Error, Allocation failure");
	need_To_Free_Init[1]++;

	for (i = 0; i < size; i++)
	{	
		if((kdarr->copied_arr[i] = spPointCopy(arr[i])) == NULL)
			SEND_ERROR("Error, Allocation failure");
		// Append another Coordinate to each point with the its index in the permutation. 
		if((kdarr->copied_arr[i] = ExpendDim(kdarr->copied_arr[i],i)) == NULL)
			SEND_ERROR("Error, Expend dimention");
		need_To_Free_Init[2]++;
	}

	
	//malloc memory for the maxtrin with size of n * d.
	if((kdarr->mat = (int**)malloc(sizeof(int*) * dim)) == NULL)
		SEND_ERROR_INIT("Error, Allocation failure");
	need_To_Free_Init[3]++;

	for (i = 0; i < dim; i++)
	{
		if((kdarr->mat[i] = (int*)malloc(sizeof(int) * size)) == NULL)
			SEND_ERROR_INIT("Error, Allocation failure");
		// Sort the points by each Coordinate and return thier order on the first order.
		if((Sorted_Coor_i = spPointSortByIndex(kdarr->copied_arr, i, kdarr->size)) == NULL)
			SEND_ERROR_INIT("Error, Sort by index failure");
		
		need_To_Free_Init[4]++;
		kdarr->mat[i] = Sorted_Coor_i;
	}

	for (i = size - 1; i >= 0; i--)
	{
		// Set the real order of the points at the arr.
		spPointDestroy(kdarr->copied_arr[i]);
		if((kdarr->copied_arr[i] = spPointCopy(arr[i])) == NULL)
			SEND_ERROR_INIT("Error, Allocation failure");
		need_To_Free_Init[4]--;
	}
	return 0;	
}	


void KDArrayDestroy(KDArray* kdarr)
{
	int i;
	for(i = 0; i < kdarr->dim; i++)
		free(kdarr->mat[i]);
	free(kdarr->mat);
	for(i = 0; i < kdarr->size; i++)
		spPointDestroy(kdarr->copied_arr[i]);
	free(kdarr->copied_arr);
	free(kdarr);
}			


int split(KDArray* kdarr, int coor, KDArray* kdleft, KDArray* kdright)
{
	int Just4Free;
	int mid = RND_HALF(kdarr->size);
	int i, j;
	int Place_At_Left_i;
	int Place_At_Right_i;
	int* perm_reverse;
	int* perm;
	if((perm = (int*)malloc(sizeof(int) * kdarr->size)) == NULL)
		SEND_ERROR_SPLIT("Error, Allocation failure");
	need_To_Free_Split[10]++;


	if((perm_reverse = (int*)malloc(sizeof(int) * kdarr->size)) == NULL)
		SEND_ERROR_SPLIT("Error, Allocation failure");
	need_To_Free_Split[11]++;

	for (i = 0; i < kdarr->size; i++)
		perm[i] = kdarr->mat[coor][i];
	
	/* --------------------- For Left ------------------------------------------ */
	if((kdleft = (KDArray*)malloc(sizeof(KDArray))) == NULL)
		SEND_ERROR("Error, Allocation failure");
	need_To_Free_Split[0]++;
	kdleft->dim = kdarr->dim;
	kdleft->size = RND_HALF(kdarr->size);
	
	if((kdleft->copied_arr = (SPPoint**)malloc(kdleft->size * sizeof(SPPoint*))) == NULL)
		SEND_ERROR("Error, Allocation failure");
	need_To_Free_Split[1]++;

	if((kdleft->mat = (int**)malloc(sizeof(kdleft->dim))) == NULL)
		SEND_ERROR("Error, Allocation failure");
	need_To_Free_Split[2]++;

	for(i = 0; i < kdleft->dim; i++){
		if((kdleft->mat[i] = (int*)malloc(sizeof(int) * kdleft->size)) == NULL)
			SEND_ERROR("Error, Allocation failure");	
		need_To_Free_Split[3]++;
	}
	
	/* --------------------- For Right ----------------------------------------- */
	if((kdright = (KDArray*)malloc(sizeof(KDArray))) == NULL)
		SEND_ERROR("Error, Allocation failure");
	need_To_Free_Split[4]++;

	kdright->dim = kdarr->dim;
	kdright->size = kdarr->size - RND_HALF(kdarr->size);
	if((kdright->copied_arr = (SPPoint**)malloc(kdright->size * sizeof(SPPoint*))) == NULL)
		SEND_ERROR("Error, Allocation failure");
	need_To_Free_Split[5]++;

	if((kdright->mat = (int**)malloc(sizeof(kdright->dim))) == NULL)
		SEND_ERROR("Error, Allocation failure");
	need_To_Free_Split[6]++;

	for(i = 0; i < kdright->dim; i++){
		if((kdright->mat[i] = (int*)malloc(sizeof(int) * kdright->size)) == NULL)
			SEND_ERROR("Error, Allocation failure");	
		need_To_Free_Split[7]++;
	}
 	/* ----------------------- Done Allocate ------------------------- */
	
	
	// set the reverse permutation of perm.
	for (i = 0; i < kdarr->size; i++)
		perm_reverse[perm[i]] = i;

	// create both kdleft & kdright matrix field.
	for (i = 0; i < kdarr->dim; i++) // for sort by what coordiante
	{
		Place_At_Left_i = 0;
 		Place_At_Right_i = 0;
		for (j = 0; j < kdarr->size; i++) // to search over all points
		{
			if(perm_reverse[kdarr->mat[i][j]] <= RND_HALF(kdarr->size))
			{
				kdleft->mat[i][Place_At_Left_i] = perm_reverse[kdarr->mat[i][j]];
				Place_At_Left_i++;
			} else {
				kdright->mat[i][Place_At_Right_i] = perm_reverse[kdarr->mat[i][j]] - RND_HALF(kdarr->size);
				Place_At_Right_i++;
			}
		}
	}

	for (i = 0; i < kdleft->size; i++){
		if((kdleft->copied_arr[i] = spPointCopy(kdarr->copied_arr[perm[i]])) == NULL)
			SEND_ERROR_SPLIT("Error, spPOintCopy failue")
		need_To_Free_Split[8]++;
	}

	for (i = 0; i < kdright->size; i++){
		if((kdright->copied_arr[i] = spPointCopy(kdarr->copied_arr[perm[i + RND_HALF(kdarr->size) + 1]])) == NULL)
			SEND_ERROR_SPLIT("Error, spPOintCopy failue")
		need_To_Free_Split[9]++;
	}
	return 0;
}


int GetKDArraySize(KDArray* arr)
{
	if(arr == NULL)
		return -1;
	return arr->size; 
}

int GetKDArrayDim(KDArray* arr)
{
	if(arr == NULL)
		return -1;
	return arr->dim; 
}

SPPoint** GetKDArrayCopied_Arr(KDArray* arr)
{
	if(arr == NULL)
		return NULL;
	return arr->copied_arr;
}

int** GetKDArrayMat(KDArray* arr)
{
	if(arr == NULL)
		return NULL;
	return arr->mat;
}


int main(int argc, char const *argv[])
{
	/* code */
	return 0;
}