#include  "SPConfig.h"
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <math.h>

#ifndef EOL
#define EOL '\n'
#endif

#define SKIP_WHITESPACES  do { \
		while(read == 32/*' '*/) \
			read = getc(fp); \
	} while(0)

#define SKIP_COMMENTS do { \
		if ( read == '#'){ \
			while ((read = getc(fp)) != EOL || read != EOF); \
			if (read == EOF) \
				break; \
			continue; \
		} \
	} while(0)

#define FREE(out_msg) do { \
		spConfigDestroy(conf); \
		close(fp); \
		*msg = out_msg; \
		return NULL; \
	} while(0)

#define CHANGE_CHECK do { \
		for (i = 0; i < 14; i++) \
		{ \
			if ( i < 4 && corr_field_cell[i] == 0 ) \
			{ \
				free(val); \
				printf("File:%s\nLine%d\nMessage:Parameter %s is not set", filename, line, must_param_name[i]); \
				FREE(conf_error[i]); \
			} \
			if( corr_field_cell[i] > 1) \
			{ \
				free(val); \
				FREE(conf_error[5]); \
			} \
		} \
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

typedef enum DKTreeSplitMethods_t {
	MAX_SPRED,
	RANDOM,
	INCREAMENTAL
} SplitMethod;

struct sp_config_t
{
	char *spImagesDirectory;
	char *spImagesPrefix;
	char *spImagesSuffix;
	int spNumOfImages;
	int spPCADimension;
	char *spPCAFilename;
	int spNumOfFeatures;
	bool spExtractionMode;
	int spNumOfSimilarImages;
	SplitMethod spKDTreeSplitMethod;
	int spKNN;
	bool spMinimalGUI; 
	int spLoggerLevel;
	char *spLoggerFilename;
};

/**
 * cell 0 for spImagesPrefix
 * cell 1 for spImgaesSuffix
 * cell 2 for spImagesDirectory
 * cell 3 for spLoggerFilename
 * cell 4 for spPCAFilename
 * need to free all items which thiers cells are true
 */
bool sp_free[5] = {false, false, false, false, false};
/**
 * - cell 0  for spImagesDirectory
 * - cell 1  for spImagesPrefix
 * - cell 2  for spImagesSuffix
 * - cell 3  for spNumOfImages
 * - to know what param name need to retrurn in case not met in conf file
 */
char* must_param_name[4] = {SP_DIR, SP_PRE, SP_SUF, SP_NOI};

SPConfig conf;

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
int corr_field_cell[14]={0};
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
	assert(msg != NULL);
	int line = 0;
	FILE* fp;
	int i = 0;
	char var[200]; //beacuse variables names are no longer then 200 ;)
	char* val = NULL;
	char read;
	SP_CONFIG_MSG* new_msg;
	
	if ( filename == NULL)
	{
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return NULL;
	}

	if ((fp = fopen(filename, O_RONLY))  < 0 )
	{
		*msg = SP_CONFIG_CANNOT_OPEN_FILE;
		return NULL;
	}
	
	if((conf = (SPConfig)malloc(sizeof(SPConfig))) == NULL){
		close(fp);
		*msg = SP_CONFIG_ALLOC_FAIL;
		return NULL;
	}

	conf->spPCADimension = 20;
	conf->spNumOfFeatures = 100;
	conf->spExtractionMode = true;
	conf->spNumOfSimilarImages = 1;
	conf->spKDTreeSplitMethod = MAX_SPRED;
	conf->spKNN = 1;
	conf->spMinimalGUI = false;
	conf->spLoggerLevel = 3;

	if( (conf->spPCAFilename = (char*)malloc(strlen("pca.yml") * sizeof(char))) == NULL )
		FREE(SP_CONFIG_ALLOC_FAIL);
	
	sp_free[4] = true;
	strcpy( conf->spPCAFilename, "pca.yml");

	if( (conf->spLoggerFilename = (char*)malloc(strlen("stdout") * sizeof(char))) == NULL )
		FREE(SP_CONFIG_ALLOC_FAIL);

	sp_free[3] = true;
	strcpy(conf->spLoggerFilename, "stdout");

	while ((read = getc(fp)) != EOF)
	{
		line++;//to see the line in case of error
		// for empty lines
		if(read == EOL)
			continue;

		// for comments
		SKIP_COMMENTS;

		SKIP_WHITESPACES;
		// read till end of variable name 
		while(read != 32/*' '*/ || read != 61/*'='*/ )
		{
			if (97/*'A'*/ < read < 122/*'Z'*/ || 65/*'a'*/ < read < 90/*'z'*/)//only alphabet's chars
			{
				var[i] = read;
				i++;
				read = getc(fp);
			}
			else
			{
				printf("File:%s\nLine%d\nMessage:Invalid configuration line", filename, line);
				FREE(SP_CONFIG_INVALID_STRING);
			}
		}

		SKIP_WHITESPACES;
		//if there are whitespaces in varliabes name
		if ( read != 61/*'='*/) 
		{
			printf("File:%s\nLine%d\nMessage:Invalid configuration line", filename, line);
			FREE(SP_CONFIG_INVALID_STRING);
		}
		
		SKIP_WHITESPACES;
		i = 0;
		//assumed cant be comments after a variable data
		//b3cu453 f0ld3r/f1l3n4m3 c4n c0n741n # 1n 17
		//#w3_4r3_1337 
		//try to decrypt ;)
		while (read != 32/*' '*/ || read != EOL || read != EOF)
		{
			if((val = (char*)realloc(val, i + 1)) == NULL )
				FREE(SP_CONFIG_ALLOC_FAIL);
			val[i] = read;
			i++;
		}

		if((val = (char*)realloc(val, i + 1)) == NULL )
			FREE(SP_CONFIG_INVALID_STRING);
		
		val[i] = 0/* '\0' */;

		SKIP_WHITESPACES;
		if(read == EOL || read == EOF)
		{
			*new_msg = add_field_to_struct(var, val, i + 1, filename, line);
			if (*new_msg != SP_CONFIG_SUCCESS){
				free(val);
				spConfigDestroy(conf);
				close(fp);
				msg = new_msg;
				return NULL;
			}
		}
		else
		{
			printf("File:%s\nLine%d\nMessage:Invalid configuration line", filename, line);
			free(val);
			FREE(SP_CONFIG_INVALID_STRING);
		}
	}	

	// for error SP_CONFIG_DOULBE_USED_VAR & any SP_CONFIG in the conf_error 
	CHANGE_CHECK;
	
	return conf;

}

bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg)
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

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg)
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

int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg)
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

int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg)
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

int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg)
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

SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config,
		int index)
{
	int i;
	if (conf == NULL || imagePath == NULL )
		return SP_CONFIG_INVALID_ARGUMENT;

	if( conf->spNumOfImages <= index)
		return SP_CONFIG_INDEX_OUT_OF_RANGE;
	
	/* assmued sprintf always works */
	sprintf(imagePath, "%s%s%d%s", conf->spImagesDirectory, conf->spImagesPrefix,
									index, conf->spImagesSuffix);
	return SP_CONFIG_SUCCESS;
}

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config)
{
	int i;
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
	if(sp_free[0])
		free(config->spImagesPrefix);
	if(sp_free[1])
		free(config->spImagesSuffix);
	if(sp_free[2])
		free(config->spImagesDirectory);
	if(sp_free[3])
		free(config->spLoggerFilename);
	if(sp_free[4])
		free(config->spPCAFilename);
	free(config);
}

SP_CONFIG_MSG add_field_to_struct(char* var, char* val, int n, const char* filename, int line)
{	
	char *temp;
	if(val == NULL || var == NULL)
		return SP_CONFIG_INVALID_STRING;
	
	// spImagesDirectory
	if(strcmp(SP_DIR,var) == 0)
	{	
		corr_field_cell[0]++;
		if((conf->spImagesDirectory = realloc(conf->spImagesDirectory,n)) == NULL )
			return SP_CONFIG_ALLOC_FAIL;
		sp_free[2] = true;
		strncpy(conf->spImagesDirectory ,val, n);
		return SP_CONFIG_SUCCESS;
	}

	// spImagesSuffix
	if(strcmp(SP_SUF,var) == 0 )
	{
		corr_field_cell[2]++;
		if((conf->spImagesSuffix = realloc(conf->spImagesSuffix,n)) == NULL )
			return SP_CONFIG_ALLOC_FAIL;
		sp_free[1] = true;
		conf->spImagesSuffix = val;
		return SP_CONFIG_SUCCESS;
	}

	// spImagesPrefix
	if(strcmp(SP_PRE,var) == 0 )
	{
		corr_field_cell[1]++;
		if((conf->spImagesPrefix = realloc(conf->spImagesPrefix,n)) == NULL )
			return SP_CONFIG_ALLOC_FAIL;
		sp_free[0] = true;
		conf->spImagesPrefix = val;
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
		if((conf->spPCAFilename = (char*)realloc(conf->spPCAFilename, 
									sizeof(val)*sizeof(char))) == NULL )
			return SP_CONFIG_ALLOC_FAIL;
		sp_free[4] = true;
		conf->spPCAFilename = val;
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
		if(strcmp(val, "MAX_SPRED") == 0)
		{
			conf->spKDTreeSplitMethod = MAX_SPRED;
			return SP_CONFIG_SUCCESS;
		}
		if(strcmp(val,"INCREAMENTAL") == 0)
		{
			conf->spKDTreeSplitMethod = INCREAMENTAL;
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
		if((conf->spLoggerFilename = (char*)realloc(conf->spLoggerFilename, 
								sizeof(val)*sizeof(char))) == NULL )
			return SP_CONFIG_ALLOC_FAIL;
		sp_free[4] = true;
		return SP_CONFIG_SUCCESS;
	}
	printf("File:%s\nLine%d\nInvalid value-constraint not met", filename, line);
	return SP_CONFIG_WRONG_FIELD_NAME;
}

int main(int argc, char const *argv[])
{
	return 0;
}