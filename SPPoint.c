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

void copy(double *, double *, int range);

SPPoint* spPointCreate(double* data, int dim, int index) 
{
	if (dim <= 0 || index < 0 || data == NULL)
		return NULL;
	SPPoint *sp = malloc(sizeof(*sp));
	if (sp == NULL)
		return NULL;
	if ((sp->data = (double *)malloc(sizeof(double) * dim)) == NULL)
		return NULL; 
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
	if ((sp->data = (double *)malloc(sizeof(double) * source->dim)) == NULL)
		return NULL;
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
	assert (point!=NULL || axis < point->dim);
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
