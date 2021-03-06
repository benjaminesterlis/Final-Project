#ifndef KDTREE_H
#define KDTREE_H


#include "SPConfig.h"
#include "KDArray.h"
#include <time.h>


typedef struct sp_KDTreeNode_t{
	struct sp_KDTreeNode_t* left;
	struct sp_KDTreeNode_t* right;
	int dim;
	double* val;
	SPPoint* data;
} KDTreeNode;

typedef SplitMethod KDTreeSplitMethod;


int KDTreeInit (KDArray* arr, KDTreeNode** root, 
				KDTreeSplitMethod split_method, int upper_level_dim);

int MAX_SPREAD_Split( KDArray* arr);

int getKDTreeDim (KDTreeNode* node);

double* getKDTreeVal (KDTreeNode* node);

KDTreeNode* getKDTreeLeft(KDTreeNode* node);

KDTreeNode* getKDTreeRight(KDTreeNode* node);

SPPoint* getKDTreeData(KDTreeNode* node);

void KDTreeNodeDestroy(KDTreeNode* node);

#endif
