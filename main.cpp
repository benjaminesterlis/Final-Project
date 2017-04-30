#include "main_aux.h"

using namespace sp;
/********************* MAIN *********************/

int main(int argc, char const *argv[])
{
	ImageProc proc;
	const char* file_name = DEFUALT;
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
	SPPoint** QeuryImage = NULL;
	int* images_indexes = NULL;
	int K_close;
	bool is_minimal;

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
		
	is_minimal = spConfigMinimalGui(conf, msg)
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	K_close = spConfigNumOfSimilarImages(conf, msg)
	MSG_NOT_SUCCESS(msg, NULL_POINTER_ERROR);
	num_of_images = spConfigGetNumOfImages(conf, msg);
	MSG_NOT_SUCCESS(msg, SP_CONFIG_SUCCESS);

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
