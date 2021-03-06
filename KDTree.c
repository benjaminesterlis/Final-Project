#include "KNN_Search.h" 
#include <stdio.h>
#include <stdlib.h>


#define LINE printf("~~~~~~~~~~~~~~~~~~~~ %d ~~~~~~~~~~~~~~~~~~~~\n", __LINE__);
#define RND_HALF(num) ((num % 2 == 0) ? (num / 2) : (num / 2 + 1))

#define COOR_VAL_BY_ORDER(arr, mat, dim, rank) \
	spPointGetAxisCoor((GetKDArrayCopied_Arr(arr))[mat[dim][(rank)]], dim + 1)

#define SPREAD_BY_COOR(arr, mat, dim, first, last)  \
	COOR_VAL_BY_ORDER(arr, mat, dim, last) - COOR_VAL_BY_ORDER(arr, mat, dim, first) 

#define KD_S(check, kdside, root_side, method, dim, root) \
	if((check = KDTreeInit(kdside, root_side, method, dim)) != 0) \
	{ \
		free(root); \
		return -1; \
	}
#define ROOT_SET(root, dim1, val1, left1, right1, data1, arr) \
do { \
	(root)->dim = dim1; \
	(root)->val = val1; \
	(root)->left = left1; \
	(root)->right = right1; \
	(root)->data = (data1); \
	KDArrayDestroy(arr); \
	return 0; \
} while(0)

#define SEND_ERROR(error) { printf("Error!, line: %d, %s\n", __LINE__, error); return -1;} 


int KDTreeInit (KDArray* arr, KDTreeNode** root, 
				KDTreeSplitMethod split_method, int upper_level_dim)
{
	int split_dim;
	int size;
	int check;
	KDArray** total;
    double med_val;
	double* med_val_pointer;

	if(arr == NULL)
		return -1;

	if((size = GetKDArraySize(arr)) == 0)
		return -1;
	

	if ((*root = (KDTreeNode*)malloc(sizeof(KDTreeNode))) == NULL)
		return -1;
	
	
	if(size == 1)
		ROOT_SET(*root, -1, NULL, NULL, NULL, spPointCopy(GetKDArrayCopied_Arr(arr)[0]), arr);

	if((med_val_pointer = (double*)malloc(sizeof(double))) == NULL){
		free(root);
		return 1;
	}

	switch (split_method)
	{
		case MAX_SPREAD :
			split_dim = MAX_SPREAD_Split(arr);
			break;
		case RANDOM :
			srand(time(NULL));
			split_dim = rand() % GetKDArrayDim(arr);
			break;
		case INCREMENTAL :
			split_dim =  (upper_level_dim + 1) % GetKDArrayDim(arr);	
	}

	med_val = COOR_VAL_BY_ORDER(arr, arr->mat, split_dim, RND_HALF(size) - 1); 
	// printf("%lf\n", med_val);
	*med_val_pointer = med_val;
	// printf("%lf\n", *med_val_pointer);
 
	total = split(arr,split_dim);
 	KD_S(check, total[0], &(*root)->left, split_method, upper_level_dim + 1, *root);	
 	KD_S(check, total[1], &(*root)->right, split_method, upper_level_dim + 1, *root);
 	// printf("%lf\n", *med_val_pointer);	
 	free(total);
	ROOT_SET(*root, split_dim, med_val_pointer, (*root)->left, (*root)->right, NULL, arr);

}

int MAX_SPREAD_Split( KDArray* arr)
{
	int i;
	int** mat = GetKDArrayMat(arr);
	int size = GetKDArraySize(arr);
	int dim = GetKDArrayDim(arr);
	double spread[dim]; // need to malloc instead
	double max;
	int max_i;
	if ( arr == NULL)
		return -1;

	for (i = 0; i < dim; i++)
		spread[i] = SPREAD_BY_COOR(arr, mat, i, 0, size - 1);
	
	max = spread[0];
	max_i = 0;

	for (i = 1; i < dim; i++)
		if(spread[i] > max)
		{
			max = spread[i];
			max_i = i;
		}
	return max_i;
}

int getKDTreeDim (KDTreeNode* node)
{
	if (node == NULL)
		return -1;
	return node->dim;
}

double* getKDTreeVal (KDTreeNode* node)
{
	if (node == NULL)
		return NULL;
	return 	node->val;
}

KDTreeNode* getKDTreeLeft(KDTreeNode* node)
{
	if (node == NULL)
		return NULL;
	return node->left;
}


KDTreeNode* getKDTreeRight(KDTreeNode* node)
{
	if (node == NULL)
		return NULL;
	return node->right;
}

SPPoint* getKDTreeData(KDTreeNode* node)
{
	if (node == NULL)
		return NULL;
	return node->data;
}

void KDTreeNodeDestroy(KDTreeNode* node){
	if (NULL == node)
		return;

	if(NULL != node->data)
		spPointDestroy(node->data);

	if (NULL != node->val)
		free(node->val);
	
	if(NULL != node->right)
		KDTreeNodeDestroy(node->right);
	
	if(NULL != node->left)
		KDTreeNodeDestroy(node->left);
	
	free(node);
}
