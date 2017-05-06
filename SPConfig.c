#include "SPConfig.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#ifndef EOL
#define EOL '\n'
#endif

#define SKIP_WHITESPACES  do { \
		while(read == 32/*' '*/ || read == 9/*'\t'*/ || read == 13/*'\r'*/) \
			read = getc(fp); \
	} while(0)

#define SKIP_COMMENTS do { \
		if ( read == '#'){ \
			cont++; \
			do { \
				read = getc(fp); \
			} while ( read != EOL && read != EOF); \
		} \
	} while(0);

#define SET_MESSAGE_RET(msg, msg_val, ret_val) \
do { \
	msg = msg_val; \
	ret = ret_val; \
	goto CLEANUP; \
} while(0)

#define CHECK(cond,func) \
do { \
	if(!(cond)) \
	{ \
		func; \
		goto CLEANUP; \
	} \
} while(0)

#define CHECK_C(cond,func) \
do { \
	if(!(cond)) \
	{ \
		func; \
		continue; \
	} \
	else { \
		goto CLEANUP; \
	} \
} while(0)

#define SET_MESSAGE_RETURN(msg, msg_val, ret_val) \
do { \
	msg = msg_val; \
	return ret_val; \
} while(0)

#define SET_MESSAGE_CLEAN(msg, msg_val) \
do{ \
	*(msg) = msg_val; \
	goto CLEANUP; \
} while(0)

#define O_RONLY "r"
#define	SP_DIR "spImagesDirectory"
#define	SP_PRE "spImagesPrefix"
#define	SP_SUF "spImagesSuffix"
#define	SP_NOI "spNumOfImages"
#define	SP_PCAD "spPCADimension"
#define	SP_PCAF "spPCAFilename"
#define	SP_NOF "spNumOfFeatures"
#define	SP_EXM "spExtractionMode"
#define	SP_NOFSI "spNumOfSimilarImages"
#define	SP_KDTSM "spKDTreeSplitMethod"
#define	SP_KNN "spKNN"
#define	SP_MGUI "spMinimalGUI"
#define	SP_LL "spLoggerLevel"
#define	SP_LF "spLoggerFilename"

static SP_CONFIG_MSG add_field_to_struct(SPConfig conf, char var[MAX_LEN], char val[MAX_LEN], int n, const char* filename, int line, int corr_field_cell[14]);



/**
 * - cell 0  for spImagesDirectory
 * - cell 1  for spImagesPrefix
 * - cell 2  for spImagesSuffix
 * - cell 3  for spNumOfImages
 * - to know what param name need to retrurn in case not met in conf file
 */
char* must_param_name[4] = {SP_DIR, SP_PRE, SP_SUF, SP_NOI};

/**
 * Fit SP_CONFIG_MSG error to his cell
 * We need the errors only of the first 4 variables beasue 
 * other got defualt value, so they must not be change
*/
SP_CONFIG_MSG conf_error[5]={SP_CONFIG_MISSING_DIR, SP_CONFIG_MISSING_PREFIX,
						SP_CONFIG_MISSING_SUFFIX, SP_CONFIG_MISSING_NUM_IMAGES,
						SP_CONFIG_DOUBLE_USED_VAR};

SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg)
{
	/** 
	 * - cell 0  for spImagesDirectory
	 * - cell 1  for spImagesPrefix
	 * - cell 2  for spImagesSuffix
	 * - cell 3  for spNumOfImages
	 * - cell 4 for spPCADimension
	 * - cell 5 for spPCAFilename
	 * - cell 6 for spNumOfFeatures
	 * - cell 7 for spExtractionMode
	 * - cell 8 for spNumOfSimilarImages
	 * - cell 9 for spKDTreeSplitMethod
	 * - cell 10 for spKNN
	 * - cell 11 for spMinimalGUI
	 * - cell 12 for spLoggerLevel
	 * - cell 13 for spLoggerFilename
	 * - cell value is 0 if not changed
	 * - every time we changed each varilabe we increment the corresponded cell
	 */
	int corr_field_cell[14] = {0};
	int cont = 0;
	SPConfig ret = NULL;
	SPConfig conf = NULL;
	assert(msg != NULL);
	int line = 0;
	FILE* fp = NULL;
	int i = 0;
	char var[MAX_LEN] = {0}; //beacuse variables names are no longer then 200 ;)
	char val[MAX_LEN] = {0};
	char read;
	SP_CONFIG_MSG* new_msg = NULL;

	*msg = SP_CONFIG_SUCCESS;

	CHECK(filename, SET_MESSAGE_RET(*msg, SP_CONFIG_INVALID_ARGUMENT, NULL));
	CHECK(fp = fopen(filename, O_RONLY), SET_MESSAGE_RET(*msg, SP_CONFIG_CANNOT_OPEN_FILE, NULL));
	CHECK(conf = (SPConfig)malloc(sizeof(struct sp_config_t)), SET_MESSAGE_RET(*msg, SP_CONFIG_ALLOC_FAIL, NULL));
	CHECK(new_msg = (SP_CONFIG_MSG*)malloc(sizeof(SP_CONFIG_MSG)), SET_MESSAGE_RET(*msg, SP_CONFIG_ALLOC_FAIL, NULL));

	conf->spPCADimension = 20;
	conf->spNumOfFeatures = 100;
	conf->spExtractionMode = true;
	conf->spNumOfSimilarImages = 1;
	conf->spKDTreeSplitMethod = MAX_SPREAD;
	conf->spKNN = 1;
	conf->spMinimalGUI = false;
	conf->spLoggerLevel = 3;
	memset(conf->spPCAFilename, 0, MAX_LEN);
	memset(conf->spLoggerFilename, 0, MAX_LEN);
	strcpy(conf->spPCAFilename, "pca.yml");
	strcpy(conf->spLoggerFilename, "stdout");

	while ((read = getc(fp)) != EOF)
	{
		cont = 0;
		i=0;
		line++;//to see the line in case of error

		SKIP_WHITESPACES;
		
		if(read == EOL) // for empty lines
			continue;

		// for comments
		SKIP_COMMENTS;

		if(cont)
			continue;

		SKIP_WHITESPACES;
		// read till end of variable name 
		while(read != 32/*' '*/ && read != 61/*'='*/ && read != 9/*'\t'*/ && read != 13/*'\r'*/)
		{
			if ((97/*'A'*/ <= read && read <= 122/*'Z'*/ )|| (65/*'a'*/ <= read && read <= 90/*'z'*/))//only alphabet's chars
			{
				var[i] = (char)read;
				i++;
				read = getc(fp);
			}
			else
			{
				printf("File:%s\nLine:%d\nMessage:Invalid configuration line\n", filename, line);
				SET_MESSAGE_RET(*msg, SP_CONFIG_INVALID_STRING, NULL);
			}
		}
		var[i] = 0 /* '\0' */;
		SKIP_WHITESPACES;
		//if there are whitespaces in varliabes name
		if ( read != 61/*'='*/) 
		{
			printf("File:%s\nLine:%d\nMessage:Invalid configuration line\n", filename, line);
			SET_MESSAGE_RET(*msg, SP_CONFIG_INVALID_STRING, NULL);
		}
		read = getc(fp);
		SKIP_WHITESPACES;
		i = 0;
		//assumed cant be comments after a variable data
		//b3cu453 f0ld3r/f1l3n4m3 c4n c0n741n # 1n 17
		//#w3_4r3_1337 
		//try to decrypt ;)
		while (read != 32/*' '*/ && read != 9/*'\t'*/ && read != 13/*'\r'*/ && read != EOL && read != EOF)
		{
			val[i] = (char)read;
			i++;
			read = getc(fp);
		}

		val[i] = 0 /* '\0' */;
		
		SKIP_WHITESPACES;
		SKIP_COMMENTS;
		if(read == EOL || read == EOF)
		{
			*new_msg = add_field_to_struct(conf, var, val, i + 1, filename, line, corr_field_cell);
			*msg = *new_msg;
			CHECK_C(*new_msg != SP_CONFIG_SUCCESS, i++);
		}
		else
		{
			printf("File:%s\nLine:%d\nMessage:Invalid configuration line\n", filename, line);
			goto CLEANUP;
		}
    }

    //tp check if we insert one object twice.
    for (i = 0; i < 14; i++)
	{
        if (i < 4 && corr_field_cell[i] == 0)
        	SET_MESSAGE_CLEAN(msg, conf_error[i]);
        if(corr_field_cell[i] > 1)
        	SET_MESSAGE_CLEAN(msg, SP_CONFIG_DOUBLE_USED_VAR);
    }

    //In case we succed to make a confing.
    ret = conf;

    if (fp != NULL) {
        fclose(fp);
    }
	if (new_msg != NULL)
		free(new_msg);
    return ret;

CLEANUP:
	if (fp != NULL)
		fclose(fp);
	if (conf != NULL)
		free(conf);
	if (new_msg != NULL)
		free(new_msg);
	return ret;
}

bool spConfigIsExtractionMode(const SPConfig conf, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (conf == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return conf->spExtractionMode;

}

bool spConfigMinimalGui(const SPConfig conf, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (conf == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return conf->spMinimalGUI;
}

int spConfigGetNumOfImages(const SPConfig conf, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (conf == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return conf->spNumOfImages;
}

int spConfigGetNumOfFeatures(const SPConfig conf, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (conf == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return conf->spNumOfFeatures;
}

int spConfigGetPCADim(const SPConfig conf, SP_CONFIG_MSG* msg)
{
	assert(msg != NULL);
	if (conf == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return -1;
	}
	*msg = SP_CONFIG_SUCCESS;
	return conf->spPCADimension;
}

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig conf,
		int index)
{
	if (conf == NULL || imagePath == NULL )
		return SP_CONFIG_INVALID_ARGUMENT;

	if( conf->spNumOfImages <= index)
		return SP_CONFIG_INDEX_OUT_OF_RANGE;



	/* assmued sprintf always works */
	sprintf(imagePath, "%s%s%d%s", conf->spImagesDirectory, conf->spImagesPrefix,
									index, conf->spImagesSuffix);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig conf)
{
	if (conf == NULL || pcaPath == NULL )
		return SP_CONFIG_INVALID_ARGUMENT;
	/* assmued sprintf always works */
	sprintf(pcaPath, "%s%s", conf->spImagesDirectory, conf->spPCAFilename);
	return SP_CONFIG_SUCCESS;
}

void spConfigDestroy(SPConfig config)
{
	if (config == NULL)
		return;
	free(config);
}


/**
 * this function get two strings ($var, $val) and set
 * SPConfig.$var = $val
 * @retrun
 * - SP_CONFIG_INVALID_INTEGER - in case type of $var is int and $val is string
 * - SP_CONFIG_INVALID_STRING - in case there is no such field name $var in SPConfig
 * - SP_CONFIG_INVALID_BOOLEAN - in case type of var is boolean and val is not
 * - SP_CONFIG_INVLAID_ENUM - in case string is not one of enum
 * - SP_CONFIG_INVLAID_FIELD_NAME - in case ther is no field named $var
 * - SP_CONDIF_SUCCEES - otherwise
 */
static
SP_CONFIG_MSG add_field_to_struct(SPConfig conf, char var[MAX_LEN], char val[MAX_LEN], int n, const char* filename, int line, int corr_field_cell[14])
{	
	char *temp;
	if(val == NULL || var == NULL)
		return SP_CONFIG_INVALID_STRING;
	
	// spImagesDirectory
	if(strcmp(SP_DIR,var) == 0)
	{	
		corr_field_cell[0]++;
		strncpy(conf->spImagesDirectory ,val, n);
		return SP_CONFIG_SUCCESS;
	}

	// spImagesPrefix
	if(strcmp(SP_PRE,var) == 0)
	{
		corr_field_cell[1]++;
		strncpy(conf->spImagesPrefix ,val, n);	
		return SP_CONFIG_SUCCESS;
	}
	// spImagesSuffix
	if(strcmp(SP_SUF,var) == 0 )
	{
		corr_field_cell[2]++;
		strncpy(conf->spImagesSuffix ,val, n);
		return SP_CONFIG_SUCCESS;
	}

	
	// spNumOfImages
	if(strcmp(SP_NOI,var) == 0 )
	{
		corr_field_cell[3]++;
		conf->spNumOfImages = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spPCADimension
	if(strcmp(SP_PCAD,var) == 0 )
	{
		corr_field_cell[4]++;
		conf-> spPCADimension = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spPCAFilename
	if(strcmp(SP_PCAF,var) == 0 )
	{
		corr_field_cell[5]++;
		strcpy(conf->spPCAFilename, val);
		return SP_CONFIG_SUCCESS;
	}
	// spNumOfFeatures
	if(strcmp(SP_NOF,var) == 0 )
	{
		corr_field_cell[6]++;
		conf->spNumOfFeatures = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}
	// spExtractionMode
	if(strcmp(SP_EXM,var) == 0 )
	{
		corr_field_cell[7]++;
		if (strcmp(val,"true") == 0)
		{ 
			conf->spExtractionMode = true;
			return SP_CONFIG_SUCCESS;
		}
		if  (strcmp(val,"false") == 0)
		{
			conf->spExtractionMode = false;
			return SP_CONFIG_SUCCESS;
		}
		return SP_CONFIG_INVALID_BOOLEAN;
	}

	// spNumOfSimilarImages
	if(strcmp(SP_NOFSI,var) == 0 )
	{
		corr_field_cell[8]++;
		conf->spNumOfSimilarImages = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spKDTreeSplitMethod
	if(strcmp(SP_KDTSM,var) == 0 )
	{
		corr_field_cell[9]++;
		if(strcmp(val, "RANDOM") == 0)
		{
			conf->spKDTreeSplitMethod = RANDOM;	
			return SP_CONFIG_SUCCESS;
		}
		if(strcmp(val, "MAX_SPREAD") == 0)
		{
			conf->spKDTreeSplitMethod = MAX_SPREAD;
			return SP_CONFIG_SUCCESS;
		}
		if(strcmp(val,"INCREMENTAL") == 0)
		{
			conf->spKDTreeSplitMethod = INCREMENTAL;
			return SP_CONFIG_SUCCESS;
		}
		return SP_CONFIG_INVALID_ENUM;
	}

	// spKNN
	if(strcmp(SP_KNN,var) == 0 )
	{
		corr_field_cell[10]++;
		conf->spKNN = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}
	
	// spMinimalGUI
	if(strcmp(SP_MGUI,var) == 0 )
	{
		corr_field_cell[11]++;
		if (strcmp(val,"true") == 0)
		{
			conf->spMinimalGUI = true;
			return SP_CONFIG_SUCCESS;
		}
		if  (strcmp(val,"false") == 0)
		{
			conf->spMinimalGUI = false;
			return SP_CONFIG_SUCCESS;
		}
		return SP_CONFIG_INVALID_BOOLEAN;
	}

	// spLoggerLevel
	if(strcmp(SP_LL,var) == 0 )
	{
		corr_field_cell[12]++;
		conf->spLoggerLevel = (int)strtol(val,&temp,10);
		if(*temp)
			return SP_CONFIG_INVALID_INTEGER;
		return SP_CONFIG_SUCCESS;
	}

	// spLoggerFilename
	if(strcmp(SP_LF,var) == 0 )
	{
		corr_field_cell[13]++;
		strcpy(conf->spLoggerFilename, val);
		return SP_CONFIG_SUCCESS;
	}
	printf("File:%s\nLine:%d\nInvalid value-constraint not met\n", filename, line);
	return SP_CONFIG_WRONG_FIELD_NAME;
}

char* spGetImagePrefix(const SPConfig config, SP_CONFIG_MSG* msg)
{
	char* ret;
    ret = NULL;
	CHECK(config, SET_MESSAGE_RETURN(*msg, SP_CONFIG_NULL_POINTER, NULL));
	return config->spImagesPrefix;
CLEANUP:
	return ret;
}


char* spGetImageSuffix(const SPConfig config, SP_CONFIG_MSG* msg)
{
	char* ret;
    ret = NULL;
	CHECK(config, SET_MESSAGE_RETURN(*msg, SP_CONFIG_NULL_POINTER, NULL));
	return config->spImagesSuffix;
CLEANUP:
	return ret;
}

char* spGetImageDirectory(const SPConfig config, SP_CONFIG_MSG* msg)
{
	char* ret;
    ret = NULL;
	CHECK(config, SET_MESSAGE_RETURN(*msg, SP_CONFIG_NULL_POINTER, NULL));
	return config->spImagesDirectory;
CLEANUP:
	return  ret;
}

int spGetImageKNN(const SPConfig config, SP_CONFIG_MSG* msg)
{
	int ret;
    ret = 0;
	CHECK(config, SET_MESSAGE_RETURN(*msg, SP_CONFIG_NULL_POINTER, -1));
	return config->spKNN;
CLEANUP:
	return ret;
}

SplitMethod spConfigGetSplitMethod(const SPConfig config, SP_CONFIG_MSG* msg)
{
	SplitMethod ret;
    ret = 0;
	CHECK(config, SET_MESSAGE_RETURN(*msg, SP_CONFIG_NULL_POINTER, -1));
	return config->spKDTreeSplitMethod;
CLEANUP:
	return ret;
}

int spConfigNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg)
{
	int ret;
    ret = 0;
	CHECK(config, SET_MESSAGE_RETURN(*msg, SP_CONFIG_NULL_POINTER, -1));
	return config->spNumOfSimilarImages;
CLEANUP:
	return ret;
}

void spPrintConfig(const SPConfig config)
{
	if (config == NULL)
		exit(-1);
	printf("spImagesDirectory:__%s__\n",config->spImagesDirectory );
	printf("spImagesPrefix:__%s__\n", config->spImagesPrefix);
	printf("spImagesSuffix:__%s__\n", config->spImagesSuffix);
	printf("spNumOfImages:__%d__\n", config->spNumOfImages);
	printf("spPCADimension:__%d__\n", config->spPCADimension);
	printf("spPCAFilename:__%s__\n", config->spPCAFilename);
	printf("spNumOfFeatures:__%d__\n", config->spNumOfFeatures);
	printf("spExtractionMode:__%d__\n", config->spExtractionMode);
	printf("spNumOfSimilarImages:__%d__\n", config->spNumOfSimilarImages);
	printf("spKDTreeSplitMethod:__%d__\n", config->spKDTreeSplitMethod);
	printf("spKNN:__%d__\n", config->spKNN);
	printf("spMinimalGUI:__%d__\n", config->spMinimalGUI);
	printf("spLoggerLevel:__%d__\n", config->spLoggerLevel);
	printf("spLoggerFilename:__%s__\n", config->spLoggerFilename);
}
