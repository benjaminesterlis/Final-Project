#include "SPPoint.h"

typedef struct sp_KDArray_t
{
	SPPoint** copied_arr;
	int size, dim;
	int** mat;
} KDArray;

/* Noitce in our implementation we added another Dim to each point
 * which will hold the place of this point in the real order of the points.
 */

/*
 * This is the Init function of KDArray,
 * Initialize the kdarr with values as given
 * @param arr - An array of SPPoint that will be inserted to KDArray.
 * @param size - No. of SPPoints in arr.
 * @param kdarr - An empty poitner which we will insert the new KDArray.  
 * @retrun 0 when succes , -1 otherwise.
 */
KDArray* Init(SPPoint** arr, int size);

/*
 * This is the Destroy function of KDArray,
 * free all memory allocated for KDArray.
 * @param arr - pointer the KDArray needs to free.
 */
void KDArrayDestroy(KDArray* arr);

/*
 * 
 */
KDArray** split(KDArray* kdarr, int coor);

/* 
 * read the function name and you will understand
 */
int GetKDArraySize(KDArray* arr);

/* 
 * read the function name and you will understand
 */
int GetKDArrayDim(KDArray* arr);

/* 
 * read the function name and you will understand
 */
SPPoint** GetKDArrayCopied_Arr(KDArray* arr);

/* 
 * read the function name and you will understand
 */
int** GetKDArrayMat(KDArray* arr);
