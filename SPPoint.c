#include "SPPoint.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

struct sp_point_t
{
	double* data;
	int dim;
	int index;
};

int SortIndex; // Global Var to sort the SPPoints array by this coordinate

void copy(double *dest, double *src, int range);

SPPoint* spPointCreate(double* data, int dim, int index) 
{
	if (dim <= 0 || index < 0 || data == NULL)
		return NULL;
	SPPoint *sp = malloc(sizeof(*sp));
	if (sp == NULL)
		return NULL;
	if ((sp->data = (double *)malloc(sizeof(double) * dim)) == NULL){
		free(sp);
		return NULL; 
	}
	copy(sp->data, data, dim);
	sp->dim = dim; // (*sp).dim = dim
	sp->index = index; // (*sp).index = index
	return sp;
}

SPPoint* spPointCopy(SPPoint* source) 
{
	assert (source != NULL);
	SPPoint *sp = malloc(sizeof(*source));
	if (sp == NULL)
		return NULL;
	if ((sp->data = (double *)malloc(sizeof(double) * source->dim)) == NULL){
		free(sp);
		return NULL;
	}
	copy(sp->data, source->data, source->dim);
	sp->dim = source->dim;
	sp->index = source->index;
	return sp;
}


void spPointDestroy(SPPoint* point) 
{	
	if (point != NULL)
	{
		free(point->data);
		free(point);
	}

}

int spPointGetDimension(SPPoint* point)
{
	assert (point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint* point) 
{
	assert (point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint* point, int axis) 
{
	assert (point != NULL || axis < point->dim);
	return point->data[axis-1];
}

double spPointL2SquaredDistance(SPPoint* p, SPPoint* q) 
{
	int i;
	double dist = 0;
	assert (p!=NULL && q!=NULL && p->dim == q->dim);

	for (i = 0; i < p->dim; ++i)
	{	
		dist += pow((p->data[i])-(q->data[i]),2);
		//printf("%f %d\n",p->data[i],i); // why the last cell in the array is 0
	}
	return dist;
}

void copy(double *dest, double *src, int range)
{
	int i;
	for (i = 0; i < range; ++i)
	{
		dest[i] = src[i];
	}
}

SPPoint* ExpendDim(SPPoint* p, double val)
{
	if (p == NULL)
		return NULL;

	SPPoint* q;
	int dim, index;
	double* expend_data;

	dim = spPointGetDimension(p);
	index = spPointGetIndex(p);
	
	if((expend_data = (double*)malloc(sizeof(double) * (dim + 1))) == NULL){
		spPointDestroy(p);
		return NULL;
	}
	copy(expend_data, p->data, dim);
	expend_data[dim] = val;
	spPointDestroy(p);

	if((q = spPointCreate(expend_data, dim + 1, index)) == NULL){
		free(expend_data);
		return NULL;
	}
	free(expend_data);
	return q;
}


int Mine_Cmp(const void* a, const void *b){ 
	return (int)(*(SPPoint**)a)->data[SortIndex] - (int)(*(SPPoint**)b)-> data[SortIndex];
}



int* spPointSortByIndex(SPPoint** arr, int index, int size)
{
	int i;
	int* indexes;
	SortIndex = index;
	if (arr == NULL)
		return NULL;
	if((indexes = (int*)malloc(sizeof(int) * size)) == NULL)
		return NULL;
	qsort(arr, 5, sizeof(SPPoint*), Mine_Cmp);
	for (i = 0; i < 5; i++)
		indexes[i] = (int)spPointGetAxisCoor(arr[i],arr[i]->dim);
	return indexes;
} 

SPPoint* DecreaseDim(SPPoint* p)
{
	if(p == NULL)
		return NULL;
	p->dim--;
	if((p->data = realloc(p->data, p->dim)) == NULL)
		return NULL;
	return p;
}
