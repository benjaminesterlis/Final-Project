#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stirng,h>
#include "SPConfig" 

#define C "-c"
#define DEFUALT "spcbir.config"
#define CONFIG ".config"
#define CONF_SIZE 7
#define INVALID_COMMAND "invalid command line: use -c <config_filename>"
#define INVALID_CONFIG "invlaid configuration file: %s couldn't be open"
#define DEFUALT_CONFIG "The default configuration file %s couldn’t be open"

#define SEND_ERROR( fmt, ...) \
do { \
	printf(fmt, ##__VA__ARGS__); \
	ret = -1; /* the return value of the main fucntion :)*/ \
	goto CLEANUP; \
} while(0)

#define CHECK_ZERO(number, ...) \
do { \
	if (!nubmer) \
		SEND_ERROR("%s" ,##__VA__ARGS__); \
} while(0)

#define OPEN_FAIL(file_name) \
do { \
	if (flag == 3) \
		SEND_ERROR(INVALID_CONFIG, file_name); \
	else \
		SEND_ERROR(DEFUALT_CONFIG, file_name); \
}while(0)

// for help methods
#define MSG_NOT_SUCCESS(msg,error) { \
	if (msg != SP_CONFIG_SUCCESS) \
		return -1; \
}

// for main
#define MSG_SUCCESS_MAIN(msg, error) { \
	if (msg != SP_CONFIG_SUCCESS) \
		SEND_ERROR("%s", error); \
}

int main(int argc, char const *argv[])
{
	char* file_name = DEFUALT;
	FILE* conf_file;
	int ret;
	SP_CONFIG_MSG msg;
	const SPConfig confing = NULL;
	if (argc != 3 && argc != 1)
		SEND_ERROR("%s", INVALID_COMMAND);

	if (argc == 3)
	{
		CHECK_ZERO(strcmp(argv[1], C), "ERROR, strcmp");
		CHECK_ZERO(check_file_name(argv[2]), "ERROR, file format is *.config");	
		file_name = argv[2];
	}
	conf = spConfigCreate(conf_file, msg);
	if (conf == NULL)
	{
		switch msg
		{
			case SP_CONFIG_CANNOT_OPEN_FILE:
				OPEN_FAIL(conf_file);
				break;
		}
	}

	extract_feats_to_file(conf, msg);

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

int extract_feats_to_file(const SPConfig conf, SP_CONFIG_MSG msg;)
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

int num_of_images;

	num_of_images = spConfigGetNumOfImages(conf, msg);

}
