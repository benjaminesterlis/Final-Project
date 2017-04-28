#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include "SPConfig.h"
#include "SPImageProc.h"

#define C "-c"
#define DEFUALT "spcbir.config"
#define CONFIG ".config"
#define CONF_SIZE 7
#define INVALID_COMMAND "invalid command line: use -c <config_filename>\n"
#define INVALID_CONFIG "invlaid configuration file: %s couldn't be open\n"
#define DEFUALT_CONFIG "The default configuration file %s couldn’t be open\n"
#define FEAT ".feat"
#define O_RDONLY "r"
#define O_WRONLY "w"
#define O_RDWR "r+"
#define EXTRACT_ERROR "Extraction Error!\n"
#define BUILD_ERROR "Build Data instrcution Error!\n"
#define READ_ERROR "Reading Error!\n"
#define OPEN_ERROR "Opening Error!\n"
#define MALLOC_ERROR "Allocation Error!\n"
#define KDTREE_INIT_ERROR "KDTreeInit Error!\n"
#define EXIT "Exting...\n"
#define SRTCMP_ERROR "Strcmp Error!\n"
#define WRONG_FORMAT_ERROR "file is not .config Error!\n"
#define NULL_POINTER_ERROR "NULL pointer Error!\n"

#define SEND_ERROR( fmt, ...) \
do { \
	printf(fmt, __VA__ARGS__); \
	ret = -1; /* the return value of the main fucntion :)*/ \
	goto CLEANUP; \
} while(0)

#define CHECK_RET(cond, ...) \
do { \
	if (!(cond)) \
		SEND_ERROR("%s" , __VA__ARGS__); \
} while(0)

#define CHECK_NOT(cond, ...) CHECK_RET(!(cond), __VA__ARGS__)

#define MSG_NOT_SUCCESS(msg, error) \
do { \
	if (msg) \
		SEND_ERROR("%s", error); \
} while(0)

#define CHECK_MSG_RET(cond, msg , val_msg) \
do { \
	if(!(cond)) \
	{ \
		msg = val_msg; \
		SEND_ERROR("",""); \
	} \
} while(0)

#define OPEN(fstream, path, prem, msg, msg_val) \
do { \
	CHECK_MSG_RET(fstream = fopen(path, prem), msg, val_msg); \
} while(0)

#define READ(fstream, buffer, size, nitem, msg, val_msg) \
do { \
	CHECK_MSG_RET(fread(buffer, size, nitem, fstream), msg , val_msg); \
} while(0);

#define WRITE(fstream, buffer, size, nitem, msg, val_msg) \
do { \
	CHECK_MSG_RET(fwrite(buffer, size, nitem, fstream) < (size_t)nitem, msg, val_msg); \
} while(0)


/********************* MAIN *********************/

int main(int argc, char const *argv[])
{
	ImageProc proc = NULL;
	char* file_name = DEFUALT;
	FILE* conf_file = NULL;
	FILE* new_image_file = NULL;
	int ret = 0;
	SP_CONFIG_MSG msg = NULL;
	SPPoint*** features = NULL;
	int points_done = 0;
	const SPConfig confing = NULL;
	SPPoint** total_features = NULL;
	int total_n_feature = 0;
	int* num_feat_arr = NULL;
	KDTreeNode* root = NULL;
	int num_of_images;
	int pos = 0;
	int flag = 0;
	int new_n_feat;
	char* new_image_path;
	SPBPQueue* bqp = NULL;
	SPPOint** QeuryImage = NULL;
	int* images_indexes = NULL;
	int K_close;
	bool is_minimal;

	is_minimal = spConfigMinimalGui(conf, msg)
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	K_close = spConfigNumOfSimilarImages(conf, msg)
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);

	num_of_images = spConfigGetNumOfImages(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	/********************** Start **********************/
	if (argc != 3 && argc != 1)
		SEND_ERROR("%s", INVALID_COMMAND);

	if (argc == 3)
	{
		CHECK_RET(strcmp(argv[1], C), SRTCMP_ERROR);
		CHECK_RET(check_file_name(argv[2]), WRONG_FORMAT_ERROR);	
		file_name = argv[2];
	}

	/**********************INIT from  Config **********************/


	conf = spConfigCreate(conf_file, msg);
	if (conf == NULL)
	{
		switch msg
		{
			case SP_CONFIG_CANNOT_OPEN_FILE:
				if (argc == 3) 
					SEND_ERROR(INVALID_CONFIG, file_name); 
				else 
					SEND_ERROR(DEFUALT_CONFIG, file_name); 
				break;
		}
	}
	fclose(conf_file);
		
	proc = ImageProc(conf);

	// check if need to extact data to file
	if (spConfigIsExtractionMode(conf, msg))
	 	CHECK_RET(extraction_mode(conf), EXTRACT_ERROR);

	// craete SPPoints matrix each line for each image
	CHECK_RET(faetures = (SPPoint***)malloc(sizeof(SPPoint**) * num_of_images), MALLOC_ERROR);

	for ( i = 0; i < num_of_images; i++){
	 	CHECK_RET(features[i] = read_features(conf, i, &num_feat_arr), BUILD_ERROR);
	 	total_n_feature += num_feat_arr[i];
	}

	CHECK_RET(total_features = (SPPoint**)malloc(sizeof(SPPoint*) * total_n_feature), MALLOC_ERROR);
	
	//convert the matrix to array
	for(i = 0; i < num_of_images; i++)
		for(j = 0; j < num_feat_arr[i]; j++, pos++)
			total_features[pos] = features[i][j];
	flag = 1;

	CHECK_NOT(KDTreeInit(total_features, root, spConfigGetSplitMethod(conf), 0), KDTREE_INIT_ERROR);

	CHECK_RET(images_indexes = (int*)malloc(sizeof(int) * num_of_images), MALLOC_ERROR);

	/***************************** QUERY MODE *****************************/
	while(1)
	{
		printf("Please enter an image path:\n");
		scanf("%s",new_image_path);
		CHECK_RET(strcmp(new_image_path, "<>"), EXIT);
		new_n_feat = spConfigGetNumOfFeatures(conf, msg)
		MSG_NOT_SUCCESS (msg, "Error, in spConfigGetNumOfFeatures");
		QeuryImage = getImageFeatures(new_image_path, -1, &new_n_feat);

		// to choose the closest images
		for ( i = 0; i < new_n_feat; i++){
			indexes = best_indexes(QeurtImage[i], root, bpq, KNN_features);
			for (j = 0; j < KNN_features; j++)
				images_indexes[indexes[j]]++;

			free(indexes);
		}

		CHECK_RET(to_sort = (long*)malloc(sizeof(long) * num_of_images), MALLOC_ERROR);
		CHECK_RET(indexes = (int*)malloc(sizeof(int) * K_close), MALLOC_ERROR);
	
		// unsigned long mask = -1 //0xffffffffffffffff
		// set integers for special sorting style	
		for (i = 0; i < num_of_images; ++i)
			to_sort[i] = extend(images_indexes[i], i);

		// take the KNN closest images
		qsort(to_sort, num_of_images, sizeof(long), _mine_cmp);
		// take only k_close images as we need
		for (i = 0; i < K_close; ++i)
			indexes[i] = get_least_ms_dword(to_sort[i]);
		free(to_sort);

		/**************************** SHOW RESULT ****************************/

		if(is_minimal)
			ShowMinimalResult(indexes, prefix, suffix, dir, K_close, proc);
		else
			ShowNonMinimalResult(new_image_path, indexes, prefix, suffix, dir , K_close);
	
		/**************************** Done image ****************************/
	}

CLEANUP:
	// if(flag == 1)
	// {
	// 	for ( i = 0; i < pos; i++)
	// 		spPointDestroy(total_features[pos]);
	// 	free(total_features);
	// 	free(features);
	// } 
	// else
	// {

	// }
	return ret;
}

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
int check_file_name(char* file_name)
{
	int size = strlen(file_name);
	if (strcmp(CONFIG, file_name + size - CONF_SIZE) == 0)
		return 0;
	return -1;
}

int extraction_mode(const SPConfig conf, ImageProc proc)
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
	char* file_name = NULL;
	char* prefix;
	char* suffix;
	char* dir;
	int num_of_features;
	int i,j;
	char* feature_path;
	char* data;
	int size_data;
	FILE* image_file;
	SPPoint** features;

	num_of_images = spConfigGetNumOfImages(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	num_of_features = spConfigGetNumOfFeatures(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	prefix = spGetImagePreffix(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	suffix = spGetImageSuffix(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	dir = spGetImageDirectory(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	// for each image will create .feat file
	for(i = 0; i < num_of_images; i++)
	{
		// open image file & get features
		sprintf(img_path, "%s%s%d%s",dir, suffix, i, prefix);
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
	int index = spPointGetIndex(p);
	int i;
	int read;

	for( printf(""),i = 0; i < dim; i++)
		read = sprintf(buf, "%s%lf?",buf, spPointGetAxisCoor(p, i));

	return read;
}

SPPoint** read_features(const SPConfig conf, int index, int* num)
{
	int ret = 0;
	int j;
	int dim, index;
	int num_of_images;
	FILE* feature_file;
	char* prefix;
	char* dir;
	char* path;
	SPPoint** features;
	SP_CONFIG_MSG msg;

	prefix = spGetImagePreffix(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	dir = spGetImageDirectory(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

	// create the path to the file to open
	sprintf(path, "%s%s%d%s", dir, prefix, index, FEAT);

	// open file
	CHECK_RET( OPEN(feature_file, path, O_RDONLY ,msg , NULL), OPEN_ERROR);

	// first dword is num_of features
	CHECK_RET(READ(feature_file, &num_of_features, sizeof(int), 1, msg, NULL), READ_ERROR);  
	
	// second dword is dim
	CHECK_RET(READ(feature_file, &dim, sizeof(int), 1, msg, NULL), READ_ERROR);

	// third dword is index
	CHECK_RET(READ(feature_file, &index, sizeof(int), 1, msg, NULL), READ_ERROR);

	// malloc featuers array
	CHEK_RET(features = (SPPoint**)malloc(sizeof(SPPoint*) * num_of_features), MALLOC_ERROR);


	for ( j = 0; j < num_of_features; j++)
	{
		CHECK_RET(READ(feature_file, &data, sizeof(double), dim, msg, NULL), READ_ERROR);
		faetures[j] = spPointCreate(data, dim, index);
		*num++;
	}

CLEANUP:
	if (!feature_file)
		close(feature_file);
	if (ret == -1)
		return NULL;
	return features;
}


int* best_indexes(SPPoint* feature, KDTreeNode* curr, SPBPQueue* bpq, int size)
{
	int ret = 0;
	int i;
	SP_BPQUEUE_MSG msg;
	BPQueueElement* elem;

	CHECK_RET( indexes = (int*)malloc(sizeof(int) * size), MALLOC_ERROR);
	
	CHECK_RET(kNearestNeighbors(root, bpq, feature, size),"");
	for (i = 0; i < size; i++){
		msg = spBPQueuePeek(bpq, elem);
		indexes[i] = spBPQueueElementGetIndex(elem);
	}

CLEANUP:
	return ret;
}

int _mine_cmp(const void* a, const void *b)
{
	int* aa;
	int* bb;
	*aa = (int)(*(long *) a >> 32));
	*bb = (int)(*(long *) b >> 32));
   	return ( *(int*)a - *(int*)b );
}

void ShowMinimalResult(int* pic_indexes, char* prefix, char* suffix, char* dir, int size, ImageProc proc)
{
	char* image_path;
	int i;
	for( printf(""), i = 0; i < size, i++)
	{
		sprintf(image_path, "%s%s%d%s", dir, prefix, pic_indexes[i], suffix);
		proc.ShowImage(image_path);
	}
}

void ShowNonMinimalResult(char* q_image_path, int* pic_indexes, char* prefix, char* suffix, char* dir, int size)
{
	int i;
	char* image_path;
	
	printf("%s\n", q_image_path);
	for (i = 0; i < size; ++i)
		printf("%s%s%d%s\n", dir, prefix, pic_indexes[i], suffix );
}