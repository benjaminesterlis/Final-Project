#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stirng,h>
#include <errno.h>
#include "SPConfig" 

#define C "-c"
#define DEFUALT "spcbir.config"
#define CONFIG ".config"
#define CONF_SIZE 7
#define INVALID_COMMAND "invalid command line: use -c <config_filename>"
#define INVALID_CONFIG "invlaid configuration file: %s couldn't be open"
#define DEFUALT_CONFIG "The default configuration file %s couldn’t be open"
#define FEAT ".feat"
#define O_RDONLY "r"
#define O_WRONLY "w"
#define O_RDWR "r+"

#define SEND_ERROR( fmt, ...) \
do { \
	printf(fmt, ##__VA__ARGS__); \
	ret = -1; /* the return value of the main fucntion :)*/ \
	goto CLEANUP; \
} while(0)

#define CHECK_RET(cond, ...) \
do { \
	if (!(cond)) \
		SEND_ERROR("%s" ,##__VA__ARGS__); \
} while(0)

#define MSG_NOT_SUCCESS(msg, error) 
do { \
	if (msg != SP_CONFIG_SUCCESS) \
		SEND_ERROR("%s", error); \
} while(0)

#define CHECK_MSG_RET(cond, msg , val_msg) \
do { \
	if(!(cond)) \
	{ \
		msg = val_msg; \
		SEND_ERROR(""); \
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
	char* file_name = DEFUALT;
	FILE* conf_file;
	int ret = 0;
	SP_CONFIG_MSG msg;
	const SPConfig confing = NULL;
	if (argc != 3 && argc != 1)
		SEND_ERROR("%s", INVALID_COMMAND);

	if (argc == 3)
	{
		CHECK_RET(strcmp(argv[1], C), "ERROR, strcmp");
		CHECK_RET(check_file_name(argv[2]), "ERROR, file format is *.config");	
		file_name = argv[2];
	}
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

	 CHECK_RET(extraction_mode(conf, msg));

CLEANUP:
	return ret;
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

int extraction_mode(const SPConfig conf, SP_CONFIG_MSG msg)
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
		features = getImageFeatures(img_path, i, &num_of_features);
		
		// open feature file.
		sprintf(feature_path, "%s%s%d%s",dir, suffix, i, FEAT);
		OPEN(image_file, feature_path, O_WRONLY, msg, SP_CONFIG_CANNOT_OPEN_FILE);

		//write each feature
		fprintf(image_file, "%s$\n", num_of_features); 
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

	sprintf(buf, "%d@%d@", spPointGetDimension(p), spPointGetIndex(p));
	for( printf(""),i = 0; i < dim; i++)
		read = sprintf(buf, "%s%lf?",buf, spPointGetAxisCoor(p, i));

	return read;
}