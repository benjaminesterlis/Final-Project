#include "main_aux.h"
#include "KNN_Search.h"
#include <stdint.h>


#define NOT_SUCCESS(msg) { \
	if(msg != SP_CONFIG_SUCCESS) \
	{ \
		ret = -1; \
		goto CLEANUP; \
	} \
}

#define NOT_SUCCESS_NULL(msg) { \
	if(msg != SP_CONFIG_SUCCESS) \
	{ \
		ret = NULL; \
		goto CLEANUP; \
	} \
}


#define CHECK_ONLY(exp, val) \
{ \
	if ((exp)) \
	{ \
		ret = val; \
		goto CLEANUP; \
	} \
}

#define CHECK_NULL(exp) { \
	if((exp) == NULL) \
		goto CLEANUP; \
}

// the last letter must be .config which is 7
// check if the type is .config type	
int check_file_name(const char* file_name)
{
	int size = strlen(file_name);
	if (strcmp(CONFIG, file_name + size - CONF_SIZE) == 0)
		return 0;
	return -1;
}

int get_least_ms_dword(long qword)
{	
	long mask = -1;
	mask = (mask >> 32);
	return (int)(qword & mask);
}

uint64_t extend(uint32_t most_seg_dword, uint32_t least_seg_dword)
{
	uint64_t l = most_seg_dword;
	l = l << 32;
	l += least_seg_dword;
	return l;
}

int _mine_cmp(const void* a, const void *b)
{
	uint32_t aa = (uint32_t)(((*(uint64_t *)a) >> 32) & 0xffffffff);
	uint32_t bb = (uint32_t)(((*(uint64_t *)b) >> 32) & 0xffffffff);
   	// printf("a:%d\tb:%d\n", (int32_t)aa, (int32_t)bb);
   	return (int)( (int64_t)bb - (int64_t)aa);
}

int extraction_mode(const SPConfig conf, sp::ImageProc proc)
{
	int ret = 0;
	int i, j, k;
	char* dir;
	char* suffix;
	char* prefix;
	int num_of_image;
	SP_CONFIG_MSG msg;
	char img_path[MAX_PATH_LENGTH] = {0}; 
	int num_of_features;
	FILE* image_file = NULL;
	char feature_path[MAX_PATH_LENGTH] = {0};
	FILE* feature_file = NULL;
	SPPoint** features= NULL;
	int dim;
	double coor_val;
	int index;
	int feature_flag = 0; // to indicate if files are open
	int image_flag = 0; // to indicate if files are open

	num_of_image = spConfigGetNumOfImages(conf, &msg);
	NOT_SUCCESS(msg);
	prefix = spGetImagePrefix(conf, &msg);
	NOT_SUCCESS(msg);
	suffix = spGetImageSuffix(conf, &msg);
	NOT_SUCCESS(msg);
	dir = spGetImageDirectory(conf,&msg);
	NOT_SUCCESS(msg);
	for (i = 0; i < num_of_image; i++)
	{

		feature_flag = 0;
		sprintf(img_path, "%s%s%d%s", dir, prefix, i, suffix);
 		// open img file
		CHECK_NULL(image_file = fopen(img_path, O_RDONLY));
		// make features
		CHECK_NULL(features = proc.getImageFeatures(img_path, i, &num_of_features));
 		fclose(image_file);
 		image_flag = 1;
 		sprintf(feature_path, "%s%s%d%s", dir, prefix, i, FEAT);
 		// creatre the feature file
 		CHECK_NULL(feature_file = fopen(feature_path, O_WRONLY_TRUNC));
		dim = spPointGetDimension(features[0]);
		index = spPointGetIndex(features[0]);
 		// start write to file
 		CHECK_ONLY((fwrite((void*)&num_of_features, sizeof(int), 1, feature_file) != 1), -1);
 		// assumed for every image all features got the same index and  dim.
 		CHECK_ONLY((fwrite((void*)&dim, sizeof(int), 1, feature_file) != 1), -1);
 		CHECK_ONLY((fwrite((void*)&index, sizeof(int), 1, feature_file) != 1), -1);
 		for (j = 0; j < num_of_features; j++)
 		{
 			//write the data in the file
 			for ( k = 0; k < dim; k++)
 			{
 				coor_val = spPointGetAxisCoor(features[j], k);
 				CHECK_ONLY((fwrite((void*)&coor_val, sizeof(double), 1, feature_file) != 1), -1);
 			}
		}
		fclose(feature_file);
		feature_flag = 1;
	}

	CLEANUP:
	if(feature_flag == 0)
		fclose(feature_file);
	if(image_flag == 0)
		fclose(image_file);

	return ret;
}

/*
 * retrun a buffer with data input
 * follows the format:
 * "dim index data[0] data[1]....data[dim-1]"
 */
int magic_func(SPPoint* p, char* buf)
{
	int dim = spPointGetDimension(p);
	int i;
	int read;
 	buf[0] = '\0';
	for(i = 0; i < dim; i++){
		printf("%s\titer:%d\n", buf, i);
		read = sprintf(buf, "%s%lf",buf, spPointGetAxisCoor(p, i));
	}
	return read;
}

// take index of image and return an array of her features (SPPoints)
// num is the number of features in the image.feat
SPPoint** read_features(const SPConfig conf, int index, int* num)
{
	char* dir;
	char* prefix;
	int i;
	unsigned int dim;
	int index_got;
	char dot_feat_path[MAX_PATH_LENGTH];
	FILE* feature_file;
	double* data = NULL;
	SPPoint** ret = NULL;
	int ret_len = 0;
	SP_CONFIG_MSG msg;
	prefix = spGetImagePrefix(conf, &msg);
	NOT_SUCCESS_NULL(msg);
	dir = spGetImageDirectory(conf,&msg);
	NOT_SUCCESS_NULL(msg);

	//create the file path
	sprintf(dot_feat_path, "%s%s%d%s", dir, prefix, index, FEAT);
	// open feat file
	CHECK_NULL(feature_file = fopen(dot_feat_path, O_RDONLY));
	// read nubmer of features
	CHECK_ONLY((fread((void*)num, sizeof(int), 1, feature_file) != 1), NULL);
	// read the dim 
	CHECK_ONLY((fread((void*)&dim, sizeof(int), 1, feature_file) != 1), NULL);
	// read the index
	CHECK_ONLY((fread((void*)&index_got, sizeof(int), 1, feature_file) != 1), NULL);

	// *************** check if index is corret , need? ***************
	CHECK_ONLY((index != index_got), NULL);
	// ****************************************************************

	data = (double*)malloc(sizeof(double) * dim);
	// read the features, array of doubles
	CHECK_NULL(ret = (SPPoint**)malloc(sizeof(SPPoint*) * (*num)));
	for (i = 0; i < (*num); i++)
	{
		CHECK_ONLY((fread((void*)data, sizeof(double), dim, feature_file) != dim), NULL);	
		CHECK_NULL(ret[i] = spPointCreate(data, dim, index));
		ret_len++;	
	}
	if(data != NULL)
		free(data);
	return ret;
	
CLEANUP:
	if (ret != NULL)
	{
		for( i = 0; i < ret_len; i++)
			spPointDestroy(ret[i]);
		free(ret);
	}
	if(data != NULL)
		free(data);
	return ret;
}


int* best_indexes(SPPoint* feature, KDTreeNode** curr, SPBPQueue* bpq, int size)
{
	int i;
	BPQueueElement elem;
	int* indexes = NULL;
	int ret = 0;
	CHECK_RET( indexes = (int*)malloc(sizeof(int) * size), MALLOC_ERROR);
	CHECK_RET( kNearestNeighbors((*curr), &bpq, feature, size),KNN_ERROR);
	for (i = 0; i < size; i++){
		spBPQueuePeek(bpq, &elem);
		indexes[i] = spBPQueueElementGetIndex(&elem);
	}
	ret++;
CLEANUP:
	return indexes;
}

void ShowMinimalResult( int* pic_indexes, char* prefix, char* suffix, char* dir, int size, sp::ImageProc proc)
{
	char image_path[MAX_PATH_LENGTH];
	int i;
	for(i = 0; i < size; i++)
	{
		sprintf(image_path, "%s%s%d%s", dir, prefix, pic_indexes[i], suffix);
		proc.showImage(image_path);
	}
}

void ShowNonMinimalResult(char* q_image_path, int* pic_indexes, char* prefix, char* suffix, char* dir, int size)
{
	int i;
	
	printf("Best candidates for %s are:\n", q_image_path);
	for (i = 0; i < size; ++i)
		printf("%s%s%d%s\n", dir, prefix, pic_indexes[i], suffix );
}

void PrintToLogger(const char* message, const char* file,
		const char* function, const int line)
{
	switch (SPGetLogggerLevel())
	{
		case SP_LOGGER_ERROR_LEVEL:
			spLoggerPrintError(message, file, function, line);
			break;
		case SP_LOGGER_WARNING_ERROR_LEVEL:
			spLoggerPrintWarning(message, file, function, line);
			break;
		case SP_LOGGER_INFO_WARNING_ERROR_LEVEL:
			spLoggerPrintInfo(message);
			break;
		case SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL:
			spLoggerPrintDebug(message, file, function, line);
			break;
		default:
			spLoggerPrintMsg(message);
			break;
	}
}