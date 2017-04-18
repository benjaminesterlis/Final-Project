#include "KDArray.h"

typedef struct sp_KDTreeNode_t KDTreeNode;

typedef enum sp_KDTreeSplitMethod_t
{
	MAX_SPREAD,
	RANDOM,
	INCREMENTAL,
} KDTreeSplitMethod;


int KDTreeInit (KDArray* arr, KDTreeNode* root, KDTreeSplitMethod split_method)

int MAX_SPREAD_Split( KDArray* arr);