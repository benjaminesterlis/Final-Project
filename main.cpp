#ifndef MAIN
#define MAIN

#include "main_aux.h"

#define ddyo printf("%d\n", __LINE__);

using namespace sp;

/********************* MAIN *********************/

int main(int argc, char const *argv[])
{
	int i, j;
	int* indexes = NULL;
	const char* file_name = DEFUALT;
	int ret = 0;
	SP_CONFIG_MSG msg;
	SPPoint*** features = NULL;
	SPConfig conf = NULL;
	SPPoint** total_features = NULL;
	KDArray* arr = NULL;
	int total_n_feature = 0;
	int* num_feat_arr = NULL;
	KDTreeNode** root = NULL;
	int num_of_images;
	int pos = 0;
	int new_n_feat;
	SPBPQueue* bpq = NULL;
	char new_image_path[MAX_PATH_LENGTH] = {0};
	SPPoint** QeuryImage = NULL;
	int* images_indexes = NULL;
	uint64_t* to_sort = NULL;
	int K_close;
	bool is_minimal;
	int KNN_feat;
	char* prefix = NULL;
	char* suffix = NULL;
	char* dir = NULL;
	char* logger_name;
	SP_LOGGER_MSG logger_msg;
	SP_LOGGER_LEVEL logger_level;

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

	conf = spConfigCreate(file_name, &msg);

	if (conf == NULL)
	{
		if (msg ==  SP_CONFIG_CANNOT_OPEN_FILE)
		{
			if (argc == 3)
			{
				SEND_ERROR(INVALID_CONFIG, file_name);
			}
			else
			{
				SEND_ERROR(DEFUALT_CONFIG, file_name);
			}
		}
	}

	/************************ create logger ************************/
	logger_name  = spConfigGetLoggerName(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	logger_level = (SP_LOGGER_LEVEL)spConfigGetLoggerLevel(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	
	logger_msg = spLoggerCreate(logger_name, (SP_LOGGER_LEVEL)logger_level);
	if(logger_msg != SP_LOGGER_SUCCESS)
		goto CLEANUP;
	/***************************************************************/

	KNN_feat = spGetImageKNN(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	prefix = spGetImagePrefix(conf,&msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	suffix = spGetImageSuffix(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	dir = spGetImageDirectory(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	is_minimal = spConfigMinimalGui(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	K_close = spConfigNumOfSimilarImages(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	num_of_images = spConfigGetNumOfImages(conf, &msg);
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	
	{
		ImageProc proc(conf);	

		// check if need to extact data to file
		if (spConfigIsExtractionMode(conf, &msg))
			CHECK_RET(extraction_mode(conf, proc) == 0, EXTRACT_ERROR);
		// craete SPPoints matrix each line for each image
		CHECK_RET(features = (SPPoint***)malloc(sizeof(SPPoint**) * num_of_images), MALLOC_ERROR);
		CHECK_RET(num_feat_arr = (int*)malloc(sizeof(int) * num_of_images), MALLOC_ERROR);
		
		for ( i = 0; i < num_of_images; i++) {
			CHECK_RET(features[i] = read_features(conf, i, num_feat_arr + i), BUILD_ERROR);
			total_n_feature += num_feat_arr[i];
		}
		CHECK_RET(total_features = (SPPoint**)malloc(sizeof(SPPoint*) * total_n_feature), MALLOC_ERROR);

		//convert the matrix to array
		for (i = 0; i < num_of_images; i++)
			for (j = 0; j < num_feat_arr[i]; j++)
			{
				total_features[pos] = features[i][j];
				pos++;
			}

		arr = Init(total_features, total_n_feature);
		CHECK_RET(root = (KDTreeNode**)malloc(sizeof(KDTreeNode*)), MALLOC_ERROR);
		CHECK_NOT(KDTreeInit(arr, root, spConfigGetSplitMethod(conf, &msg), 0), KDTREE_INIT_ERROR);
		CHECK_RET(images_indexes = (int*)malloc(sizeof(int) * num_of_images), MALLOC_ERROR);
		//set the indicator to zero
		for( i = 0; i < num_of_images; i++)
			images_indexes[i] = 0;
		/***************************** QUERY MODE *****************************/
		while (1)
		{
			printf("Please enter an image path:\n");
			scanf("%s", new_image_path);
			
			if(0 == (strcmp(new_image_path, "<>"))){
				printf("%s", EXIT);
				goto CLEANUP;
			}
			
			new_n_feat = spConfigGetNumOfFeatures(conf, &msg);
			MSG_NOT_SUCCESS(msg, "Error, in spConfigGetNumOfFeatures");
			CHECK_RET(QeuryImage = proc.getImageFeatures(new_image_path, 0, &new_n_feat), PROC_ERROR);
			// to choose the closest images
			for ( i = 0; i < new_n_feat; i++)
			{
				indexes = best_indexes(QeuryImage[i], root, bpq, KNN_feat);
				for (j = 0; j < KNN_feat; j++)
					images_indexes[indexes[j]]++;

				free(indexes);
				indexes = NULL;

			}

			CHECK_RET(to_sort = (uint64_t*)malloc(sizeof(uint64_t) * num_of_images), MALLOC_ERROR);
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
			to_sort = NULL;

			/**************************** SHOW RESULT ****************************/
			if (is_minimal)
				ShowMinimalResult(indexes, prefix, suffix, dir, K_close, proc);
			else
				ShowNonMinimalResult(new_image_path, indexes, prefix, suffix, dir , K_close);

			/**************************** Done image ****************************/
			free(indexes);
			indexes = NULL;
			spBPQueueClear(bpq);
			for (i = 0; i < num_of_images; i++)
				images_indexes[i] = 0;
		}
	}

CLEANUP:
	if(total_features != NULL)
	{
		for ( i = 0; i < pos; i++)
			spPointDestroy(total_features[i]);
		free(total_features);
	}
	if (features != NULL)
	{
		for (int i = 0; i < num_of_images; ++i)
			free(features[i]);
		free(features);
	}
	if (images_indexes != NULL)
		free(images_indexes);
	if(num_feat_arr != NULL)
		free(num_feat_arr);
	if (indexes != NULL)
		free(indexes);
	if (to_sort != NULL)
		free(to_sort);
	if (root != NULL){
		if (*(root) != NULL)
			KDTreeNodeDestroy(*(root));
		free(root);
	}
	if (bpq != NULL)
		spBPQueueDestroy(bpq);
	if (conf != NULL)
		spConfigDestroy(conf);
	if (spLoggerIsDefined())
		spLoggerDestroy();

	return ret;
}

#endif
