#include "main_aux.h"
#include "KNN_Search.h"

int get_least_ms_dword(long qword)
{	
	unsigned long mask = -1;
	mask = (mask >> 32);
	return (int)(qword & mask);
}

long extend(int most_seg_dword, int least_seg_dword)
{
	long l = (long)most_seg_dword;
	l = l << 32;
	l += least_seg_dword;
	return l;
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

int extraction_mode(const SPConfig conf, sp::ImageProc proc)
{

	/* need to open new files .feat and copy 
	 * feats to it according to our data
	 * file place is: spImagesDirectroy.
	 * prefix is: spImagesPrefix.
	 * suffix is: spImagesSuffix.
	 * no. of images: spNumOfImages.
	 * no. of features: soNumOfFeatuers.
	 * all of these are fields in SPCofing conf.
	*/
	SP_CONFIG_MSG msg;
	int ret = 0;
	int num_of_images;
	char* suffix= NULL;
	char* dir= NULL;
	int num_of_features;
	int i,j;
	char* feature_path= NULL;
	char* data= NULL;
	int size_data;
	FILE* image_file= NULL;
	SPPoint** features= NULL;
	char img_path[MAX_PATH_LENGTH];

	num_of_images = spConfigGetNumOfImages(conf, &msg);
	MSG_NOT_SUCCESS(msg, NOT_SUCCESS);

	num_of_features = spConfigGetNumOfFeatures(conf, &msg);
	MSG_NOT_SUCCESS(msg, NOT_SUCCESS);

	suffix = spGetImageSuffix(conf, &msg);
	MSG_NOT_SUCCESS(msg, NOT_SUCCESS);

	dir = spGetImageDirectory(conf, &msg);
	MSG_NOT_SUCCESS(msg, NOT_SUCCESS);

	// for each image will create .feat file
	for(i = 0; i < num_of_images; i++)
	{
		// open image file & get features
		spConfigGetImagePath(img_path, conf, i);
		features = proc.getImageFeatures(img_path, i, &num_of_features);
		
		// open feature file.
		sprintf(feature_path, "%s%s%d%s",dir, suffix, i, FEAT);
		OPEN(image_file, feature_path, O_WRONLY, msg, SP_CONFIG_CANNOT_OPEN_FILE);

		// each image will have her own file with the following format:
		// numOfFeatures # dimForAllPoints @ IndexForAllPoints
		fprintf(image_file, "%d", num_of_features);
		sprintf(data, "%d%d", spPointGetDimension(features[i]), spPointGetIndex(features[i]));

		// write each feature
		for (j = 0; j < num_of_features; j++)
		{
			size_data = magic_func(features[i], data);
			WRITE(image_file, data, size_data, 1, msg,SP_CONFIG_WRITE_FAILURE); 
		}
	}

CLEANUP:
	return ret;
}

/*
 * retrun a buffer with data input
 * follows the format:
 * "dim@index@data[0]?data[1]?....data[dim-1]?"
 */
int magic_func(SPPoint* p, char* buf)
{
	int dim = spPointGetDimension(p);
	int i;
	int read;

	for(i = 0; i < dim; i++)
		read = sprintf(buf, "%s%lf?",buf, spPointGetAxisCoor(p, i));

	return read;
}

SPPoint** read_features(const SPConfig conf, int index, int* num)
{
	int num_of_features;
	int ret = 0;
	int j;
	int dim;
	FILE* feature_file= NULL;
	char* prefix= NULL;
	char* dir= NULL;
	char* path = NULL;
	double* data= NULL;
	SPPoint** features= NULL;
	SP_CONFIG_MSG msg;


	prefix = spGetImagePreffix(conf, &msg);
	MSG_NOT_SUCCESS(msg, NOT_SUCCESS);

	dir = spGetImageDirectory(conf, &msg);
	MSG_NOT_SUCCESS(msg, NOT_SUCCESS);

	// create the path to the file to open
	sprintf(path, "%s%s%d%s", dir, prefix, index, FEAT);

	// open file
	OPEN(feature_file, path, O_RDONLY ,msg , SP_CONFIG_WRITE_FAILURE);
	CHECK_RET(feature_file, OPEN_ERROR);

	// first dword is num_of features
	READ(feature_file, &num_of_features, sizeof(int), 1, msg, SP_CONFIG_WRITE_FAILURE); 
	CHECK_RET(feature_file, READ_ERROR);
	
	// second dword is dim
	READ(feature_file, &dim, sizeof(int), 1, msg, SP_CONFIG_WRITE_FAILURE);
	CHECK_RET(feature_file, READ_ERROR);

	// third dword is index
	READ(feature_file, &index, sizeof(int), 1, msg, SP_CONFIG_WRITE_FAILURE);
	CHECK_RET(feature_file, READ_ERROR);	

	// malloc featuers array
	CHECK_RET(features = (SPPoint**)malloc(sizeof(SPPoint*) * num_of_features), MALLOC_ERROR);

	CHECK_RET(data = (double*) malloc(sizeof(double) * dim ), MALLOC_ERROR);

	for ( j = 0; j < num_of_features; j++)
	{
		READ(feature_file, data, sizeof(double), dim, msg, SP_CONFIG_WRITE_FAILURE);
		CHECK_RET(feature_file, READ_ERROR);

		features[j] = spPointCreate(data, dim, index);
		*num = *num+1;
	}

CLEANUP:
	if (!feature_file)
		fclose(feature_file);
	if (ret == -1)
		return NULL;
	return features;
}


int* best_indexes(SPPoint* feature, KDTreeNode* curr, SPBPQueue* bpq, int size)
{
	int i;
	BPQueueElement* elem= NULL;
	int * indexes = NULL;
	int ret=0;

	CHECK_RET( indexes = (int*)malloc(sizeof(int) * size), MALLOC_ERROR);
	
	CHECK_RET(kNearestNeighbors(curr, bpq, feature, size),"");
	for (i = 0; i < size; i++){
		spBPQueuePeek(bpq, elem);
		indexes[i] = spBPQueueElementGetIndex(elem);
	}
	ret ++;
CLEANUP:
	return indexes;
}

int _mine_cmp(const void* a, const void *b)
{
	int aa;
	int bb;
	aa = (int)(*(long *) a >> 32);
	bb = (int)(*(long *) b >> 32);
   	return ( aa - bb );
}

void ShowMinimalResult( int* pic_indexes, char* prefix, char* suffix, char* dir, int size, sp::ImageProc proc)
{
	char* image_path = NULL;
	int i;
	// sp::ImageProc imageProc = sp::ImageProc(conf);
	for(i = 0; i < size; i++)
	{
		sprintf(image_path, "%s%s%d%s", dir, prefix, pic_indexes[i], suffix);
		proc.showImage(image_path);
	}
}

void ShowNonMinimalResult(char* q_image_path, int* pic_indexes, char* prefix, char* suffix, char* dir, int size)
{
	int i;
	
	printf("%s\n", q_image_path);
	for (i = 0; i < size; ++i)
		printf("%s%s%d%s\n", dir, prefix, pic_indexes[i], suffix );
}
